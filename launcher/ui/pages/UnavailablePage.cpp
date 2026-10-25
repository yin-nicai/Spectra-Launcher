// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#include "UnavailablePage.h"

#include <QLabel>
#include <QVBoxLayout>

UnavailablePage::UnavailablePage(const QString& id, const QString& title, const QString& message, QWidget* parent)
    : QWidget(parent), m_id(id), m_title(title)
{
    auto layout = new QVBoxLayout(this);
    auto heading = new QLabel(title, this);
    QFont headingFont = heading->font();
    headingFont.setPointSize(headingFont.pointSize() + 3);
    headingFont.setWeight(QFont::DemiBold);
    heading->setFont(headingFont);
    auto body = new QLabel(message, this);
    body->setWordWrap(true);
    layout->addWidget(heading);
    layout->addWidget(body);
    layout->addStretch(1);
}
