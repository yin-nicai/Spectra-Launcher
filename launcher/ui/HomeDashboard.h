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

#include <QPointer>
#include <QWidget>

class BaseInstance;
class LaunchTask;
class QAction;
class QDialog;
class QLabel;
class QMenu;
class QProgressBar;
class QPushButton;
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
    void watchLaunchTask(LaunchTask* task);
    void resetLaunchProgress();
    void showLaunchProgress(const QString& status);
    void hideLaunchProgressLater(int delayMs);
    void openLaunchDetails();
    void updateLaunchDetails();

    BaseInstance* m_instance = nullptr;
    QLabel* m_welcome = nullptr;
    QLabel* m_subtitle = nullptr;
    QLabel* m_productLogo = nullptr;
    QLabel* m_productName = nullptr;
    QLabel* m_productVersion = nullptr;
    QLabel* m_instanceIcon = nullptr;
    QLabel* m_instanceName = nullptr;
    QLabel* m_instanceDescription = nullptr;
    QLabel* m_status = nullptr;
    QLabel* m_librarySummary = nullptr;
    QWidget* m_progressArea = nullptr;
    QLabel* m_progressText = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QToolButton* m_detailsButton = nullptr;
    QPointer<QDialog> m_progressDialog;
    QPointer<QLabel> m_detailsStatus;
    QPointer<QProgressBar> m_detailsProgress;
    LaunchTask* m_launchTask = nullptr;
    int m_progressGeneration = 0;
    QToolButton* m_addInstance = nullptr;
    QToolButton* m_launch = nullptr;
    QToolButton* m_stop = nullptr;
    QToolButton* m_edit = nullptr;
    QToolButton* m_memory = nullptr;
    QToolButton* m_folder = nullptr;
    QToolButton* m_more = nullptr;
    QPushButton* m_downloads = nullptr;
    QPushButton* m_accounts = nullptr;
    QPushButton* m_settings = nullptr;
};
