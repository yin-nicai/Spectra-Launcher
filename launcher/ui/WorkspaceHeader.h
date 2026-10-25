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

#include <QFrame>
#include <QIcon>
#include <QList>
#include <QPair>

class QComboBox;
class QAction;
class QMenu;
class QPushButton;
class QToolButton;

class WorkspaceHeader final : public QFrame {
    Q_OBJECT

   public:
    struct InstanceChoice {
        QString id;
        QString name;
        QIcon icon;
    };

    explicit WorkspaceHeader(QWidget* parent = nullptr);

    void setInstances(const QList<InstanceChoice>& instances, const QString& selectedId);
    void setSelectedInstance(const QString& instanceId);
    void setAccount(const QString& name, const QIcon& icon, const QString& toolTip);
    void setAccountMenu(QMenu* menu);
    void setMoreMenu(QMenu* menu);
    void setNews(const QString& title, bool enabled);
    void setDownloadSourceStatus(const QString& summary, const QString& toolTip);
    void setLaunchAction(QAction* action);
    QPushButton* downloadSourceButton() const { return m_downloadSourceButton; }
    void retranslate();

   signals:
    void instanceSelected(const QString& instanceId);
    void newsRequested();
    void downloadSourcesRequested();

   private:
    QComboBox* m_instanceSelector = nullptr;
    QToolButton* m_launchButton = nullptr;
    QAction* m_launchAction = nullptr;
    QPushButton* m_downloadSourceButton = nullptr;
    QToolButton* m_newsButton = nullptr;
    QToolButton* m_accountButton = nullptr;
    QToolButton* m_moreButton = nullptr;
};
