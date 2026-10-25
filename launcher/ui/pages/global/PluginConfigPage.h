// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#pragma once

#include <QHash>
#include <QJsonArray>
#include <QWidget>

#include "ui/pages/BasePage.h"

class QLabel;
class QPlainTextEdit;
class QPushButton;
class QTreeWidget;

namespace Plugins {
template <typename Interface>
class PluginRegistry;
}

class PluginConfigPage final : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit PluginConfigPage(QWidget* parent = nullptr);

    QString id() const override { return QStringLiteral("plugins"); }
    QString displayName() const override;
    QIcon icon() const override { return QIcon::fromTheme(QStringLiteral("plugins")); }
    QString helpPage() const override { return QStringLiteral("Plugins"); }
    bool apply() override;

   private:
    struct Group {
        QString jsonKey;
        QTreeWidget* tree = nullptr;
        QPushButton* up = nullptr;
        QPushButton* down = nullptr;
        QLabel* summary = nullptr;
        QHash<QString, QVariantMap> options;
    };

    template <typename Interface>
    void populateGroup(Group& group, Plugins::PluginRegistry<Interface>& registry);
    void addGroup(QWidget* parent, Group& group, const QString& title, const QString& description, const QString& jsonKey);
    void moveItem(QTreeWidget* tree, int offset);
    QJsonArray groupJson(Group& group);
    QStringList lines(QPlainTextEdit* editor) const;

    Group m_metadataSources;
    Group m_downloadSources;
    Group m_directoryStrategies;
    Group m_resourceProviders;
    QPlainTextEdit* m_metadataUrls = nullptr;
    QPlainTextEdit* m_foreignMirrors = nullptr;
    QPlainTextEdit* m_domesticMirrors = nullptr;
};
