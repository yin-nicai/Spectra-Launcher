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

#include <QJsonObject>
#include <QObject>

#include "PluginInterfaces.h"
#include "PluginRegistry.h"

class SettingsObject;

namespace Plugins {

class PluginSystem : public QObject {
    Q_OBJECT

   public:
    static constexpr auto SETTING_ID = "PluginConfiguration";

    explicit PluginSystem(SettingsObject* settings, QObject* parent = nullptr);

    PluginRegistry<IMetadataSource>& metadataSources() { return m_metadataSources; }
    const PluginRegistry<IMetadataSource>& metadataSources() const { return m_metadataSources; }

    PluginRegistry<IDownloadSource>& downloadSources() { return m_downloadSources; }
    const PluginRegistry<IDownloadSource>& downloadSources() const { return m_downloadSources; }

    PluginRegistry<IDirectoryStrategy>& directoryStrategies() { return m_directoryStrategies; }
    const PluginRegistry<IDirectoryStrategy>& directoryStrategies() const { return m_directoryStrategies; }

    PluginRegistry<IUIExtension>& uiExtensions() { return m_uiExtensions; }
    const PluginRegistry<IUIExtension>& uiExtensions() const { return m_uiExtensions; }

    PluginRegistry<IResourceProvider>& resourceProviders() { return m_resourceProviders; }
    const PluginRegistry<IResourceProvider>& resourceProviders() const { return m_resourceProviders; }

    QList<MetadataCandidate> metadataCandidates(const MetadataRequest& request) const;
    QList<DownloadCandidate> downloadCandidates(const DownloadRequest& request) const;
    QString validateDownloadUrl(const QUrl& url) const;

    QJsonObject configuration() const;
    void applyConfiguration(const QJsonObject& configuration, bool persist = true);
    void reloadConfiguration();

   signals:
    void configurationChanged();

   private:
    void registerBuiltins();

    SettingsObject* m_settings = nullptr;
    PluginRegistry<IMetadataSource> m_metadataSources;
    PluginRegistry<IDownloadSource> m_downloadSources;
    PluginRegistry<IDirectoryStrategy> m_directoryStrategies;
    PluginRegistry<IUIExtension> m_uiExtensions;
    PluginRegistry<IResourceProvider> m_resourceProviders;
};

}  // namespace Plugins
