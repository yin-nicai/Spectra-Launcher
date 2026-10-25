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
class QLabel;

class DownloadsPage final : public QWidget {
    Q_OBJECT

   public:
    explicit DownloadsPage(QWidget* parent = nullptr);

    void retranslate();

   signals:
    void providerSelected(const QString& pageId);
    void configureSourcesRequested();

   private:
    void rebuildProviderCards();
    void clearProviderCards();

    QLabel* m_title = nullptr;
    QLabel* m_subtitle = nullptr;
    QLabel* m_policyNote = nullptr;
    QLabel* m_empty = nullptr;
    QGridLayout* m_cards = nullptr;
};
