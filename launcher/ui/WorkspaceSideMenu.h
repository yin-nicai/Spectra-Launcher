// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#pragma once

#include <QFrame>
#include <QHash>
#include <QIcon>
#include <QList>
#include <QString>
#include <QStringList>

class QButtonGroup;
class QLabel;
class QToolButton;
class QVBoxLayout;

struct WorkspaceMenuItem {
    QString id;
    QString title;
    QString toolTip;
    QIcon icon;
    bool enabled = true;
};

class WorkspaceSideMenu final : public QFrame {
    Q_OBJECT

   public:
    explicit WorkspaceSideMenu(QWidget* parent = nullptr);

    void setMenu(const QString& title, const QList<WorkspaceMenuItem>& items, const QString& selectedId = {});
    void select(const QString& id);

   signals:
    void itemActivated(const QString& id);

   private:
    void clearItems();

    QLabel* m_title = nullptr;
    QWidget* m_content = nullptr;
    QVBoxLayout* m_itemsLayout = nullptr;
    QButtonGroup* m_group = nullptr;
    QHash<QString, QToolButton*> m_buttons;
    QStringList m_itemSignature;
};
