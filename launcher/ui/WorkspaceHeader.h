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
#include <QPointer>

class QAction;
class QButtonGroup;
class QComboBox;
class QFrame;
class QLabel;
class QMenu;
class QPropertyAnimation;
class QPushButton;
class QToolButton;

class WorkspaceHeader final : public QFrame {
    Q_OBJECT

   public:
    enum class Page { Home = 0, Instances, Downloads, Accounts, Settings };

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
    void setCurrentPage(Page page);
    QPushButton* downloadSourceButton() const { return m_downloadSourceButton; }
    void retranslate();

   signals:
    void pageSelected(Page page);
    void instanceSelected(const QString& instanceId);
    void accountsRequested();
    void newsRequested();
    void downloadSourcesRequested();

   private:
    QToolButton* addPageButton(const QString& text, Page page);
    void updatePageIndicator(bool animate);

   protected:
    void resizeEvent(QResizeEvent* event) override;

    QButtonGroup* m_pageGroup = nullptr;
    QToolButton* m_home = nullptr;
    QToolButton* m_instances = nullptr;
    QToolButton* m_downloads = nullptr;
    QToolButton* m_accounts = nullptr;
    QToolButton* m_settings = nullptr;
    QWidget* m_pageButtonHost = nullptr;
    QFrame* m_pageIndicator = nullptr;
    QPointer<QPropertyAnimation> m_pageIndicatorAnimation;

    QComboBox* m_instanceSelector = nullptr;
    QToolButton* m_launchButton = nullptr;
    QAction* m_launchAction = nullptr;
    QPushButton* m_downloadSourceButton = nullptr;
    QToolButton* m_newsButton = nullptr;
    QToolButton* m_accountButton = nullptr;
    QToolButton* m_moreButton = nullptr;

    QString m_newsText;
    QString m_downloadSourceSummary;
};
