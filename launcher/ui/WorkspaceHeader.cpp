// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "WorkspaceHeader.h"

#include <QAction>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QSignalBlocker>
#include <QToolButton>

#include "WorkspaceStyle.h"

WorkspaceHeader::WorkspaceHeader(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("workspaceHeader"));
    setFrameShape(QFrame::NoFrame);

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(20, 10, 20, 10);
    layout->setSpacing(8);

    auto instanceLabel = new QLabel(tr("Instance"), this);
    instanceLabel->setObjectName(QStringLiteral("workspaceFieldLabel"));
    layout->addWidget(instanceLabel);

    m_instanceSelector = new QComboBox(this);
    m_instanceSelector->setObjectName(QStringLiteral("workspaceInstanceSelector"));
    m_instanceSelector->setMinimumContentsLength(16);
    m_instanceSelector->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_instanceSelector->setMinimumWidth(190);
    m_instanceSelector->setMaximumWidth(310);
    m_instanceSelector->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_instanceSelector->setToolTip(tr("Select the instance used by the launch and instance actions."));
    layout->addWidget(m_instanceSelector);

    m_launchButton = new QToolButton(this);
    m_launchButton->setObjectName(QStringLiteral("workspaceLaunchButton"));
    m_launchButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_launchButton->setMinimumHeight(36);
    layout->addWidget(m_launchButton);

    layout->addStretch(1);

    m_downloadSourceButton = new QPushButton(this);
    m_downloadSourceButton->setObjectName(QStringLiteral("downloadSourceBadge"));
    m_downloadSourceButton->setCursor(Qt::PointingHandCursor);
    m_downloadSourceButton->setIcon(WorkspaceStyle::icon(QStringLiteral("download"), QStyle::SP_ArrowDown));
    m_downloadSourceButton->setMaximumWidth(260);
    layout->addWidget(m_downloadSourceButton);

    m_newsButton = new QToolButton(this);
    m_newsButton->setIcon(WorkspaceStyle::icon(QStringLiteral("news"), QStyle::SP_MessageBoxInformation));
    m_newsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_newsButton->setMinimumHeight(36);
    layout->addWidget(m_newsButton);

    m_accountButton = new QToolButton(this);
    m_accountButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_accountButton->setPopupMode(QToolButton::InstantPopup);
    m_accountButton->setMinimumHeight(36);
    m_accountButton->setMinimumWidth(44);
    layout->addWidget(m_accountButton);

    m_moreButton = new QToolButton(this);
    m_moreButton->setText(QStringLiteral("⋯"));
    m_moreButton->setPopupMode(QToolButton::InstantPopup);
    m_moreButton->setMinimumSize(38, 36);
    layout->addWidget(m_moreButton);

    connect(m_instanceSelector, &QComboBox::activated, this, [this](int index) {
        const auto instanceId = m_instanceSelector->itemData(index).toString();
        if (!instanceId.isEmpty()) {
            emit instanceSelected(instanceId);
        }
    });
    connect(m_downloadSourceButton, &QPushButton::clicked, this, &WorkspaceHeader::downloadSourcesRequested);
    connect(m_newsButton, &QToolButton::clicked, this, &WorkspaceHeader::newsRequested);

    retranslate();
}

void WorkspaceHeader::setInstances(const QList<InstanceChoice>& instances, const QString& selectedId)
{
    const QSignalBlocker blocker(m_instanceSelector);
    m_instanceSelector->clear();
    m_instanceSelector->addItem(tr("Select an instance"), QString());
    for (const auto& instance : instances) {
        m_instanceSelector->addItem(instance.icon, instance.name, instance.id);
    }
    setSelectedInstance(selectedId);
}

void WorkspaceHeader::setSelectedInstance(const QString& instanceId)
{
    const QSignalBlocker blocker(m_instanceSelector);
    const int index = m_instanceSelector->findData(instanceId);
    m_instanceSelector->setCurrentIndex(index >= 0 ? index : 0);
}

void WorkspaceHeader::setAccount(const QString& name, const QIcon& icon, const QString& toolTip)
{
    m_accountButton->setText(name);
    m_accountButton->setIcon(icon);
    m_accountButton->setToolTip(toolTip);
}

void WorkspaceHeader::setAccountMenu(QMenu* menu)
{
    m_accountButton->setMenu(menu);
}

void WorkspaceHeader::setMoreMenu(QMenu* menu)
{
    m_moreButton->setMenu(menu);
}

void WorkspaceHeader::setNews(const QString& title, bool enabled)
{
    m_newsButton->setText(enabled ? tr("News") : tr("No news"));
    m_newsButton->setToolTip(title);
    m_newsButton->setEnabled(enabled);
}

void WorkspaceHeader::setDownloadSourceStatus(const QString& summary, const QString& toolTip)
{
    m_downloadSourceButton->setText(summary);
    m_downloadSourceButton->setToolTip(toolTip);
}

void WorkspaceHeader::setLaunchAction(QAction* action)
{
    if (m_launchAction != action) {
        m_launchAction = action;
        m_launchButton->setDefaultAction(action);
    }
    // Launch variants remain available in the instance menu. Keeping the primary action as a
    // single button avoids the split-color menu-button rendering.
    m_launchButton->setMenu(nullptr);
}

void WorkspaceHeader::retranslate()
{
    if (m_instanceSelector->count() > 0) {
        m_instanceSelector->setItemText(0, tr("Select an instance"));
    }
    m_instanceSelector->setToolTip(tr("Select the instance used by the launch and instance actions."));
    m_downloadSourceButton->setToolTip(tr("Open metadata and Minecraft download source configuration."));
    m_newsButton->setToolTip(tr("Open launcher news."));
    m_accountButton->setToolTip(tr("Switch account or open account settings."));
    m_moreButton->setToolTip(tr("Folders, themes, help, updates, and application actions."));
}
