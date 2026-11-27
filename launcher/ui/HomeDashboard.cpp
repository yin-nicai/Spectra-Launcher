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
#include <QDialog>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QProgressBar>
#include <QPushButton>
#include <QStyle>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include <limits>

#include "Application.h"
#include "BaseInstance.h"
#include "BuildConfig.h"
#include "InstanceList.h"
#include "icons/IconList.h"
#include "launch/LaunchTask.h"
#include "minecraft/auth/AccountList.h"
#include "ui/WorkspaceStyle.h"

HomeDashboard::HomeDashboard(QWidget* parent) : QWidget(parent)
{
    setObjectName(QStringLiteral("homeDashboard"));
    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(38, 28, 38, 26);
    outer->setSpacing(14);

    auto heading = new QHBoxLayout();
    auto headingText = new QVBoxLayout();
    headingText->setSpacing(2);
    m_welcome = new QLabel(this);
    m_welcome->setObjectName(QStringLiteral("pclPageTitle"));
    m_subtitle = new QLabel(this);
    m_subtitle->setObjectName(QStringLiteral("pclPageSubtitle"));
    m_subtitle->setWordWrap(true);
    headingText->addWidget(m_welcome);
    headingText->addWidget(m_subtitle);
    heading->addLayout(headingText, 1);

    m_addInstance = new QToolButton(this);
    m_addInstance->setObjectName(QStringLiteral("pclPrimaryButton"));
    m_addInstance->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_addInstance->setMinimumSize(122, 34);
    heading->addWidget(m_addInstance);
    outer->addLayout(heading);
    outer->addStretch(1);

    auto instancePanel = new QFrame(this);
    instancePanel->setObjectName(QStringLiteral("pclHomePanel"));
    instancePanel->setFrameShape(QFrame::NoFrame);
    auto panelLayout = new QVBoxLayout(instancePanel);
    panelLayout->setContentsMargins(24, 22, 24, 18);
    panelLayout->setSpacing(12);
    auto instanceLayout = new QHBoxLayout();
    instanceLayout->setContentsMargins(0, 0, 0, 0);
    instanceLayout->setSpacing(20);

    auto brandBlock = new QWidget(instancePanel);
    brandBlock->setObjectName(QStringLiteral("pclBrandBlock"));
    auto brandLayout = new QVBoxLayout(brandBlock);
    brandLayout->setContentsMargins(0, 0, 18, 0);
    brandLayout->setSpacing(2);
    m_productLogo = new QLabel(brandBlock);
    m_productLogo->setObjectName(QStringLiteral("pclProductLogo"));
    m_productLogo->setPixmap(APPLICATION->logo().pixmap(46));
    m_productLogo->setFixedSize(76, 46);
    m_productLogo->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    brandLayout->addWidget(m_productLogo);
    m_productName = new QLabel(QStringLiteral("光谱"), brandBlock);
    m_productName->setObjectName(QStringLiteral("pclProductName"));
    brandLayout->addWidget(m_productName);
    m_productVersion = new QLabel(QStringLiteral("Spectra %1").arg(BuildConfig.VERSION_DISPLAY), brandBlock);
    m_productVersion->setObjectName(QStringLiteral("pclProductVersion"));
    brandLayout->addWidget(m_productVersion);
    instanceLayout->addWidget(brandBlock);

    auto brandDivider = new QFrame(instancePanel);
    brandDivider->setFrameShape(QFrame::VLine);
    brandDivider->setFrameShadow(QFrame::Plain);
    brandDivider->setObjectName(QStringLiteral("pclBrandDivider"));
    instanceLayout->addWidget(brandDivider);

    m_instanceIcon = new QLabel(instancePanel);
    m_instanceIcon->setObjectName(QStringLiteral("pclInstanceIcon"));
    m_instanceIcon->setFixedSize(78, 78);
    m_instanceIcon->setAlignment(Qt::AlignCenter);
    instanceLayout->addWidget(m_instanceIcon);

    auto instanceInfo = new QVBoxLayout();
    instanceInfo->setSpacing(3);
    m_instanceName = new QLabel(instancePanel);
    m_instanceName->setObjectName(QStringLiteral("pclInstanceName"));
    m_instanceDescription = new QLabel(instancePanel);
    m_instanceDescription->setObjectName(QStringLiteral("pclMutedText"));
    m_instanceDescription->setWordWrap(true);
    m_status = new QLabel(instancePanel);
    m_status->setObjectName(QStringLiteral("pclStatusText"));
    m_status->setWordWrap(true);
    instanceInfo->addWidget(m_instanceName);
    instanceInfo->addWidget(m_instanceDescription);
    instanceInfo->addWidget(m_status);
    instanceLayout->addLayout(instanceInfo, 1);

    auto launchColumn = new QVBoxLayout();
    launchColumn->setSpacing(6);
    m_launch = new QToolButton(instancePanel);
    m_launch->setObjectName(QStringLiteral("pclLaunchButtonLarge"));
    m_launch->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_launch->setPopupMode(QToolButton::MenuButtonPopup);
    m_launch->setMinimumSize(190, 48);
    launchColumn->addWidget(m_launch);
    m_stop = new QToolButton(instancePanel);
    m_stop->setObjectName(QStringLiteral("pclSecondaryButton"));
    m_stop->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_stop->setMinimumHeight(32);
    launchColumn->addWidget(m_stop);
    instanceLayout->addLayout(launchColumn);
    panelLayout->addLayout(instanceLayout);

    m_progressArea = new QWidget(instancePanel);
    m_progressArea->setObjectName(QStringLiteral("pclLaunchProgressArea"));
    auto progressLayout = new QVBoxLayout(m_progressArea);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(4);
    auto progressHeader = new QHBoxLayout();
    progressHeader->setContentsMargins(0, 0, 0, 0);
    progressHeader->setSpacing(8);
    m_progressText = new QLabel(tr("正在准备启动…"), m_progressArea);
    m_progressText->setObjectName(QStringLiteral("pclLaunchProgressText"));
    m_progressText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    progressHeader->addWidget(m_progressText, 1);
    m_detailsButton = new QToolButton(m_progressArea);
    m_detailsButton->setObjectName(QStringLiteral("pclSecondaryButton"));
    m_detailsButton->setText(tr("详情"));
    m_detailsButton->setToolTip(tr("打开独立的启动与下载进度详情窗口。"));
    m_detailsButton->setAutoRaise(true);
    m_detailsButton->setMinimumHeight(24);
    progressHeader->addWidget(m_detailsButton);
    progressLayout->addLayout(progressHeader);
    m_progressBar = new QProgressBar(m_progressArea);
    m_progressBar->setObjectName(QStringLiteral("pclLaunchProgress"));
    m_progressBar->setRange(0, 1000);
    m_progressBar->setValue(0);
    m_progressBar->setFormat(QStringLiteral("%p%"));
    m_progressBar->setTextVisible(true);
    m_progressBar->setMinimumHeight(14);
    progressLayout->addWidget(m_progressBar);
    m_progressArea->hide();
    panelLayout->addWidget(m_progressArea);
    outer->addWidget(instancePanel);

    connect(m_detailsButton, &QToolButton::clicked, this, &HomeDashboard::openLaunchDetails);

    auto actions = new QHBoxLayout();
    actions->setSpacing(8);
    m_edit = new QToolButton(this);
    m_memory = new QToolButton(this);
    m_folder = new QToolButton(this);
    m_more = new QToolButton(this);
    for (auto* button : { m_edit, m_memory, m_folder, m_more }) {
        button->setObjectName(QStringLiteral("pclSecondaryButton"));
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setMinimumHeight(34);
        actions->addWidget(button);
    }
    actions->addStretch(1);
    outer->addLayout(actions);

    auto quick = new QHBoxLayout();
    quick->setSpacing(10);
    m_downloads = new QPushButton(tr("下载游戏与整合包"), this);
    m_downloads->setObjectName(QStringLiteral("pclQuickButton"));
    m_downloads->setIcon(WorkspaceStyle::icon(QStringLiteral("download"), QStyle::SP_ArrowDown));
    m_downloads->setMinimumHeight(54);
    m_accounts = new QPushButton(tr("管理账户"), this);
    m_accounts->setObjectName(QStringLiteral("pclQuickButton"));
    m_accounts->setIcon(WorkspaceStyle::icon(QStringLiteral("accounts"), QStyle::SP_FileDialogDetailedView));
    m_accounts->setMinimumHeight(54);
    m_settings = new QPushButton(tr("启动器设置"), this);
    m_settings->setObjectName(QStringLiteral("pclQuickButton"));
    m_settings->setIcon(WorkspaceStyle::icon(QStringLiteral("settings"), QStyle::SP_FileDialogDetailedView));
    m_settings->setMinimumHeight(54);
    quick->addWidget(m_downloads);
    quick->addWidget(m_accounts);
    quick->addWidget(m_settings);
    outer->addLayout(quick);

    m_librarySummary = new QLabel(this);
    m_librarySummary->setObjectName(QStringLiteral("pclFooterText"));
    outer->addWidget(m_librarySummary);

    connect(m_downloads, &QPushButton::clicked, this, &HomeDashboard::downloadsRequested);
    connect(m_accounts, &QPushButton::clicked, this, &HomeDashboard::accountsRequested);
    connect(m_settings, &QPushButton::clicked, this, &HomeDashboard::settingsRequested);
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
    m_more->setText(tr("更多"));
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
        connect(m_instance, &BaseInstance::launchTaskChanged, this, &HomeDashboard::watchLaunchTask);
        connect(m_instance, &QObject::destroyed, this, [this] {
            resetLaunchProgress();
            m_instance = nullptr;
            refresh();
        });
    }
    watchLaunchTask(m_instance ? m_instance->getLaunchTask() : nullptr);
    refresh();
    refreshLibrarySummary();
}

void HomeDashboard::watchLaunchTask(LaunchTask* task)
{
    if (m_launchTask == task) {
        return;
    }
    if (m_launchTask) {
        disconnect(m_launchTask, nullptr, this, nullptr);
    }

    ++m_progressGeneration;
    m_launchTask = task;
    m_progressBar->setProperty("failed", false);
    m_progressText->setProperty("failed", false);
    m_progressBar->style()->unpolish(m_progressBar);
    m_progressBar->style()->polish(m_progressBar);
    m_progressText->style()->unpolish(m_progressText);
    m_progressText->style()->polish(m_progressText);

    if (!m_launchTask) {
        resetLaunchProgress();
        return;
    }

    showLaunchProgress(tr("正在准备启动…"));
    m_progressBar->setRange(0, 1000);
    m_progressBar->setValue(0);
    updateLaunchDetails();

    connect(m_launchTask, &Task::started, this, [this] { showLaunchProgress(tr("正在准备启动…")); });
    connect(m_launchTask, &Task::status, this, [this](const QString& status) {
        if (!status.isEmpty()) {
            showLaunchProgress(status);
        }
    });
    connect(m_launchTask, &Task::progress, this, [this](qint64 current, qint64 total) {
        showLaunchProgress(m_progressText->text());
        if (total > 0) {
            m_progressBar->setRange(0, static_cast<int>(qMin<qint64>(total, std::numeric_limits<int>::max())));
            m_progressBar->setValue(static_cast<int>(qBound<qint64>(0, current, total)));
        } else {
            m_progressBar->setRange(0, 0);
        }
        updateLaunchDetails();
    });
    connect(m_launchTask, &LaunchTask::readyForLaunch, this, [this] {
        m_progressBar->setRange(0, 1000);
        m_progressBar->setValue(1000);
        showLaunchProgress(tr("正在启动 Minecraft…"));
        updateLaunchDetails();
        hideLaunchProgressLater(1000);
    });
    connect(m_launchTask, &Task::finished, this, [this] {
        if (!m_launchTask) {
            return;
        }
        if (m_launchTask->wasSuccessful()) {
            m_progressBar->setRange(0, 1000);
            m_progressBar->setValue(1000);
            updateLaunchDetails();
            hideLaunchProgressLater(500);
        } else {
            m_progressBar->setProperty("failed", true);
            m_progressText->setProperty("failed", true);
            m_progressBar->style()->unpolish(m_progressBar);
            m_progressBar->style()->polish(m_progressBar);
            m_progressText->style()->unpolish(m_progressText);
            m_progressText->style()->polish(m_progressText);
            m_progressText->setToolTip(m_launchTask->failReason());
            showLaunchProgress(tr("启动失败"));
            updateLaunchDetails();
            hideLaunchProgressLater(4000);
        }
    });
    connect(m_launchTask, &QObject::destroyed, this, [this] {
        m_launchTask = nullptr;
        resetLaunchProgress();
    });
}

void HomeDashboard::resetLaunchProgress()
{
    ++m_progressGeneration;
    if (m_progressArea) {
        m_progressArea->hide();
    }
    if (m_status) {
        m_status->show();
    }
    if (m_progressDialog) {
        m_progressDialog->close();
    }
}

void HomeDashboard::showLaunchProgress(const QString& status)
{
    if (!m_progressArea) {
        return;
    }
    m_progressText->setText(status);
    m_progressText->setToolTip(status);
    m_progressArea->show();
    m_status->hide();
    updateLaunchDetails();
}

void HomeDashboard::hideLaunchProgressLater(int delayMs)
{
    const auto generation = m_progressGeneration;
    QTimer::singleShot(delayMs, this, [this, generation] {
        if (generation != m_progressGeneration) {
            return;
        }
        m_progressArea->hide();
        m_status->show();
        m_progressBar->setProperty("failed", false);
        m_progressText->setProperty("failed", false);
        m_progressBar->style()->unpolish(m_progressBar);
        m_progressBar->style()->polish(m_progressBar);
        m_progressText->style()->unpolish(m_progressText);
        m_progressText->style()->polish(m_progressText);
        if (m_progressDialog) {
            m_progressDialog->close();
        }
    });
}

void HomeDashboard::openLaunchDetails()
{
    if (!m_launchTask) {
        return;
    }
    if (m_progressDialog) {
        m_progressDialog->show();
        m_progressDialog->raise();
        m_progressDialog->activateWindow();
        return;
    }

    auto* dialog = new QDialog(window(), Qt::Dialog);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowModality(Qt::NonModal);
    dialog->setWindowTitle(tr("启动与下载详情"));
    dialog->resize(560, 190);

    auto* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(10);

    auto* hint = new QLabel(tr("进度会持续显示在实例栏；此窗口仅在你主动打开时出现。"), dialog);
    hint->setObjectName(QStringLiteral("pclMutedText"));
    hint->setWordWrap(true);
    layout->addWidget(hint);

    m_detailsStatus = new QLabel(dialog);
    m_detailsStatus->setObjectName(QStringLiteral("pclLaunchProgressText"));
    m_detailsStatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_detailsStatus->setWordWrap(true);
    layout->addWidget(m_detailsStatus);

    m_detailsProgress = new QProgressBar(dialog);
    m_detailsProgress->setObjectName(QStringLiteral("pclLaunchProgress"));
    m_detailsProgress->setTextVisible(true);
    m_detailsProgress->setMinimumHeight(16);
    layout->addWidget(m_detailsProgress);
    layout->addStretch(1);

    m_progressDialog = dialog;
    updateLaunchDetails();
    dialog->show();
}

void HomeDashboard::updateLaunchDetails()
{
    if (!m_progressDialog || !m_detailsStatus || !m_detailsProgress || !m_launchTask) {
        return;
    }

    QString status = m_progressText ? m_progressText->text() : tr("正在准备启动…");
    if (m_launchTask->getState() == Task::State::AbortedByUser || m_launchTask->getState() == Task::State::Failed) {
        const auto failure = m_launchTask->failReason();
        if (!failure.isEmpty()) {
            status += QStringLiteral("\n") + failure;
        }
    }
    m_detailsStatus->setText(status);

    if (m_progressBar) {
        m_detailsProgress->setRange(m_progressBar->minimum(), m_progressBar->maximum());
        m_detailsProgress->setValue(m_progressBar->value());
        m_detailsProgress->setFormat(m_progressBar->format());
    }
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
        resetLaunchProgress();
        m_instanceIcon->setPixmap(icons->getIcon(QStringLiteral("grass")).pixmap(72));
        m_instanceName->setText(tr("尚未选择实例"));
        m_instanceDescription->setText(tr("从顶部版本列表选择实例，或创建一个新的 Minecraft 安装。"));
        m_status->setText(tr("启动器已就绪"));
        return;
    }

    m_instanceIcon->setPixmap(icons->getIcon(m_instance->iconKey()).pixmap(72));
    m_instanceName->setText(m_instance->name());
    m_instanceDescription->setText(m_instance->getStatusbarDescription());
    m_status->setText(m_instance->isRunning() ? tr("游戏正在运行") : tr("可以启动"));
}

void HomeDashboard::refreshLibrarySummary()
{
    const auto instances = APPLICATION->instances();
    const auto account = APPLICATION->accounts()->defaultAccount();
    m_librarySummary->setText(
        tr("%1 个实例  ·  当前账户：%2").arg(instances ? instances->count() : 0).arg(account ? account->displayName() : tr("未登录")));
}

void HomeDashboard::retranslate()
{
    m_welcome->setText(tr("Minecraft"));
    m_subtitle->setText(tr("选择一个版本，然后直接开始游戏。"));
    m_productName->setText(tr("光谱"));
    m_productVersion->setText(QStringLiteral("Spectra %1").arg(BuildConfig.VERSION_DISPLAY));
    m_downloads->setText(tr("下载游戏与整合包"));
    m_accounts->setText(tr("管理账户"));
    m_settings->setText(tr("启动器设置"));
    refresh();
}
