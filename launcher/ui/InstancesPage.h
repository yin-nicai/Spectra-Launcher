// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#pragma once

#include <QWidget>

class BaseInstance;
class InstanceView;
class QAction;
class QLabel;
class QMenu;
class QToolButton;

class InstancesPage final : public QWidget {
    Q_OBJECT

   public:
    explicit InstancesPage(InstanceView* view, QWidget* parent = nullptr);

    void setActions(QAction* addInstance,
                    QAction* launchAction,
                    QAction* editAction,
                    QAction* folderAction,
                    QMenu* instanceMenu,
                    QAction* undoTrashAction);
    void setInstance(BaseInstance* instance);
    void retranslate();

   private:
    InstanceView* m_view = nullptr;
    QLabel* m_title = nullptr;
    QLabel* m_instance = nullptr;
    QToolButton* m_add = nullptr;
    QToolButton* m_launch = nullptr;
    QToolButton* m_edit = nullptr;
    QToolButton* m_folder = nullptr;
    QToolButton* m_more = nullptr;
};
