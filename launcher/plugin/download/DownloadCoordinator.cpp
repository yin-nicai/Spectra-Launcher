// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "DownloadCoordinator.h"

#include <QCoreApplication>

#include "Application.h"
#include "plugin/PluginSystem.h"

namespace {

Net::Download::Ptr configureDownload(Net::Download::Ptr download, const Plugins::DownloadRequest& request)
{
    const auto candidates = APPLICATION->plugins()->downloadCandidates(request);
    QList<QUrl> urls;
    urls.reserve(candidates.size());
    for (const auto& candidate : candidates) {
        urls.append(candidate.url);
    }

    if (!urls.isEmpty()) {
        download->setUrl(urls.takeFirst());
        download->setFallbackUrls(std::move(urls));
        download->setUrlValidator([](const QUrl& url) { return APPLICATION->plugins()->validateDownloadUrl(url); });
    } else {
        // Keep one request so NetJob can report a normal, actionable failure.
        download->setUrl(request.upstreamUrl);
        download->setUrlValidator([](const QUrl& url) {
            const auto policyError = APPLICATION->plugins()->validateDownloadUrl(url);
            if (!policyError.isEmpty()) {
                return policyError;
            }
            return QCoreApplication::translate("Plugins::DownloadCoordinator",
                                               "No enabled download source supplied an allowed Minecraft download URL.");
        });
    }

    return download;
}

}  // namespace

namespace MinecraftDownloads {

Net::Download::Ptr makeCached(const Plugins::DownloadRequest& request, MetaEntryPtr entry, Net::Download::Options options)
{
    auto download = Net::Download::makeCached(request.upstreamUrl, std::move(entry), options);
    return configureDownload(std::move(download), request);
}

Net::Download::Ptr makeFile(const Plugins::DownloadRequest& request, const QString& path, Net::Download::Options options)
{
    auto download = Net::Download::makeFile(request.upstreamUrl, path, options);
    return configureDownload(std::move(download), request);
}

}  // namespace MinecraftDownloads
