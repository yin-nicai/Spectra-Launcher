// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include "WorkspaceSideMenu.h"

#include <QButtonGroup>
#include <QLabel>
#include <QScrollArea>
#include <QToolButton>
#include <QVBoxLayout>

WorkspaceSideMenu::WorkspaceSideMenu(QWidget* parent) : QFrame(parent)
{
    setObjectName(QStringLiteral("workspaceSecondaryMenu"));
    setFrameShape(QFrame::NoFrame);
    setMinimumWidth(184);
    setMaximumWidth(212);

    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(10, 14, 10, 14);
    outer->setSpacing(8);

    m_title = new QLabel(this);
    m_title->setObjectName(QStringLiteral("workspaceSecondaryTitle"));
    outer->addWidget(m_title);

    auto scroll = new QScrollArea(this);
    scroll->setObjectName(QStringLiteral("workspaceSecondaryScroll"));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_content = new QWidget(scroll);
    m_itemsLayout = new QVBoxLayout(m_content);
    m_itemsLayout->setContentsMargins(0, 0, 0, 0);
    m_itemsLayout->setSpacing(3);
    m_itemsLayout->addStretch(1);
    scroll->setWidget(m_content);
    outer->addWidget(scroll, 1);
}

void WorkspaceSideMenu::setMenu(const QString& title, const QList<WorkspaceMenuItem>& items, const QString& selectedId)
{
    QStringList signature{ title };
    signature.reserve(items.size() * 3 + 1);
    for (const auto& item : items) {
        signature.append(item.id);
        signature.append(item.title);
        signature.append(item.enabled ? QStringLiteral("1") : QStringLiteral("0"));
    }

    if (signature == m_itemSignature) {
        select(selectedId);
        return;
    }

    setUpdatesEnabled(false);
    m_title->setText(title);
    clearItems();
    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);
    for (const auto& item : items) {
        auto* button = new QToolButton(m_content);
        button->setObjectName(QStringLiteral("workspaceSecondaryItem"));
        button->setText(item.title);
        button->setToolTip(item.toolTip);
        button->setIcon(item.icon);
        button->setIconSize(QSize(18, 18));
        button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        button->setCheckable(true);
        button->setEnabled(item.enabled);
        button->setAutoRaise(true);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        button->setMinimumHeight(34);
        button->setProperty("workspaceMenuId", item.id);
        m_group->addButton(button);
        m_buttons.insert(item.id, button);
        m_itemsLayout->insertWidget(m_itemsLayout->count() - 1, button);
        connect(button, &QToolButton::clicked, this, [this, id = item.id] { emit itemActivated(id); });
        if (item.id == selectedId) {
            button->setChecked(true);
        }
    }
    m_itemSignature = signature;
    m_itemsLayout->activate();
    setUpdatesEnabled(true);
    update();
}

void WorkspaceSideMenu::select(const QString& id)
{
    if (!m_group) {
        return;
    }
    if (auto* button = m_buttons.value(id)) {
        button->setChecked(true);
    }
}

void WorkspaceSideMenu::clearItems()
{
    if (m_group) {
        delete m_group;
        m_group = nullptr;
    }
    m_buttons.clear();
    while (m_itemsLayout->count() > 1) {
        auto* item = m_itemsLayout->takeAt(0);
        if (auto* button = item->widget()) {
            button->hide();
            delete button;
        }
        delete item;
    }
}
