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

#include <QApplication>
#include <QIcon>
#include <QString>
#include <QStyle>
#include <QStyleFactory>

namespace WorkspaceStyle {
inline QIcon icon(const QString& themeName, QStyle::StandardPixmap fallback)
{
    const auto themed = QIcon::fromTheme(themeName);
    return themed.isNull() ? QApplication::style()->standardIcon(fallback) : themed;
}

inline bool isNativeTheme(const QString& themeId)
{
    const auto normalizedId = themeId.trimmed().toLower();
    if (normalizedId.isEmpty() || normalizedId == QStringLiteral("system")) {
        return true;
    }
    if (normalizedId == QStringLiteral("fusion")) {
        return false;
    }
    for (const auto& styleName : QStyleFactory::keys()) {
        if (styleName.compare(normalizedId, Qt::CaseInsensitive) == 0) {
            return true;
        }
    }
    return false;
}

inline QString styleSheet(bool nativeTheme)
{
    QString style = QStringLiteral(R"QSS(
QToolButton#workspaceLaunchButton, QToolButton#homePrimaryAction, QToolButton#primaryActionButton {
    background: palette(highlight);
    color: palette(highlighted-text);
    border: 1px solid palette(highlight);
    border-radius: %2;
    padding: 0 16px;
    font-weight: 600;
}
QToolButton#workspaceLaunchButton:hover, QToolButton#homePrimaryAction:hover,
QToolButton#primaryActionButton:hover {
    border-color: palette(highlighted-text);
}
QToolButton#workspaceLaunchButton:disabled, QToolButton#homePrimaryAction:disabled,
QToolButton#primaryActionButton:disabled {
    background: palette(button);
    color: palette(disabled-text);
    border-color: palette(mid);
}
QToolButton#workspaceLaunchButton::menu-button, QToolButton#homePrimaryAction::menu-button,
QToolButton#primaryActionButton::menu-button {
    width: 24px;
    border: 0;
    background: transparent;
}
QFrame#workspaceHeader {
    background: palette(window);
    border: 0;
    border-bottom: 1px solid palette(mid);
}
QLabel#workspaceFieldLabel {
    color: palette(text);
    font-weight: 600;
    padding-left: 2px;
}
QFrame#homeHero {
    background: palette(alternate-base);
    border: 1px solid palette(mid);
    border-radius: %1;
}
QFrame#homeMetric {
    background: palette(button);
    border: 1px solid palette(mid);
    border-radius: %2;
}
QFrame#instancesViewFrame {
    background: palette(base);
    border: 1px solid palette(mid);
    border-radius: %1;
}
QFrame#downloadPolicyNote {
    background: palette(alternate-base);
    border: 1px solid palette(mid);
    border-radius: %2;
}
QFrame#accountCard {
    background: palette(button);
    border: 1px solid palette(mid);
    border-radius: %1;
}
QLabel#accountDefault {
    color: palette(highlight);
    font-weight: 600;
}
)QSS");
    style = style.arg(nativeTheme ? QStringLiteral("4px") : QStringLiteral("14px"))
                .arg(nativeTheme ? QStringLiteral("4px") : QStringLiteral("10px"));

    if (nativeTheme) {
        return style;
    }

    style += QStringLiteral(R"QSS(
QToolButton, QPushButton {
    min-height: 32px;
    padding: 0 12px;
    border: 1px solid palette(mid);
    border-radius: 8px;
    background: palette(button);
    color: palette(button-text);
}
QToolButton:hover, QPushButton:hover {
    background: palette(midlight);
    border-color: palette(dark);
}
QToolButton:pressed, QPushButton:pressed {
    background: palette(mid);
}
QToolButton:disabled, QPushButton:disabled {
    background: palette(button);
    color: palette(disabled-text);
    border-color: palette(mid);
}
QComboBox {
    min-height: 32px;
    padding: 0 10px;
    border: 1px solid palette(mid);
    border-radius: 8px;
    background: palette(button);
    color: palette(button-text);
}
QComboBox:hover, QComboBox:focus {
    border-color: palette(highlight);
}
QComboBox::drop-down {
    width: 28px;
    border: 0;
}
QCommandLinkButton {
    padding: 14px;
    border: 1px solid palette(mid);
    border-radius: 12px;
    background: palette(button);
    color: palette(button-text);
    text-align: left;
}
QCommandLinkButton:hover {
    border-color: palette(highlight);
    background: palette(midlight);
}
)QSS");
    return style;
}

inline QString styleSheetForTheme(const QString& themeId)
{
    return styleSheet(isNativeTheme(themeId));
}
}  // namespace WorkspaceStyle
