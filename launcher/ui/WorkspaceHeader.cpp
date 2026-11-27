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
#include <QButtonGroup>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSignalBlocker>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

#include "ui/WorkspaceStyle.h"

WorkspaceHeader::WorkspaceHeader(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("workspaceHeader"));
    setFrameShape(QFrame::NoFrame);
    setAttribute(Qt::WA_StyledBackground, true);

    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(14, 4, 14, 5);
    outer->setSpacing(2);

    auto menuRow = new QHBoxLayout();
    menuRow->setContentsMargins(0, 0, 0, 0);
    menuRow->setSpacing(2);

    m_pageGroup = new QButtonGroup(this);
    m_pageGroup->setExclusive(true);
    m_pageButtonHost = new QWidget(this);
    m_pageButtonHost->setObjectName(QStringLiteral("pclNavButtonHost"));
    auto pageButtonLayout = new QHBoxLayout(m_pageButtonHost);
    pageButtonLayout->setContentsMargins(0, 0, 0, 0);
    pageButtonLayout->setSpacing(2);
    m_home = addPageButton(tr("主页"), Page::Home);
    m_instances = addPageButton(tr("实例"), Page::Instances);
    m_downloads = addPageButton(tr("下载"), Page::Downloads);
    m_accounts = addPageButton(tr("账户"), Page::Accounts);
    m_settings = addPageButton(tr("设置"), Page::Settings);
    pageButtonLayout->addWidget(m_home);
    pageButtonLayout->addWidget(m_instances);
    pageButtonLayout->addWidget(m_downloads);
    pageButtonLayout->addWidget(m_accounts);
    pageButtonLayout->addWidget(m_settings);
    m_pageIndicator = new QFrame(m_pageButtonHost);
    m_pageIndicator->setObjectName(QStringLiteral("pclNavIndicator"));
    m_pageIndicator->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_pageIndicator->lower();
    menuRow->addWidget(m_pageButtonHost);
    menuRow->addStretch(1);

    m_accountButton = new QToolButton(this);
    m_accountButton->setObjectName(QStringLiteral("pclUtilityButton"));
    m_accountButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_accountButton->setMinimumHeight(28);
    menuRow->addWidget(m_accountButton);

    m_moreButton = new QToolButton(this);
    m_moreButton->setObjectName(QStringLiteral("pclUtilityButton"));
    m_moreButton->setText(QStringLiteral("⋯"));
    m_moreButton->setPopupMode(QToolButton::InstantPopup);
    m_moreButton->setMinimumSize(32, 28);
    m_moreButton->hide();
    menuRow->addWidget(m_moreButton);
    outer->addLayout(menuRow);

    auto actionRow = new QHBoxLayout();
    actionRow->setContentsMargins(0, 0, 0, 0);
    actionRow->setSpacing(6);

    auto instanceLabel = new QLabel(tr("版本"), this);
    instanceLabel->setObjectName(QStringLiteral("pclFieldLabel"));
    actionRow->addWidget(instanceLabel);

    m_instanceSelector = new QComboBox(this);
    m_instanceSelector->setObjectName(QStringLiteral("pclInstanceSelector"));
    m_instanceSelector->setMinimumContentsLength(14);
    m_instanceSelector->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
    m_instanceSelector->setMinimumWidth(180);
    m_instanceSelector->setMaximumWidth(330);
    m_instanceSelector->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    actionRow->addWidget(m_instanceSelector);

    m_launchButton = new QToolButton(this);
    m_launchButton->setObjectName(QStringLiteral("pclLaunchButton"));
    m_launchButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_launchButton->setMinimumSize(112, 30);
    actionRow->addWidget(m_launchButton);
    actionRow->addStretch(1);

    m_downloadSourceButton = new QPushButton(this);
    m_downloadSourceButton->setObjectName(QStringLiteral("pclStatusButton"));
    m_downloadSourceButton->setCursor(Qt::PointingHandCursor);
    m_downloadSourceButton->setIcon(WorkspaceStyle::icon(QStringLiteral("download"), QStyle::SP_ArrowDown));
    m_downloadSourceButton->setMaximumWidth(260);
    actionRow->addWidget(m_downloadSourceButton);

    m_newsButton = new QToolButton(this);
    m_newsButton->setObjectName(QStringLiteral("pclUtilityButton"));
    m_newsButton->setIcon(WorkspaceStyle::icon(QStringLiteral("news"), QStyle::SP_MessageBoxInformation));
    m_newsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_newsButton->setMinimumHeight(28);
    actionRow->addWidget(m_newsButton);
    outer->addLayout(actionRow);

    connect(m_pageGroup, &QButtonGroup::idClicked, this, [this](int id) {
        if (id >= 0) {
            emit pageSelected(static_cast<Page>(id));
        }
    });
    connect(m_instanceSelector, &QComboBox::activated, this, [this](int index) {
        const auto instanceId = m_instanceSelector->itemData(index).toString();
        if (!instanceId.isEmpty()) {
            emit instanceSelected(instanceId);
        }
    });
    connect(m_accountButton, &QToolButton::clicked, this, &WorkspaceHeader::accountsRequested);
    connect(m_downloadSourceButton, &QPushButton::clicked, this, &WorkspaceHeader::downloadSourcesRequested);
    connect(m_newsButton, &QToolButton::clicked, this, &WorkspaceHeader::newsRequested);

    setCurrentPage(Page::Home);
    retranslate();
    QTimer::singleShot(0, this, [this] { updatePageIndicator(false); });
}

QToolButton* WorkspaceHeader::addPageButton(const QString& text, Page page)
{
    auto button = new QToolButton(this);
    button->setObjectName(QStringLiteral("pclNavButton"));
    button->setText(text);
    button->setCheckable(true);
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setMinimumSize(54, 28);
    m_pageGroup->addButton(button, static_cast<int>(page));
    return button;
}

void WorkspaceHeader::setInstances(const QList<InstanceChoice>& instances, const QString& selectedId)
{
    const QSignalBlocker blocker(m_instanceSelector);
    m_instanceSelector->clear();
    m_instanceSelector->addItem(tr("选择实例"), QString());
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
    Q_UNUSED(menu);
    m_accountButton->setMenu(nullptr);
}

void WorkspaceHeader::setMoreMenu(QMenu* menu)
{
    Q_UNUSED(menu);
    m_moreButton->setMenu(nullptr);
}

void WorkspaceHeader::setNews(const QString& title, bool enabled)
{
    m_newsText = enabled ? tr("新闻") : tr("无新闻");
    m_newsButton->setText(m_newsText);
    m_newsButton->setToolTip(title);
    m_newsButton->setEnabled(enabled);
}

void WorkspaceHeader::setDownloadSourceStatus(const QString& summary, const QString& toolTip)
{
    m_downloadSourceSummary = summary;
    m_downloadSourceButton->setText(summary);
    m_downloadSourceButton->setToolTip(toolTip);
}

void WorkspaceHeader::setLaunchAction(QAction* action)
{
    if (m_launchAction != action) {
        m_launchAction = action;
        m_launchButton->setDefaultAction(action);
    }
    m_launchButton->setMenu(nullptr);
}

void WorkspaceHeader::setCurrentPage(Page page)
{
    auto* button = m_pageGroup->button(static_cast<int>(page));
    if (button && !button->isChecked()) {
        button->setChecked(true);
    }
    if (button) {
        updatePageIndicator(true);
    }
}

void WorkspaceHeader::updatePageIndicator(bool animate)
{
    auto* button = m_pageGroup->checkedButton();
    if (!button || !m_pageIndicator || !m_pageButtonHost) {
        return;
    }

    const auto target = button->geometry().adjusted(2, 2, -2, -2);
    if (!animate || m_pageIndicator->geometry().isEmpty()) {
        if (m_pageIndicatorAnimation) {
            m_pageIndicatorAnimation->stop();
        }
        m_pageIndicator->setGeometry(target);
        m_pageIndicator->show();
        m_pageIndicator->lower();
        return;
    }

    if (m_pageIndicatorAnimation) {
        m_pageIndicatorAnimation->stop();
    }
    m_pageIndicatorAnimation = new QPropertyAnimation(m_pageIndicator, "geometry", this);
    m_pageIndicatorAnimation->setDuration(220);
    m_pageIndicatorAnimation->setStartValue(m_pageIndicator->geometry());
    m_pageIndicatorAnimation->setEndValue(target);
    m_pageIndicatorAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_pageIndicatorAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    m_pageIndicator->show();
    m_pageIndicator->lower();
}

void WorkspaceHeader::resizeEvent(QResizeEvent* event)
{
    QFrame::resizeEvent(event);
    QTimer::singleShot(0, this, [this] { updatePageIndicator(false); });
}

void WorkspaceHeader::retranslate()
{
    m_home->setText(tr("主页"));
    m_instances->setText(tr("实例"));
    m_downloads->setText(tr("下载"));
    m_accounts->setText(tr("账户"));
    m_settings->setText(tr("设置"));
    if (m_instanceSelector->count() > 0) {
        m_instanceSelector->setItemText(0, tr("选择实例"));
    }
    m_instanceSelector->setToolTip(tr("选择需要启动或管理的游戏实例。"));
    m_downloadSourceButton->setText(m_downloadSourceSummary);
    m_downloadSourceButton->setToolTip(tr("打开元数据源和 Minecraft 下载源配置。"));
    m_newsButton->setText(m_newsText);
    m_newsButton->setToolTip(tr("查看启动器新闻。"));
    m_accountButton->setToolTip(tr("切换账户或打开账户设置。"));
    m_moreButton->setToolTip(tr("目录、主题、帮助和启动器操作。"));
}
