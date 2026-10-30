// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "HomeDashboard.h"

#include <QAction>
#include <QCommandLinkButton>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

#include "Application.h"
#include "BaseInstance.h"
#include "InstanceList.h"
#include "icons/IconList.h"
#include "minecraft/auth/AccountList.h"
#include "ui/WorkspaceStyle.h"

namespace {
QCommandLinkButton* makeCard(QWidget* parent, const QString& icon, QStyle::StandardPixmap fallback)
{
    auto card = new QCommandLinkButton(parent);
    card->setIcon(WorkspaceStyle::icon(icon, fallback));
    card->setIconSize(QSize(36, 36));
    card->setFixedHeight(104);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    return card;
}

QFrame* makeMetric(QWidget* parent, QLabel** value, QLabel** caption, const QString& iconName)
{
    auto frame = new QFrame(parent);
    frame->setObjectName(QStringLiteral("homeMetric"));
    frame->setFrameShape(QFrame::StyledPanel);
    auto layout = new QHBoxLayout(frame);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(10);

    auto icon = new QLabel(frame);
    icon->setPixmap(WorkspaceStyle::icon(iconName, QStyle::SP_FileDialogDetailedView).pixmap(24));
    icon->setFixedSize(28, 28);
    layout->addWidget(icon);

    auto text = new QVBoxLayout();
    text->setSpacing(0);
    *value = new QLabel(frame);
    QFont valueFont = (*value)->font();
    valueFont.setWeight(QFont::DemiBold);
    valueFont.setPointSize(valueFont.pointSize() + 1);
    (*value)->setFont(valueFont);
    *caption = new QLabel(frame);
    (*caption)->setEnabled(false);
    text->addWidget(*value);
    text->addWidget(*caption);
    layout->addLayout(text, 1);
    return frame;
}
}  // namespace

HomeDashboard::HomeDashboard(QWidget* parent) : QWidget(parent)
{
    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outer->addWidget(scroll);

    auto content = new QWidget(scroll);
    content->setObjectName(QStringLiteral("homeContent"));
    auto layout = new QVBoxLayout(content);
    layout->setContentsMargins(28, 24, 28, 28);
    layout->setSpacing(18);

    auto heading = new QHBoxLayout();
    auto headingText = new QVBoxLayout();
    headingText->setSpacing(3);
    m_welcome = new QLabel(content);
    QFont welcomeFont = m_welcome->font();
    welcomeFont.setPointSize(welcomeFont.pointSize() + 7);
    welcomeFont.setWeight(QFont::DemiBold);
    m_welcome->setFont(welcomeFont);
    m_subtitle = new QLabel(content);
    m_subtitle->setWordWrap(true);
    m_subtitle->setEnabled(false);
    headingText->addWidget(m_welcome);
    headingText->addWidget(m_subtitle);
    heading->addLayout(headingText, 1);
    m_addInstance = new QToolButton(content);
    m_addInstance->setObjectName(QStringLiteral("primaryActionButton"));
    m_addInstance->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_addInstance->setMinimumHeight(38);
    heading->addWidget(m_addInstance);
    layout->addLayout(heading);

    auto hero = new QFrame(content);
    hero->setObjectName(QStringLiteral("homeHero"));
    hero->setFrameShape(QFrame::StyledPanel);
    auto heroLayout = new QVBoxLayout(hero);
    heroLayout->setContentsMargins(22, 20, 22, 18);
    heroLayout->setSpacing(16);

    auto instanceRow = new QHBoxLayout();
    instanceRow->setSpacing(18);
    m_instanceIcon = new QLabel(hero);
    m_instanceIcon->setFixedSize(78, 78);
    m_instanceIcon->setScaledContents(true);
    instanceRow->addWidget(m_instanceIcon);

    auto instanceInfo = new QVBoxLayout();
    instanceInfo->setSpacing(4);
    m_instanceName = new QLabel(hero);
    QFont nameFont = m_instanceName->font();
    nameFont.setPointSize(nameFont.pointSize() + 5);
    nameFont.setWeight(QFont::DemiBold);
    m_instanceName->setFont(nameFont);
    m_instanceDescription = new QLabel(hero);
    m_instanceDescription->setWordWrap(true);
    m_instanceDescription->setEnabled(false);
    m_status = new QLabel(hero);
    m_status->setWordWrap(true);
    instanceInfo->addWidget(m_instanceName);
    instanceInfo->addWidget(m_instanceDescription);
    instanceInfo->addWidget(m_status);
    instanceRow->addLayout(instanceInfo, 1);

    auto primaryActions = new QVBoxLayout();
    primaryActions->setSpacing(8);
    m_launch = new QToolButton(hero);
    m_launch->setObjectName(QStringLiteral("homePrimaryAction"));
    m_launch->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_launch->setPopupMode(QToolButton::MenuButtonPopup);
    m_launch->setMinimumSize(176, 44);
    primaryActions->addWidget(m_launch);
    m_stop = new QToolButton(hero);
    m_stop->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_stop->setMinimumHeight(36);
    primaryActions->addWidget(m_stop);
    instanceRow->addLayout(primaryActions);
    heroLayout->addLayout(instanceRow);

    auto separator = new QFrame(hero);
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    heroLayout->addWidget(separator);

    auto secondaryActions = new QHBoxLayout();
    secondaryActions->setSpacing(8);
    m_edit = new QToolButton(hero);
    m_memory = new QToolButton(hero);
    m_folder = new QToolButton(hero);
    m_more = new QToolButton(hero);
    for (auto* button : { m_edit, m_memory, m_folder, m_more }) {
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setMinimumHeight(34);
        secondaryActions->addWidget(button);
    }
    secondaryActions->addStretch(1);
    heroLayout->addLayout(secondaryActions);
    layout->addWidget(hero);

    auto metrics = new QHBoxLayout();
    metrics->setSpacing(12);
    metrics->addWidget(makeMetric(content, &m_totalInstances, &m_totalInstancesCaption, QStringLiteral("view-list-details")));
    metrics->addWidget(makeMetric(content, &m_runningInstances, &m_runningInstancesCaption, QStringLiteral("status-running")));
    metrics->addWidget(makeMetric(content, &m_accountMetric, &m_accountMetricCaption, QStringLiteral("accounts")));
    layout->addLayout(metrics);

    m_quickTitle = new QLabel(content);
    QFont sectionFont = m_quickTitle->font();
    sectionFont.setWeight(QFont::DemiBold);
    sectionFont.setPointSize(sectionFont.pointSize() + 2);
    m_quickTitle->setFont(sectionFont);
    layout->addWidget(m_quickTitle);

    auto cards = new QGridLayout();
    cards->setHorizontalSpacing(12);
    cards->setVerticalSpacing(12);
    m_downloads = makeCard(content, QStringLiteral("download"), QStyle::SP_ArrowDown);
    m_accounts = makeCard(content, QStringLiteral("accounts"), QStyle::SP_FileDialogDetailedView);
    m_settings = makeCard(content, QStringLiteral("settings"), QStyle::SP_FileDialogDetailedView);
    cards->addWidget(m_downloads, 0, 0);
    cards->addWidget(m_accounts, 0, 1);
    cards->addWidget(m_settings, 0, 2);
    layout->addLayout(cards);
    layout->addStretch(1);
    scroll->setWidget(content);

    connect(m_downloads, &QCommandLinkButton::clicked, this, &HomeDashboard::downloadsRequested);
    connect(m_accounts, &QCommandLinkButton::clicked, this, &HomeDashboard::accountsRequested);
    connect(m_settings, &QCommandLinkButton::clicked, this, &HomeDashboard::settingsRequested);
    connect(APPLICATION->instances(), &InstanceList::instancesChanged, this, &HomeDashboard::refreshLibrarySummary);
    connect(APPLICATION->instances(), &InstanceList::dataIsInvalid, this, &HomeDashboard::refreshLibrarySummary);
    connect(APPLICATION->accounts(), &AccountList::listChanged, this, &HomeDashboard::refreshLibrarySummary);
    connect(APPLICATION->accounts(), &AccountList::defaultAccountChanged, this, &HomeDashboard::refreshLibrarySummary);

    retranslate();
    refreshLibrarySummary();
    refresh();
}

void HomeDashboard::setInstanceActions(QAction* addInstance,
                                       QAction* launch,
                                       QAction* kill,
                                       QAction* edit,
                                       QAction* memory,
                                       QAction* gameDirectory,
                                       QMenu* moreMenu)
{
    if (addInstance) {
        m_addInstance->setDefaultAction(addInstance);
    }
    if (launch) {
        m_launch->setDefaultAction(launch);
        m_launch->setMenu(nullptr);
    }
    if (kill) {
        m_stop->setDefaultAction(kill);
    }
    if (edit) {
        m_edit->setDefaultAction(edit);
    }
    if (memory) {
        m_memory->setDefaultAction(memory);
    }
    if (gameDirectory) {
        m_folder->setDefaultAction(gameDirectory);
    }
    m_more->setText(tr("More"));
    m_more->setMenu(moreMenu);
    m_more->setPopupMode(QToolButton::InstantPopup);
}

void HomeDashboard::setInstance(BaseInstance* instance)
{
    if (m_instance == instance) {
        refresh();
        return;
    }
    if (m_instance) {
        disconnect(m_instance, nullptr, this, nullptr);
    }
    m_instance = instance;
    if (m_instance) {
        connect(m_instance, &BaseInstance::propertiesChanged, this, &HomeDashboard::refresh);
        connect(m_instance, &BaseInstance::runningStatusChanged, this, &HomeDashboard::refresh);
        connect(m_instance, &BaseInstance::statusChanged, this, &HomeDashboard::refresh);
        connect(m_instance, &QObject::destroyed, this, [this] {
            m_instance = nullptr;
            refresh();
        });
    }
    refresh();
    refreshLibrarySummary();
}

void HomeDashboard::refresh()
{
    const bool available = m_instance != nullptr;
    m_stop->setVisible(available && m_instance->isRunning());
    m_folder->setVisible(available);
    auto* icons = APPLICATION->icons();
    if (!icons) {
        return;
    }

    if (!m_instance) {
        m_instanceIcon->setPixmap(icons->getIcon(QStringLiteral("grass")).pixmap(78));
        m_instanceName->setText(tr("No instance selected"));
        m_instanceDescription->setText(tr("Use the instance selector above, or create a new instance."));
        m_status->setText(tr("Your Minecraft library is ready."));
        return;
    }

    m_instanceIcon->setPixmap(icons->getIcon(m_instance->iconKey()).pixmap(78));
    m_instanceName->setText(m_instance->name());
    m_instanceDescription->setText(m_instance->getStatusbarDescription());
    m_status->setText(m_instance->isRunning() ? tr("The instance is running.") : tr("Ready to launch."));
}

void HomeDashboard::refreshLibrarySummary()
{
    const auto instances = APPLICATION->instances();
    int running = 0;
    for (int index = 0; index < instances->count(); ++index) {
        if (instances->at(index)->isRunning()) {
            ++running;
        }
    }
    m_totalInstances->setText(QString::number(instances->count()));
    m_runningInstances->setText(QString::number(running));

    const auto account = APPLICATION->accounts()->defaultAccount();
    m_accountMetric->setText(account ? account->displayName() : tr("No account"));
}

void HomeDashboard::retranslate()
{
    m_welcome->setText(tr("Welcome back"));
    m_subtitle->setText(tr("Launch a game, manage an instance, or set up a new Minecraft installation."));
    m_quickTitle->setText(tr("Quick actions"));
    m_totalInstancesCaption->setText(tr("instances"));
    m_runningInstancesCaption->setText(tr("running"));
    m_accountMetricCaption->setText(tr("active account"));
    m_downloads->setText(tr("Downloads"));
    m_downloads->setDescription(tr("Browse modpacks and resource platforms."));
    m_accounts->setText(tr("Accounts"));
    m_accounts->setDescription(tr("Manage Microsoft and offline accounts."));
    m_settings->setText(tr("Settings"));
    m_settings->setDescription(tr("Configure Java, memory, sources, and directories."));
    refresh();
}
