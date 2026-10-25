// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#pragma once

#include <QFrame>

class QLabel;
class QToolButton;

class NavigationSidebar final : public QFrame {
    Q_OBJECT

   public:
    enum class Item { Home = 0, Instances, Downloads, Accounts, Settings };

    explicit NavigationSidebar(QWidget* parent = nullptr);

    void setCurrentItem(Item item);
    void setAccountName(const QString& name);
    void setInstanceName(const QString& name);
    void retranslate();
    void applyTheme(bool nativeTheme);

   signals:
    void itemSelected(Item item);

   private:
    QToolButton* addItem(const QString& iconName, const QString& text, Item item, class QButtonGroup* group);
    void updateTexts();

    QLabel* m_title = nullptr;
    QLabel* m_version = nullptr;
    QLabel* m_instanceLabel = nullptr;
    QLabel* m_accountLabel = nullptr;
    QToolButton* m_home = nullptr;
    QToolButton* m_instances = nullptr;
    QToolButton* m_downloads = nullptr;
    QToolButton* m_accounts = nullptr;
    QToolButton* m_settings = nullptr;
    QString m_accountName;
    QString m_instanceName;
};
