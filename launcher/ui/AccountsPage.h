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

class QLabel;
class QVBoxLayout;

class AccountsPage final : public QWidget {
    Q_OBJECT

   public:
    explicit AccountsPage(QWidget* parent = nullptr);

    void retranslate();

   private:
    void refresh();
    void clearRows();

    QLabel* m_title = nullptr;
    QLabel* m_summary = nullptr;
    QLabel* m_subtitle = nullptr;
    class QPushButton* m_manage = nullptr;
    QVBoxLayout* m_list = nullptr;
};
