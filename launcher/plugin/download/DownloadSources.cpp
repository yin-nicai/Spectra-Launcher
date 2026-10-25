// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "DownloadSources.h"

#include <QCoreApplication>
#include <QSet>

#include "Application.h"
#include "BuildConfig.h"
#include "plugin/PluginSystem.h"
#include "settings/SettingsObject.h"

namespace {

bool validNetworkUrl(const QUrl& url)
{
    return url.isValid() && !url.host().isEmpty() && url.userInfo().isEmpty() &&
           (url.scheme() == QStringLiteral("https") || url.scheme() == QStringLiteral("http"));
}

bool hostMatches(const QString& host, const QString& domain)
{
    return host == domain || host.endsWith(QLatin1Char('.') + domain);
}

void appendConfiguredUrls(QStringList& result, const QVariant& value)
{
    for (const auto& item : value.toStringList()) {
        if (!item.trimmed().isEmpty()) {
            result.append(item.trimmed());
        }
    }
}

QUrl mirrorUrl(const QString& pattern, const Plugins::DownloadRequest& request)
{
    QString path = request.logicalPath;
    if (path.isEmpty()) {
        path = request.upstreamUrl.path();
    }
    while (path.startsWith(QLatin1Char('/'))) {
        path.remove(0, 1);
    }

    auto expanded = pattern;
    if (expanded.contains(QStringLiteral("{path}")) || expanded.contains(QStringLiteral("{url}"))) {
        const auto encodedPath = QString::fromLatin1(QUrl::toPercentEncoding(path, "/"));
        expanded.replace(QStringLiteral("{path}"), encodedPath);
        expanded.replace(QStringLiteral("{url}"), request.upstreamUrl.toString(QUrl::FullyEncoded));
        return QUrl(expanded, QUrl::TolerantMode);
    }

    auto base = QUrl(expanded, QUrl::TolerantMode);
    if (!validNetworkUrl(base)) {
        return {};
    }
    if (!base.path().endsWith(QLatin1Char('/'))) {
        base.setPath(base.path() + QLatin1Char('/'));
    }
    return base.resolved(QUrl(path, QUrl::TolerantMode));
}

}  // namespace

namespace Plugins {

QUrl defaultOfficialDownloadUrl(const DownloadRequest& request)
{
    if (DownloadPolicy::isOfficialUrl(request.upstreamUrl)) {
        return request.upstreamUrl;
    }

    QString path = request.logicalPath;
    while (path.startsWith(QLatin1Char('/'))) {
        path.remove(0, 1);
    }
    if (path.isEmpty()) {
        return {};
    }

    if (request.kind == MinecraftFileKind::Library) {
        return QUrl(BuildConfig.LIBRARY_BASE + path);
    }
    if (request.kind == MinecraftFileKind::Asset) {
        return QUrl(BuildConfig.DEFAULT_RESOURCE_BASE + path);
    }
    return {};
}

DownloadPolicy::DownloadPolicy(const PluginSystem& plugins)
{
    QStringList configuredUrls;

    const auto resourceOverride = APPLICATION->settings()->get(QStringLiteral("ResourceURLOverride")).toString().trimmed();
    if (!resourceOverride.isEmpty()) {
        configuredUrls.append(resourceOverride);
    }

    for (const auto& state : plugins.downloadSources().currentStates()) {
        appendConfiguredUrls(configuredUrls, state.options.value(QStringLiteral("foreignMirrors")));
        appendConfiguredUrls(configuredUrls, state.options.value(QStringLiteral("domesticMirrors")));
        appendConfiguredUrls(configuredUrls, state.options.value(QStringLiteral("additionalMirrors")));
        for (const auto& host : state.options.value(QStringLiteral("additionalAllowedHosts")).toStringList()) {
            if (!host.trimmed().isEmpty()) {
                m_allowedHosts.append(host.trimmed().toLower());
            }
        }
    }

    for (const auto& value : configuredUrls) {
        auto candidate = value;
        candidate.replace(QStringLiteral("{path}"), QStringLiteral("path"));
        candidate.replace(QStringLiteral("{url}"), QStringLiteral("https://example.invalid/file"));
        const QUrl url(candidate, QUrl::TolerantMode);
        const auto host = url.host().toLower();
        if (validNetworkUrl(url) && !m_allowedHosts.contains(host)) {
            m_allowedHosts.append(host);
        }
    }
}

bool DownloadPolicy::isOfficialUrl(const QUrl& url)
{
    if (!validNetworkUrl(url)) {
        return false;
    }
    const auto host = url.host().toLower();
    if (hostMatches(host, QStringLiteral("mojang.com")) || hostMatches(host, QStringLiteral("minecraft.net"))) {
        return true;
    }
    return host == QStringLiteral("s3.amazonaws.com") && url.path().startsWith(QStringLiteral("/Minecraft.Download/"));
}

bool DownloadPolicy::isAllowedHost(const QString& host) const
{
    for (const auto& allowedHost : m_allowedHosts) {
        if (hostMatches(host, allowedHost)) {
            return true;
        }
    }
    return false;
}

QString DownloadPolicy::rejectionReason(const QUrl& url) const
{
    if (!validNetworkUrl(url)) {
        return QCoreApplication::translate("Plugins::DownloadPolicy", "The URL is not a valid HTTP or HTTPS download address.");
    }
    if (isOfficialUrl(url)) {
        return {};
    }

    const auto host = url.host().toLower();
    if (isAllowedHost(host)) {
        return {};
    }
    return QCoreApplication::translate(
               "Plugins::DownloadPolicy",
               "Blocked Minecraft download from untrusted host '%1'. Add it explicitly as a mirror in the plugin configuration.")
        .arg(host);
}

QList<DownloadCandidate> MojangDownloadSource::candidates(const DownloadRequest& request) const
{
    const auto officialUrl = defaultOfficialDownloadUrl(request);
    if (!DownloadPolicy::isOfficialUrl(officialUrl)) {
        return {};
    }
    return { { officialUrl, 0, id() } };
}

PriorityDownloadSource::PriorityDownloadSource(QVariantMap options)
{
    m_foreignMirrors = options.value(QStringLiteral("foreignMirrors")).toStringList();
    m_domesticMirrors = options.value(QStringLiteral("domesticMirrors")).toStringList();
}

QList<DownloadCandidate> PriorityDownloadSource::candidates(const DownloadRequest& request) const
{
    QList<DownloadCandidate> result;
    const auto officialUrl = defaultOfficialDownloadUrl(request);
    if (DownloadPolicy::isOfficialUrl(officialUrl)) {
        result.append({ officialUrl, 0, id() });
    }

    const auto resourceOverride = APPLICATION->settings()->get(QStringLiteral("ResourceURLOverride")).toString().trimmed();
    if (!resourceOverride.isEmpty()) {
        const auto url = mirrorUrl(resourceOverride, request);
        if (url.isValid()) {
            result.append({ url, 50, id() });
        }
    }

    for (int i = 0; i < m_foreignMirrors.size(); ++i) {
        const auto url = mirrorUrl(m_foreignMirrors.at(i), request);
        if (url.isValid()) {
            result.append({ url, 100 + i, id() });
        }
    }
    for (int i = 0; i < m_domesticMirrors.size(); ++i) {
        const auto url = mirrorUrl(m_domesticMirrors.at(i), request);
        if (url.isValid()) {
            result.append({ url, 200 + i, id() });
        }
    }
    return result;
}

}  // namespace Plugins
