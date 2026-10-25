// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "MetadataSources.h"

#include <QSet>

#include "Application.h"
#include "BuildConfig.h"
#include "settings/SettingsObject.h"

namespace {

QUrl metadataBaseUrl(QString value)
{
    QUrl url(value);
    if (!url.isValid() || url.host().isEmpty() || (url.scheme() != QStringLiteral("http") && url.scheme() != QStringLiteral("https"))) {
        return {};
    }
    if (!url.path().endsWith(QLatin1Char('/'))) {
        url.setPath(url.path() + QLatin1Char('/'));
    }
    return url;
}

}  // namespace

namespace Plugins {

QList<MetadataCandidate> PrismMetadataSource::query(const MetadataRequest& request) const
{
    QString baseUrl = APPLICATION->settings()->get(QStringLiteral("MetaURLOverride")).toString();
    if (baseUrl.isEmpty()) {
        baseUrl = BuildConfig.META_URL;
    }

    const auto base = metadataBaseUrl(baseUrl);
    if (!base.isValid()) {
        return {};
    }
    return { MetadataCandidate{ base.resolved(request.localPath), {}, id() } };
}

ConfiguredMetadataSource::ConfiguredMetadataSource(QVariantMap options)
{
    const auto values = options.value(QStringLiteral("urls")).toStringList();
    QSet<QString> seen;
    for (const auto& value : values) {
        const auto url = metadataBaseUrl(value.trimmed());
        if (!url.isValid()) {
            continue;
        }
        const auto normalized = url.toString();
        if (!seen.contains(normalized)) {
            seen.insert(normalized);
            m_baseUrls.append(normalized);
        }
    }
}

QList<MetadataCandidate> ConfiguredMetadataSource::query(const MetadataRequest& request) const
{
    QList<MetadataCandidate> result;
    result.reserve(m_baseUrls.size());
    for (const auto& baseUrl : m_baseUrls) {
        result.append({ QUrl(baseUrl).resolved(request.localPath), {}, id() });
    }
    return result;
}

}  // namespace Plugins
