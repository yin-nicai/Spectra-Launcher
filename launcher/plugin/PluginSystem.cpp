// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "PluginSystem.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSet>

#include "directory/DirectoryStrategy.h"
#include "download/DownloadSources.h"
#include "metadata/MetadataSources.h"
#include "resources/ResourceProviders.h"
#include "settings/SettingsObject.h"

namespace {

QList<Plugins::PluginState> statesFromJson(const QJsonObject& root, const QString& key)
{
    QList<Plugins::PluginState> states;
    const auto entries = root.value(key).toArray();
    states.reserve(entries.size());
    for (const auto& entryValue : entries) {
        const auto entry = entryValue.toObject();
        const auto id = entry.value(QStringLiteral("id")).toString();
        if (id.isEmpty()) {
            continue;
        }
        states.append({
            id,
            entry.value(QStringLiteral("enabled")).toBool(true),
            entry.value(QStringLiteral("order")).toInt(states.size() * 10),
            entry.value(QStringLiteral("options")).toObject().toVariantMap(),
        });
    }
    return states;
}

template <typename Interface>
QJsonArray statesToJson(const Plugins::PluginRegistry<Interface>& registry)
{
    QJsonArray entries;
    for (const auto& state : registry.currentStates()) {
        entries.append(QJsonObject{
            { QStringLiteral("id"), state.id },
            { QStringLiteral("enabled"), state.enabled },
            { QStringLiteral("order"), state.order },
            { QStringLiteral("options"), QJsonObject::fromVariantMap(state.options) },
        });
    }
    return entries;
}

}  // namespace

namespace Plugins {

PluginSystem::PluginSystem(SettingsObject* settings, QObject* parent) : QObject(parent), m_settings(settings)
{
    registerBuiltins();
    reloadConfiguration();
}

void PluginSystem::registerBuiltins()
{
    m_metadataSources.registerFactory({
        QStringLiteral("prism-meta"),
        tr("Spectra metadata"),
        tr("Use the metadata server configured by Spectra."),
        true,
        [](const QVariantMap&) { return std::make_unique<PrismMetadataSource>(); },
    });
    m_metadataSources.registerFactory({
        ConfiguredMetadataSource::sourceId(),
        tr("Additional HTTP metadata"),
        tr("Query user-configured metadata mirrors in order. The JSON format is unchanged."),
        false,
        [](const QVariantMap& options) { return std::make_unique<ConfiguredMetadataSource>(options); },
    });

    m_downloadSources.registerFactory({
        QStringLiteral("mojang-direct"),
        tr("Mojang official downloads"),
        tr("Only permit Minecraft client, library, and asset URLs hosted by Mojang."),
        true,
        [](const QVariantMap&) { return std::make_unique<MojangDownloadSource>(); },
    });
    m_downloadSources.registerFactory({
        QStringLiteral("priority-mirrors"),
        tr("Minecraft mirror priority"),
        tr("Try the official URL first, then configured foreign mirrors, then configured domestic mirrors."),
        true,
        [](const QVariantMap& options) { return std::make_unique<PriorityDownloadSource>(options); },
    });

    m_directoryStrategies.registerFactory({
        QStringLiteral("prism-directories"),
        tr("Spectra directory strategy"),
        tr("Resolve instance, game, asset, and log directories with legacy-compatible defaults and explicit overrides."),
        true,
        [](const QVariantMap&) { return std::make_unique<PrismDirectoryStrategy>(); },
    });

    registerBuiltinResourceProviders(m_resourceProviders);
}

void PluginSystem::reloadConfiguration()
{
    const auto text = m_settings->get(QString::fromLatin1(SETTING_ID)).toString();
    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(text.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || (!document.isNull() && !document.isObject())) {
        qWarning() << "Invalid plugin configuration, using defaults:" << error.errorString();
    }
    applyConfiguration(document.isObject() ? document.object() : QJsonObject{}, false);
}

void PluginSystem::applyConfiguration(const QJsonObject& configuration, bool persist)
{
    m_metadataSources.configure(statesFromJson(configuration, QStringLiteral("metadataSources")));
    m_downloadSources.configure(statesFromJson(configuration, QStringLiteral("downloadSources")));
    m_directoryStrategies.configure(statesFromJson(configuration, QStringLiteral("directoryStrategies")));
    m_uiExtensions.configure(statesFromJson(configuration, QStringLiteral("uiExtensions")));
    m_resourceProviders.configure(statesFromJson(configuration, QStringLiteral("resourceProviders")));

    if (persist) {
        const auto serialized = QJsonDocument(this->configuration()).toJson(QJsonDocument::Compact);
        m_settings->set(QString::fromLatin1(SETTING_ID), QString::fromUtf8(serialized));
    }
    emit configurationChanged();
}

QJsonObject PluginSystem::configuration() const
{
    return {
        { QStringLiteral("metadataSources"), statesToJson(m_metadataSources) },
        { QStringLiteral("downloadSources"), statesToJson(m_downloadSources) },
        { QStringLiteral("directoryStrategies"), statesToJson(m_directoryStrategies) },
        { QStringLiteral("uiExtensions"), statesToJson(m_uiExtensions) },
        { QStringLiteral("resourceProviders"), statesToJson(m_resourceProviders) },
    };
}

QList<MetadataCandidate> PluginSystem::metadataCandidates(const MetadataRequest& request) const
{
    QList<MetadataCandidate> result;
    QSet<QString> seen;
    for (const auto* source : m_metadataSources.enabledInOrder()) {
        for (auto candidate : source->query(request)) {
            if (!candidate.url.isValid() || candidate.url.url().isEmpty()) {
                continue;
            }
            if (candidate.sourceId.isEmpty()) {
                candidate.sourceId = source->id();
            }
            const auto normalized = candidate.url.toString();
            if (seen.contains(normalized)) {
                continue;
            }
            seen.insert(normalized);
            result.append(std::move(candidate));
        }
    }
    return result;
}

QList<DownloadCandidate> PluginSystem::downloadCandidates(const DownloadRequest& request) const
{
    const DownloadPolicy policy(*this);
    QList<DownloadCandidate> result;
    QSet<QString> seen;

    for (const auto* source : m_downloadSources.enabledInOrder()) {
        for (auto candidate : source->candidates(request)) {
            const auto rejection = policy.rejectionReason(candidate.url);
            if (!rejection.isEmpty()) {
                qWarning() << "Rejected Minecraft download URL:" << candidate.url.toString() << rejection;
                continue;
            }

            const auto normalized = candidate.url.toString();
            if (seen.contains(normalized)) {
                continue;
            }
            seen.insert(normalized);
            if (candidate.sourceId.isEmpty()) {
                candidate.sourceId = source->id();
            }
            result.append(std::move(candidate));
        }
    }
    return result;
}

QString PluginSystem::validateDownloadUrl(const QUrl& url) const
{
    return DownloadPolicy(*this).rejectionReason(url);
}

}  // namespace Plugins
