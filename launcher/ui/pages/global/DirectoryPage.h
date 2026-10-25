// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#pragma once

#include <QList>
#include <QWidget>

#include "ui/pages/BasePage.h"

class QLabel;
class QLineEdit;
class QPushButton;

class DirectoryPage final : public QWidget, public BasePage {
    Q_OBJECT

   public:
    explicit DirectoryPage(QWidget* parent = nullptr);

    QString id() const override { return QStringLiteral("directories"); }
    QString displayName() const override;
    QIcon icon() const override { return QIcon::fromTheme(QStringLiteral("folder")); }
    QString helpPage() const override { return QStringLiteral("Directories"); }
    bool apply() override;
    void retranslate() override;

   private:
    struct PathRow {
        QString settingId;
        QString defaultValue;
        QString title;
        QString hint;
        QString openTitle;
        QLabel* titleLabel = nullptr;
        QLabel* hintLabel = nullptr;
        QLineEdit* edit = nullptr;
        QPushButton* browse = nullptr;
        QPushButton* open = nullptr;
        QPushButton* reset = nullptr;
    };

    void loadSettings();
    void browse(PathRow& row);
    void openDirectory(PathRow& row);
    void resetToDefault(PathRow& row);

    QLabel* m_intro = nullptr;
    QList<PathRow> m_rows;
};
