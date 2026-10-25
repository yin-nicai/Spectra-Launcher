// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#pragma once

#include <QWidget>

#include "ui/pages/BasePage.h"

class UnavailablePage final : public QWidget, public BasePage {
   public:
    UnavailablePage(const QString& id, const QString& title, const QString& message, QWidget* parent = nullptr);

    QString id() const override { return m_id; }
    QString displayName() const override { return m_title; }
    QIcon icon() const override { return QIcon::fromTheme(QStringLiteral("dialog-warning")); }

   private:
    QString m_id;
    QString m_title;
};
