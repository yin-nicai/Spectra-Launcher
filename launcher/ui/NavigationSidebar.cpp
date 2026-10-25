// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#include "NavigationSidebar.h"

#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "Application.h"
#include "BuildConfig.h"

NavigationSidebar::NavigationSidebar(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("navigationSidebar"));
    setFixedWidth(204);
    setFrameShape(QFrame::NoFrame);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 16, 12, 16);
    layout->setSpacing(5);

    auto brandRow = new QHBoxLayout();
    auto logo = new QLabel(this);
    logo->setPixmap(APPLICATION->logo().pixmap(36));
    logo->setFixedSize(40, 40);
    logo->setScaledContents(true);
    brandRow->addWidget(logo);
    m_title = new QLabel(this);
    QFont titleFont = m_title->font();
    titleFont.setPointSize(titleFont.pointSize() + 3);
    titleFont.setWeight(QFont::DemiBold);
    m_title->setFont(titleFont);
    brandRow->addWidget(m_title, 1);
    layout->addLayout(brandRow);
    layout->addSpacing(14);

    auto group = new QButtonGroup(this);
    group->setExclusive(true);
    m_home = addItem(QStringLiteral("go-home"), tr("Home"), Item::Home, group);
    m_instances = addItem(QStringLiteral("view-list-details"), tr("Instances"), Item::Instances, group);
    m_downloads = addItem(QStringLiteral("download"), tr("Downloads"), Item::Downloads, group);
    layout->addSpacing(8);
    m_accounts = addItem(QStringLiteral("accounts"), tr("Accounts"), Item::Accounts, group);
    layout->addSpacing(10);
    m_settings = addItem(QStringLiteral("settings"), tr("Settings"), Item::Settings, group);

    layout->addStretch(1);
    auto footerLine = new QFrame(this);
    footerLine->setFrameShape(QFrame::HLine);
    footerLine->setFrameShadow(QFrame::Sunken);
    layout->addWidget(footerLine);
    layout->addSpacing(4);
    m_instanceLabel = new QLabel(this);
    m_instanceLabel->setWordWrap(true);
    m_instanceLabel->setObjectName(QStringLiteral("navigationContext"));
    layout->addWidget(m_instanceLabel);
    m_accountLabel = new QLabel(this);
    m_accountLabel->setWordWrap(true);
    m_accountLabel->setObjectName(QStringLiteral("navigationContext"));
    layout->addWidget(m_accountLabel);
    m_version = new QLabel(BuildConfig.printableVersionString(), this);
    m_version->setObjectName(QStringLiteral("navigationVersion"));
    layout->addWidget(m_version);

    connect(group, &QButtonGroup::idClicked, this, [this](int id) {
        if (id >= 0) {
            emit itemSelected(static_cast<Item>(id));
        }
    });
    m_home->setChecked(true);

    applyTheme(false);
    updateTexts();
}

QToolButton* NavigationSidebar::addItem(const QString& iconName, const QString& text, Item item, QButtonGroup* group)
{
    auto button = new QToolButton(this);
    button->setCheckable(true);
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setIcon(QIcon::fromTheme(iconName));
    button->setIconSize(QSize(20, 20));
    button->setText(text);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    button->setMinimumHeight(40);
    group->addButton(button, static_cast<int>(item));
    layout()->addWidget(button);
    return button;
}

void NavigationSidebar::setCurrentItem(Item item)
{
    switch (item) {
        case Item::Home:
            m_home->setChecked(true);
            break;
        case Item::Instances:
            m_instances->setChecked(true);
            break;
        case Item::Downloads:
            m_downloads->setChecked(true);
            break;
        case Item::Accounts:
            m_accounts->setChecked(true);
            break;
        case Item::Settings:
            m_settings->setChecked(true);
            break;
    }
}

void NavigationSidebar::setAccountName(const QString& name)
{
    m_accountName = name;
    updateTexts();
}

void NavigationSidebar::setInstanceName(const QString& name)
{
    m_instanceName = name;
    updateTexts();
}

void NavigationSidebar::retranslate()
{
    updateTexts();
}

void NavigationSidebar::applyTheme(bool nativeTheme)
{
    QString style = QStringLiteral(
        "QFrame#navigationSidebar { background: palette(window); border-right: 1px solid palette(mid); }"
        "QToolButton:checked { background: palette(highlight); color: palette(highlighted-text); }"
        "QLabel#navigationContext { color: palette(text); padding: 3px 8px; }"
        "QLabel#navigationVersion { color: palette(mid); padding: 3px 8px; }");
    if (!nativeTheme) {
        style += QStringLiteral(
            "QToolButton { text-align: left; padding: 9px 11px; border: 0; border-radius: 8px; }"
            "QToolButton:hover { background: palette(alternate-base); }");
    }
    if (styleSheet() != style) {
        setStyleSheet(style);
    }
}

void NavigationSidebar::updateTexts()
{
    m_title->setText(tr("Spectra"));
    m_home->setText(tr("Home"));
    m_instances->setText(tr("Instances"));
    m_downloads->setText(tr("Downloads"));
    m_accounts->setText(tr("Accounts"));
    m_settings->setText(tr("Settings"));
    m_instanceLabel->setText(m_instanceName.isEmpty() ? tr("No instance selected") : tr("Instance: %1").arg(m_instanceName));
    m_accountLabel->setText(m_accountName.isEmpty() ? tr("No account") : m_accountName);
}
