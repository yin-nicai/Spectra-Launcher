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

#include <QWidget>

class QGridLayout;

class SettingsHubPage final : public QWidget {
    Q_OBJECT

   public:
    explicit SettingsHubPage(QWidget* parent = nullptr);

   signals:
    void aboutRequested();
    void helpRequested();

   private:
    void addCard(QGridLayout* layout,
                 int row,
                 int column,
                 const QString& title,
                 const QString& description,
                 const QString& icon,
                 const QString& pageId);
};
