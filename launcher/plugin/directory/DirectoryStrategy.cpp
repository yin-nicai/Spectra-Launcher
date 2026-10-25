// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "DirectoryStrategy.h"

#include <QDir>
#include <QFileInfo>

#include "Application.h"
#include "settings/SettingsObject.h"

namespace {

QString absolutePath(const QString& root, QString value)
{
    if (value.trimmed().isEmpty()) {
        return {};
    }
    QDir dir(root);
    return QDir::cleanPath(dir.absoluteFilePath(value.trimmed()));
}

QString configuredRoot(const QString& dataRoot, const QString& settingId)
{
    return absolutePath(dataRoot, APPLICATION->settings()->get(settingId).toString());
}

QString instancePath(const QString& root, const QString& instanceId)
{
    if (root.isEmpty()) {
        return {};
    }
    if (instanceId.isEmpty()) {
        return root;
    }
    return QDir::cleanPath(QDir(root).filePath(instanceId));
}

QString preservedOrConfigured(const QString& legacyPath, const QString& configuredPath)
{
    if (configuredPath.isEmpty()) {
        return legacyPath;
    }
    if (QFileInfo::exists(legacyPath) && !QFileInfo::exists(configuredPath)) {
        return legacyPath;
    }
    return configuredPath;
}

QString defaultGameRoot(const Plugins::DirectoryContext& context)
{
    const QDir instanceDir(context.instanceRoot);
    const auto minecraftDir = QDir::cleanPath(instanceDir.filePath(QStringLiteral("minecraft")));
    const auto dotMinecraftDir = QDir::cleanPath(instanceDir.filePath(QStringLiteral(".minecraft")));
    if (QFileInfo::exists(dotMinecraftDir) && !QFileInfo::exists(minecraftDir)) {
        return dotMinecraftDir;
    }
    return minecraftDir;
}

}  // namespace

namespace Plugins {

QString PrismDirectoryStrategy::resolve(DirectoryRole role, const DirectoryContext& context) const
{
    const auto instancesRoot = absolutePath(context.dataRoot, APPLICATION->settings()->get(QStringLiteral("InstanceDir")).toString());
    const auto instanceRoot = context.instanceRoot.isEmpty() ? instancePath(instancesRoot, context.instanceId) : context.instanceRoot;
    const Plugins::DirectoryContext effective{ context.dataRoot, instancesRoot, instanceRoot, context.instanceId };
    const auto baseGameRoot = defaultGameRoot(effective);
    const auto configuredGameRoot = instancePath(configuredRoot(context.dataRoot, QStringLiteral("CustomGameDirRoot")), context.instanceId);
    const auto gameRoot = preservedOrConfigured(baseGameRoot, configuredGameRoot);

    switch (role) {
        case DirectoryRole::LauncherData:
            return QDir::cleanPath(context.dataRoot);
        case DirectoryRole::Instances:
            return instancesRoot;
        case DirectoryRole::Instance:
            return QDir::cleanPath(instanceRoot);
        case DirectoryRole::Game:
            return gameRoot;
        case DirectoryRole::Libraries:
            return QDir::cleanPath(QDir(context.dataRoot).filePath(QStringLiteral("libraries")));
        case DirectoryRole::Assets: {
            const auto legacy = QDir::cleanPath(QDir(context.dataRoot).filePath(QStringLiteral("assets")));
            const auto configured = configuredRoot(context.dataRoot, QStringLiteral("CustomAssetsDirRoot"));
            return preservedOrConfigured(legacy, configured);
        }
        case DirectoryRole::ResourcePacks:
            return QDir::cleanPath(QDir(gameRoot).filePath(QStringLiteral("resourcepacks")));
        case DirectoryRole::Logs: {
            const auto legacy = QDir::cleanPath(QDir(gameRoot).filePath(QStringLiteral("logs")));
            const auto configured = instancePath(configuredRoot(context.dataRoot, QStringLiteral("CustomLogsDirRoot")), context.instanceId);
            return preservedOrConfigured(legacy, configured);
        }
        case DirectoryRole::CrashReports: {
            const auto legacy = QDir::cleanPath(QDir(gameRoot).filePath(QStringLiteral("crash-reports")));
            const auto configured =
                instancePath(configuredRoot(context.dataRoot, QStringLiteral("CustomCrashReportsDirRoot")), context.instanceId);
            return preservedOrConfigured(legacy, configured);
        }
    }
    return {};
}

}  // namespace Plugins
