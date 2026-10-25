// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#pragma once

#include <QList>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>

#include "plugin/PluginInterfaces.h"

namespace Plugins {

class PluginSystem;

QUrl defaultOfficialDownloadUrl(const DownloadRequest& request);

class DownloadPolicy {
   public:
    explicit DownloadPolicy(const PluginSystem& plugins);

    QString rejectionReason(const QUrl& url) const;
    static bool isOfficialUrl(const QUrl& url);

   private:
    bool isAllowedHost(const QString& host) const;

    QStringList m_allowedHosts;
};

class MojangDownloadSource final : public IDownloadSource {
   public:
    QString id() const override { return QStringLiteral("mojang-direct"); }
    QList<DownloadCandidate> candidates(const DownloadRequest& request) const override;
};

class PriorityDownloadSource final : public IDownloadSource {
   public:
    explicit PriorityDownloadSource(QVariantMap options);

    QString id() const override { return QStringLiteral("priority-mirrors"); }
    QList<DownloadCandidate> candidates(const DownloadRequest& request) const override;

   private:
    QStringList m_foreignMirrors;
    QStringList m_domesticMirrors;
};

}  // namespace Plugins
