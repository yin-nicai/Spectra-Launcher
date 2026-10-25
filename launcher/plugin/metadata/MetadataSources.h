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

#include <QStringList>
#include <QVariantMap>

#include "plugin/PluginInterfaces.h"

namespace Plugins {

class PrismMetadataSource final : public IMetadataSource {
   public:
    QString id() const override { return QStringLiteral("prism-meta"); }
    QList<MetadataCandidate> query(const MetadataRequest& request) const override;
};

class ConfiguredMetadataSource final : public IMetadataSource {
   public:
    static QString sourceId() { return QStringLiteral("configured-http-meta"); }

    explicit ConfiguredMetadataSource(QVariantMap options);

    QString id() const override { return sourceId(); }
    QList<MetadataCandidate> query(const MetadataRequest& request) const override;

   private:
    QStringList m_baseUrls;
};

}  // namespace Plugins
