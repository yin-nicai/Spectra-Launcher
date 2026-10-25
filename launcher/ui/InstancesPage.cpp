// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "InstancesPage.h"

#include <QAction>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>
#include <QVBoxLayout>

#include "BaseInstance.h"
#include "ui/instanceview/InstanceView.h"

InstancesPage::InstancesPage(InstanceView* view, QWidget* parent) : QWidget(parent), m_view(view)
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 18);
    layout->setSpacing(14);

    auto header = new QHBoxLayout();
    header->setSpacing(8);
    auto titleBlock = new QVBoxLayout();
    titleBlock->setSpacing(3);
    m_title = new QLabel(this);
    QFont titleFont = m_title->font();
    titleFont.setPointSize(titleFont.pointSize() + 7);
    titleFont.setWeight(QFont::DemiBold);
    m_title->setFont(titleFont);
    m_instance = new QLabel(this);
    m_instance->setEnabled(false);
    titleBlock->addWidget(m_title);
    titleBlock->addWidget(m_instance);
    header->addLayout(titleBlock, 1);

    m_add = new QToolButton(this);
    m_add->setObjectName(QStringLiteral("primaryActionButton"));
    m_launch = new QToolButton(this);
    m_launch->setObjectName(QStringLiteral("primaryActionButton"));
    m_edit = new QToolButton(this);
    m_folder = new QToolButton(this);
    m_more = new QToolButton(this);
    for (auto* button : { m_add, m_launch, m_edit, m_folder, m_more }) {
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setMinimumHeight(36);
        header->addWidget(button);
    }
    layout->addLayout(header);

    auto listFrame = new QFrame(this);
    listFrame->setObjectName(QStringLiteral("instancesViewFrame"));
    listFrame->setFrameShape(QFrame::StyledPanel);
    auto listLayout = new QVBoxLayout(listFrame);
    listLayout->setContentsMargins(6, 6, 6, 6);
    m_view->setParent(listFrame);
    m_view->setFrameShape(QFrame::NoFrame);
    listLayout->addWidget(m_view);
    layout->addWidget(listFrame, 1);

    m_more->setPopupMode(QToolButton::InstantPopup);
    retranslate();
}

void InstancesPage::setActions(QAction* addInstance,
                               QAction* launchAction,
                               QAction* editAction,
                               QAction* folderAction,
                               QMenu* instanceMenu,
                               QAction* undoTrashAction)
{
    m_add->setDefaultAction(addInstance);
    m_launch->setDefaultAction(launchAction);
    m_launch->setMenu(nullptr);
    m_edit->setDefaultAction(editAction);
    m_folder->setDefaultAction(folderAction);
    m_more->setMenu(instanceMenu);
    if (undoTrashAction) {
        instanceMenu->addSeparator();
        instanceMenu->addAction(undoTrashAction);
    }
}

void InstancesPage::setInstance(BaseInstance* instance)
{
    m_instance->setText(instance ? tr("Selected: %1").arg(instance->name()) : tr("Select an instance to manage it."));
    m_instance->setEnabled(instance != nullptr);
}

void InstancesPage::retranslate()
{
    m_title->setText(tr("Instances"));
    m_more->setText(tr("More"));
    m_more->setToolTip(tr("Instance management, folders, logs, updates, and help."));
}
