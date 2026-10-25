// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "AccountsPage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "Application.h"
#include "minecraft/auth/AccountList.h"
#include "minecraft/auth/MinecraftAccount.h"

AccountsPage::AccountsPage(QWidget* parent) : QWidget(parent)
{
    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(28, 24, 28, 28);
    outer->setSpacing(10);

    auto header = new QHBoxLayout();
    auto heading = new QVBoxLayout();
    heading->setSpacing(3);
    m_title = new QLabel(this);
    QFont titleFont = m_title->font();
    titleFont.setPointSize(titleFont.pointSize() + 7);
    titleFont.setWeight(QFont::DemiBold);
    m_title->setFont(titleFont);
    m_summary = new QLabel(this);
    m_summary->setEnabled(false);
    heading->addWidget(m_title);
    heading->addWidget(m_summary);
    header->addLayout(heading, 1);
    m_manage = new QPushButton(this);
    m_manage->setIcon(QIcon::fromTheme(QStringLiteral("accounts")));
    m_manage->setMinimumHeight(36);
    header->addWidget(m_manage);
    outer->addLayout(header);

    m_subtitle = new QLabel(this);
    m_subtitle->setWordWrap(true);
    m_subtitle->setEnabled(false);
    outer->addWidget(m_subtitle);

    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    auto content = new QWidget(scroll);
    m_list = new QVBoxLayout(content);
    m_list->setContentsMargins(0, 8, 0, 0);
    m_list->setSpacing(10);
    scroll->setWidget(content);
    outer->addWidget(scroll, 1);

    connect(m_manage, &QPushButton::clicked, this, [this] { APPLICATION->ShowGlobalSettings(this, QStringLiteral("accounts")); });
    connect(APPLICATION->accounts(), &AccountList::listChanged, this, &AccountsPage::refresh);
    connect(APPLICATION->accounts(), &AccountList::defaultAccountChanged, this, &AccountsPage::refresh);
    retranslate();
}

void AccountsPage::clearRows()
{
    while (auto* item = m_list->takeAt(0)) {
        if (auto* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void AccountsPage::refresh()
{
    clearRows();
    const auto accounts = APPLICATION->accounts();
    const auto defaultAccount = accounts->defaultAccount();
    m_summary->setText(defaultAccount ? tr("Active account: %1").arg(defaultAccount->displayName()) : tr("No active account"));

    if (accounts->count() == 0) {
        auto empty = new QFrame(m_list->parentWidget());
        empty->setObjectName(QStringLiteral("accountCard"));
        auto emptyLayout = new QVBoxLayout(empty);
        emptyLayout->setContentsMargins(24, 30, 24, 30);
        emptyLayout->setAlignment(Qt::AlignCenter);
        auto icon = new QLabel(empty);
        icon->setPixmap(QIcon::fromTheme(QStringLiteral("noaccount")).pixmap(56));
        icon->setAlignment(Qt::AlignCenter);
        emptyLayout->addWidget(icon);
        auto text = new QLabel(tr("No account configured."), empty);
        text->setAlignment(Qt::AlignCenter);
        emptyLayout->addWidget(text);
        auto add = new QPushButton(tr("Manage accounts"), empty);
        connect(add, &QPushButton::clicked, this, [this] { APPLICATION->ShowGlobalSettings(this, QStringLiteral("accounts")); });
        emptyLayout->addWidget(add, 0, Qt::AlignCenter);
        m_list->addWidget(empty);
    } else {
        for (int index = 0; index < accounts->count(); ++index) {
            const auto account = accounts->at(index);
            const bool isDefault = account == defaultAccount;

            auto row = new QFrame(m_list->parentWidget());
            row->setObjectName(QStringLiteral("accountCard"));
            auto rowLayout = new QHBoxLayout(row);
            rowLayout->setContentsMargins(14, 12, 14, 12);
            rowLayout->setSpacing(12);

            auto avatar = new QLabel(row);
            avatar->setPixmap(account->getFace().scaled(44, 44, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            avatar->setFixedSize(48, 48);
            avatar->setAlignment(Qt::AlignCenter);
            rowLayout->addWidget(avatar);

            auto details = new QVBoxLayout();
            details->setSpacing(2);
            auto name = new QLabel(account->displayName(), row);
            QFont nameFont = name->font();
            nameFont.setWeight(QFont::DemiBold);
            name->setFont(nameFont);
            details->addWidget(name);
            details->addWidget(
                new QLabel(account->accountType() == AccountType::Offline ? tr("Offline account") : tr("Microsoft account"), row));
            rowLayout->addLayout(details, 1);

            if (isDefault) {
                auto badge = new QLabel(tr("Default"), row);
                badge->setObjectName(QStringLiteral("accountDefault"));
                rowLayout->addWidget(badge);
            } else {
                auto use = new QPushButton(tr("Use"), row);
                connect(use, &QPushButton::clicked, this, [account] { APPLICATION->accounts()->setDefaultAccount(account); });
                rowLayout->addWidget(use);
            }
            m_list->addWidget(row);
        }
    }
    m_list->addStretch(1);
}

void AccountsPage::retranslate()
{
    m_title->setText(tr("Accounts"));
    m_manage->setText(tr("Manage accounts"));
    m_subtitle->setText(
        tr("Use a Microsoft account for online play, or an offline account for local play. Please support genuine Minecraft."));
    refresh();
}
