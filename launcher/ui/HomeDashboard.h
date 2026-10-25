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

class BaseInstance;
class QAction;
class QCommandLinkButton;
class QLabel;
class QMenu;
class QToolButton;

class HomeDashboard final : public QWidget {
    Q_OBJECT

   public:
    explicit HomeDashboard(QWidget* parent = nullptr);

    void setInstance(BaseInstance* instance);
    void setInstanceActions(QAction* addInstance,
                            QAction* launch,
                            QAction* kill,
                            QAction* edit,
                            QAction* memory,
                            QAction* gameDirectory,
                            QMenu* moreMenu);
    void retranslate();
    BaseInstance* instance() const { return m_instance; }

   signals:
    void addInstanceRequested();
    void downloadsRequested();
    void accountsRequested();
    void settingsRequested();

   private:
    void refresh();
    void refreshLibrarySummary();

    BaseInstance* m_instance = nullptr;
    QLabel* m_welcome = nullptr;
    QLabel* m_subtitle = nullptr;
    QLabel* m_instanceIcon = nullptr;
    QLabel* m_instanceName = nullptr;
    QLabel* m_instanceDescription = nullptr;
    QLabel* m_status = nullptr;
    QLabel* m_totalInstances = nullptr;
    QLabel* m_totalInstancesCaption = nullptr;
    QLabel* m_runningInstances = nullptr;
    QLabel* m_runningInstancesCaption = nullptr;
    QLabel* m_accountMetric = nullptr;
    QLabel* m_accountMetricCaption = nullptr;
    QLabel* m_quickTitle = nullptr;
    QToolButton* m_addInstance = nullptr;
    QToolButton* m_launch = nullptr;
    QToolButton* m_stop = nullptr;
    QToolButton* m_edit = nullptr;
    QToolButton* m_memory = nullptr;
    QToolButton* m_folder = nullptr;
    QToolButton* m_more = nullptr;
    QCommandLinkButton* m_downloads = nullptr;
    QCommandLinkButton* m_accounts = nullptr;
    QCommandLinkButton* m_settings = nullptr;
};
