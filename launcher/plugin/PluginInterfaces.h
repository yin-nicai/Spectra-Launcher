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

#include <QByteArray>
#include <QList>
#include <QString>
#include <QUrl>

#include <functional>
#include <memory>

class BaseInstance;
class BasePage;
class QWidget;

namespace Plugins {

class IPlugin {
   public:
    virtual ~IPlugin() = default;

    virtual QString id() const = 0;
};

struct MetadataRequest {
    enum class Kind { Index, VersionList, Version };

    Kind kind = Kind::Version;
    QString uid;
    QString version;
    QString localPath;
};

struct MetadataCandidate {
    QUrl url;
    QString sha256;
    QString sourceId;
};

class IMetadataSource : public IPlugin {
   public:
    virtual QList<MetadataCandidate> query(const MetadataRequest& request) const = 0;
};

enum class MinecraftFileKind { Client, Library, AssetIndex, Asset };

struct DownloadRequest {
    MinecraftFileKind kind = MinecraftFileKind::Library;
    QUrl upstreamUrl;
    QString logicalPath;
    QByteArray sha1;
    QByteArray sha256;
    qint64 size = -1;
};

struct DownloadCandidate {
    QUrl url;
    int priority = 0;
    QString sourceId;
};

class IDownloadSource : public IPlugin {
   public:
    virtual QList<DownloadCandidate> candidates(const DownloadRequest& request) const = 0;
};

enum class DirectoryRole { LauncherData, Instances, Instance, Game, Libraries, Assets, ResourcePacks, Logs, CrashReports };

struct DirectoryContext {
    QString dataRoot;
    QString instancesRoot;
    QString instanceRoot;
    QString instanceId;
};

class IDirectoryStrategy : public IPlugin {
   public:
    virtual QString resolve(DirectoryRole role, const DirectoryContext& context) const = 0;
};

enum class UISurface { GlobalSettings, InstanceSettings };

struct UIExtensionContext {
    UISurface surface = UISurface::GlobalSettings;
    BaseInstance* instance = nullptr;
    QWidget* parent = nullptr;
};

struct UIPageSpec {
    QString id;
    QString title;
    QString description;
    std::function<BasePage*(QWidget*)> createPage;
};

class IUIExtension : public IPlugin {
   public:
    virtual QList<UIPageSpec> pages(const UIExtensionContext& context) const = 0;
};

enum class ResourceSurface { InstanceSource, Mod, ResourcePack, TexturePack, ShaderPack, DataPack };

struct ResourcePageContext {
    ResourceSurface surface = ResourceSurface::InstanceSource;
    BaseInstance* instance = nullptr;
    QWidget* parent = nullptr;
};

struct ResourcePageSpec {
    QString id;
    QString title;
    QString description;
    QString iconName;
    std::function<BasePage*(QWidget*)> createPage;
};

class IResourceProvider : public IPlugin {
   public:
    virtual QList<ResourcePageSpec> pages(const ResourcePageContext& context) const = 0;
};

}  // namespace Plugins
