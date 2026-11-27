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
#include <QStyle>

namespace WorkspaceStyle {
inline QIcon icon(const QString& themeName, QStyle::StandardPixmap fallback)
{
    const auto themed = QIcon::fromTheme(themeName);
    return themed.isNull() ? QApplication::style()->standardIcon(fallback) : themed;
}

inline bool isNativeTheme(const QString&)
{
    return false;
}

inline QString styleSheet(bool)
{
    return QStringLiteral(R"QSS(
QWidget#workspaceRoot {
    background: transparent;
}
QWidget#workspaceContentShell {
    background: transparent;
}
QStackedWidget#workspacePages,
QWidget#homeDashboard,
QWidget#instancesViewFrame,
QWidget#downloadPolicyNote {
    background: transparent;
}
PageDialog,
PageDialog QWidget,
NewInstanceDialog,
NewInstanceDialog QWidget,
InstanceWindow,
InstanceWindow QWidget,
PageContainer,
PageContainer QWidget {
    background: transparent;
}
PageDialog QLineEdit,
PageDialog QComboBox,
PageDialog QSpinBox,
PageDialog QDoubleSpinBox,
PageDialog QPlainTextEdit,
PageDialog QTextEdit,
NewInstanceDialog QLineEdit,
NewInstanceDialog QComboBox,
NewInstanceDialog QSpinBox,
NewInstanceDialog QDoubleSpinBox,
NewInstanceDialog QPlainTextEdit,
NewInstanceDialog QTextEdit,
InstanceWindow QLineEdit,
InstanceWindow QComboBox,
InstanceWindow QSpinBox,
InstanceWindow QDoubleSpinBox,
InstanceWindow QPlainTextEdit,
InstanceWindow QTextEdit,
PageContainer QLineEdit,
PageContainer QComboBox,
PageContainer QSpinBox,
PageContainer QDoubleSpinBox,
PageContainer QPlainTextEdit,
PageContainer QTextEdit {
    color: #f3f5f8;
    background: rgba(10, 13, 18, 160);
    border: 1px solid rgba(255, 255, 255, 42);
}
PageDialog QPushButton,
PageDialog QToolButton,
NewInstanceDialog QPushButton,
NewInstanceDialog QToolButton,
InstanceWindow QPushButton,
InstanceWindow QToolButton,
PageContainer QPushButton,
PageContainer QToolButton {
    color: #f3f5f8;
    background: rgba(28, 33, 42, 190);
    border: 1px solid rgba(255, 255, 255, 42);
}
PageDialog QPushButton:hover,
PageDialog QToolButton:hover,
NewInstanceDialog QPushButton:hover,
NewInstanceDialog QToolButton:hover,
InstanceWindow QPushButton:hover,
InstanceWindow QToolButton:hover,
PageContainer QPushButton:hover,
PageContainer QToolButton:hover {
    background: rgba(35, 75, 108, 205);
    border-color: #318ce7;
}
PageDialog QGroupBox,
NewInstanceDialog QGroupBox,
InstanceWindow QGroupBox,
PageContainer QGroupBox,
PageDialog QTabWidget::pane,
NewInstanceDialog QTabWidget::pane,
InstanceWindow QTabWidget::pane,
PageContainer QTabWidget::pane {
    background: rgba(13, 16, 21, 145);
    border: 1px solid rgba(255, 255, 255, 34);
}
PageDialog QToolBar,
NewInstanceDialog QToolBar,
InstanceWindow QToolBar,
PageContainer QToolBar {
    background: rgba(16, 20, 26, 150);
    border: 0;
    border-bottom: 1px solid rgba(255, 255, 255, 26);
}
QFrame#workspaceHeader {
    background: rgba(17, 19, 23, 242);
    border: 0;
    border-bottom: 1px solid rgba(255, 255, 255, 34);
}
QFrame#workspaceSecondaryMenu {
    background: rgba(15, 18, 24, 188);
    border: 0;
    border-right: 1px solid rgba(255, 255, 255, 28);
}
QScrollArea#workspaceSecondaryScroll,
QScrollArea#workspaceSecondaryScroll > QWidget > QWidget {
    background: transparent;
    border: 0;
}
QLabel#workspaceSecondaryTitle {
    color: rgba(255, 255, 255, 150);
    font-size: 10px;
    font-weight: 600;
    padding: 2px 8px 7px 8px;
}
QToolButton#workspaceSecondaryItem {
    color: rgba(255, 255, 255, 205);
    background: transparent;
    border: 0;
    border-radius: 8px;
    padding: 6px 10px;
    text-align: left;
}
QToolButton#workspaceSecondaryItem:hover {
    color: #ffffff;
    background: rgba(255, 255, 255, 18);
}
QToolButton#workspaceSecondaryItem:checked {
    color: #ffffff;
    background: #0067c0;
}
QToolButton#workspaceSecondaryItem:disabled {
    color: rgba(255, 255, 255, 82);
    background: transparent;
}
QLabel#pclBrand {
    color: #f5f7fa;
    font-size: 16px;
    font-weight: 600;
    padding: 0 4px;
}
QLabel#pclFieldLabel {
    color: rgba(255, 255, 255, 155);
    padding: 0 3px;
}
QToolButton#pclNavButton {
    color: rgba(255, 255, 255, 200);
    background: transparent;
    border: 0;
    border-radius: 0;
    padding: 5px 13px;
}
QWidget#pclNavButtonHost {
    background: transparent;
}
QFrame#pclNavIndicator {
    background: #2378d7;
    border: 0;
    border-radius: 8px;
}
QToolButton#pclNavButton:hover {
    color: #ffffff;
    background: rgba(255, 255, 255, 22);
}
QToolButton#pclNavButton:checked {
    color: #ffffff;
    background: transparent;
}
QToolButton#pclUtilityButton {
    color: rgba(255, 255, 255, 205);
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0;
    padding: 4px 9px;
}
QToolButton#pclUtilityButton:hover {
    color: #ffffff;
    border-color: rgba(255, 255, 255, 54);
    background: rgba(255, 255, 255, 18);
}
QComboBox#pclInstanceSelector {
    min-height: 28px;
    color: #f4f6f9;
    background: rgba(9, 11, 15, 178);
    border: 1px solid rgba(255, 255, 255, 48);
    border-radius: 0;
    padding: 0 8px;
}
QComboBox#pclInstanceSelector:hover,
QComboBox#pclInstanceSelector:focus {
    border-color: #3a91ed;
}
QToolButton#pclLaunchButton,
QToolButton#pclLaunchButtonLarge,
QToolButton#pclPrimaryButton {
    color: #ffffff;
    background: #1677d2;
    border: 1px solid #1677d2;
    border-radius: 0;
    padding: 0 15px;
    font-weight: 600;
}
QToolButton#pclLaunchButton:hover,
QToolButton#pclLaunchButtonLarge:hover,
QToolButton#pclPrimaryButton:hover {
    background: #258be8;
    border-color: #258be8;
}
QToolButton#pclLaunchButton:disabled,
QToolButton#pclLaunchButtonLarge:disabled,
QToolButton#pclPrimaryButton:disabled {
    color: rgba(255, 255, 255, 110);
    background: rgba(90, 96, 108, 150);
    border-color: transparent;
}
QPushButton#pclStatusButton {
    color: rgba(255, 255, 255, 180);
    background: transparent;
    border: 1px solid transparent;
    border-radius: 0;
    padding: 4px 8px;
}
QPushButton#pclStatusButton:hover {
    color: #ffffff;
    border-color: rgba(255, 255, 255, 46);
}
QLabel#pclPageTitle {
    color: #ffffff;
    font-size: 28px;
    font-weight: 600;
}
QLabel#pclPageSubtitle,
QLabel#pclMutedText,
QLabel#pclFooterText {
    color: rgba(255, 255, 255, 155);
}
QFrame#pclHomePanel {
    background: rgba(13, 16, 21, 210);
    border: 1px solid rgba(255, 255, 255, 42);
    border-radius: 0;
}
QWidget#pclBrandBlock {
    background: transparent;
}
QWidget#pclLaunchProgressArea {
    background: transparent;
}
QLabel#pclLaunchProgressText {
    color: rgba(255, 255, 255, 205);
    font-size: 11px;
}
QLabel#pclLaunchProgressText[failed="true"] {
    color: #ff8f8f;
}
QProgressBar#pclLaunchProgress {
    min-height: 14px;
    max-height: 14px;
    color: #ffffff;
    background: rgba(255, 255, 255, 24);
    border: 0;
    border-radius: 7px;
    font-size: 9px;
    text-align: center;
}
QProgressBar#pclLaunchProgress::chunk {
    background: #318ce7;
    border-radius: 7px;
}
QProgressBar#pclLaunchProgress[failed="true"]::chunk {
    background: #c42b1c;
}
QLabel#pclProductName {
    color: #ffffff;
    font-size: 18px;
    font-weight: 700;
}
QLabel#pclProductVersion {
    color: rgba(255, 255, 255, 145);
    font-size: 10px;
}
QFrame#pclBrandDivider {
    color: rgba(255, 255, 255, 48);
}
QLabel#pclInstanceName {
    color: #ffffff;
    font-size: 21px;
    font-weight: 600;
}
QLabel#pclStatusText {
    color: #7bc6ff;
}
QToolButton#pclSecondaryButton {
    color: rgba(255, 255, 255, 215);
    background: rgba(16, 19, 24, 185);
    border: 1px solid rgba(255, 255, 255, 42);
    border-radius: 0;
    padding: 0 12px;
}
QToolButton#pclSecondaryButton:hover,
QPushButton#pclQuickButton:hover {
    color: #ffffff;
    border-color: #318ce7;
    background: rgba(30, 60, 90, 210);
}
QPushButton#pclQuickButton {
    color: rgba(255, 255, 255, 225);
    background: rgba(13, 16, 21, 195);
    border: 1px solid rgba(255, 255, 255, 38);
    border-radius: 0;
    padding: 0 16px;
    text-align: left;
}
QCommandLinkButton {
    color: #f2f5f8;
    background: rgba(13, 16, 21, 195);
    border: 1px solid rgba(255, 255, 255, 38);
    border-radius: 0;
    padding: 10px 12px;
    text-align: left;
}
QCommandLinkButton:hover {
    color: #ffffff;
    border-color: #318ce7;
    background: rgba(30, 60, 90, 210);
}
QWidget#homeDashboard QLabel,
QStackedWidget#workspacePages QLabel {
    color: #f5f7fa;
}
QStackedWidget#workspacePages QScrollArea,
QStackedWidget#workspacePages QScrollArea > QWidget > QWidget {
    background: transparent;
}
PageDialog QTreeView,
PageDialog QListView,
PageDialog QTableView,
NewInstanceDialog QTreeView,
NewInstanceDialog QListView,
NewInstanceDialog QTableView,
InstanceWindow QTreeView,
InstanceWindow QListView,
InstanceWindow QTableView,
PageContainer QTreeView,
PageContainer QListView,
PageContainer QTableView,
QTreeView,
QListView,
QTableView {
    color: #f3f5f8;
    background: rgba(10, 13, 18, 172);
    alternate-background-color: rgba(22, 27, 35, 165);
    border: 1px solid rgba(255, 255, 255, 36);
    border-radius: 0;
    selection-background-color: #2378d7;
    selection-color: #ffffff;
}
QHeaderView::section {
    color: rgba(255, 255, 255, 190);
    background: rgba(19, 23, 29, 232);
    border: 0;
    border-right: 1px solid rgba(255, 255, 255, 28);
    border-bottom: 1px solid rgba(255, 255, 255, 28);
    padding: 6px 8px;
}
)QSS");
}

inline QString styleSheetForTheme(const QString& themeId)
{
    QString style = styleSheet(false);
    const auto id = themeId.trimmed().toLower();
    if (id == QStringLiteral("bright")) {
        style += QStringLiteral(R"QSS(
QFrame#workspaceHeader {
    background: rgba(248, 250, 252, 242);
    border-bottom: 1px solid rgba(15, 23, 42, 42);
}
QLabel#pclBrand { color: #111827; }
QLabel#pclFieldLabel { color: rgba(17, 24, 39, 150); }
QToolButton#pclNavButton { color: #374151; }
QToolButton#pclNavButton:hover { color: #111827; background: rgba(15, 23, 42, 16); }
QToolButton#pclNavButton:checked { color: #ffffff; background: transparent; }
QFrame#pclNavIndicator { background: #1677d2; }
QToolButton#pclUtilityButton { color: #374151; }
QToolButton#pclUtilityButton:hover { color: #111827; border-color: rgba(15, 23, 42, 40); }
QComboBox#pclInstanceSelector { color: #111827; background: rgba(255, 255, 255, 225); border-color: rgba(15, 23, 42, 55); }
QPushButton#pclStatusButton { color: #374151; }
QLabel#pclPageTitle { color: #ffffff; }
QLabel#pclPageSubtitle, QLabel#pclMutedText, QLabel#pclFooterText { color: rgba(255, 255, 255, 185); }
QFrame#pclHomePanel { background: rgba(248, 250, 252, 224); border-color: rgba(255, 255, 255, 120); }
QWidget#homeDashboard QLabel#pclProductName { color: #111827; }
QWidget#homeDashboard QLabel#pclProductVersion { color: #64748b; }
QFrame#pclBrandDivider { color: rgba(15, 23, 42, 50); }
QLabel#pclInstanceName { color: #111827; }
QWidget#homeDashboard QLabel { color: #f8fafc; }
QWidget#homeDashboard QLabel#pclInstanceName { color: #111827; }
QWidget#homeDashboard QLabel#pclMutedText { color: #4b5563; }
QWidget#homeDashboard QLabel#pclStatusText { color: #0369a1; }
QToolButton#pclSecondaryButton { color: #111827; background: rgba(255, 255, 255, 225); border-color: rgba(15, 23, 42, 42); }
QPushButton#pclQuickButton { color: #f8fafc; }
QCommandLinkButton { color: #f8fafc; }
)QSS");
    } else if (id == QStringLiteral("windows")) {
        style += QStringLiteral(R"QSS(
QFrame#workspaceHeader { background: #c0c0c0; border-bottom: 2px solid #404040; }
QLabel#pclBrand { color: #000000; font-weight: 700; }
QLabel#pclFieldLabel { color: #000000; }
QToolButton#pclNavButton { color: #000000; background: transparent; border-radius: 0; }
QToolButton#pclNavButton:hover { color: #000000; background: #d4d0c8; }
QToolButton#pclNavButton:checked { color: #ffffff; background: transparent; }
QFrame#pclNavIndicator { background: #000080; border-radius: 0; }
QToolButton#pclUtilityButton { color: #000000; border-radius: 0; }
QComboBox#pclInstanceSelector { color: #000000; background: #ffffff; border: 2px inset #808080; border-radius: 0; }
QToolButton#pclLaunchButton, QToolButton#pclLaunchButtonLarge, QToolButton#pclPrimaryButton {
    color: #000000; background: #c0c0c0; border: 2px outset #ffffff; border-radius: 0;
}
QLabel#pclPageTitle { color: #ffffff; }
QLabel#pclPageSubtitle, QLabel#pclMutedText, QLabel#pclFooterText { color: #e5e7eb; }
QFrame#pclHomePanel { background: rgba(192, 192, 192, 238); border: 2px solid #808080; border-radius: 0; }
QWidget#homeDashboard QLabel#pclProductName { color: #000000; }
QWidget#homeDashboard QLabel#pclProductVersion { color: #202020; }
QFrame#pclBrandDivider { color: #808080; }
QLabel#pclInstanceName { color: #000000; }
QWidget#homeDashboard QLabel#pclMutedText { color: #202020; }
QWidget#homeDashboard QLabel#pclStatusText { color: #000080; }
QToolButton#pclSecondaryButton, QPushButton#pclQuickButton {
    color: #000000; background: #c0c0c0; border: 2px outset #ffffff; border-radius: 0;
}
)QSS");
    } else if (id == QStringLiteral("windows11")) {
        style += QStringLiteral(R"QSS(
QFrame#workspaceHeader {
    background: rgba(24, 26, 30, 242);
    border-bottom: 1px solid rgba(255, 255, 255, 30);
}
QLabel#pclBrand { color: #f4f6f9; }
QLabel#pclFieldLabel { color: rgba(244, 246, 249, 155); }
QToolButton#pclNavButton {
    color: rgba(244, 246, 249, 210); border-radius: 8px; padding: 5px 14px;
}
QToolButton#pclNavButton:hover { color: #ffffff; background: rgba(255, 255, 255, 18); }
QToolButton#pclNavButton:checked { color: #ffffff; background: transparent; border-radius: 8px; }
QFrame#pclNavIndicator { background: #0067c0; border-radius: 8px; }
QToolButton#pclUtilityButton {
    color: rgba(244, 246, 249, 210); border: 1px solid transparent; border-radius: 8px; padding: 4px 9px;
}
QToolButton#pclUtilityButton:hover { color: #ffffff; border-color: rgba(255, 255, 255, 36); background: rgba(255, 255, 255, 16); }
QComboBox#pclInstanceSelector, QToolButton#pclLaunchButton, QToolButton#pclLaunchButtonLarge,
QToolButton#pclPrimaryButton, QToolButton#pclSecondaryButton, QPushButton#pclQuickButton,
QPushButton#pclStatusButton {
    border-radius: 8px;
}
QComboBox#pclInstanceSelector { color: #f4f6f9; background: rgba(13, 15, 19, 205); border-color: rgba(255, 255, 255, 38); }
QToolButton#pclLaunchButton, QToolButton#pclLaunchButtonLarge, QToolButton#pclPrimaryButton {
    background: #0067c0; border-color: #0067c0;
}
QPushButton#pclStatusButton { color: rgba(244, 246, 249, 205); }
QLabel#pclPageTitle { color: #ffffff; }
QLabel#pclPageSubtitle, QLabel#pclMutedText, QLabel#pclFooterText { color: rgba(255, 255, 255, 188); }
QFrame#pclHomePanel {
    background: rgba(25, 28, 34, 225);
    border: 1px solid rgba(255, 255, 255, 48);
    border-radius: 14px;
}
QWidget#homeDashboard QLabel#pclProductName { color: #ffffff; }
QWidget#homeDashboard QLabel#pclProductVersion { color: rgba(244, 246, 249, 145); }
QFrame#pclBrandDivider { color: rgba(255, 255, 255, 42); }
QLabel#pclInstanceName { color: #ffffff; }
QWidget#homeDashboard QLabel { color: #f8fafc; }
QWidget#homeDashboard QLabel#pclInstanceName { color: #ffffff; }
QWidget#homeDashboard QLabel#pclMutedText { color: rgba(244, 246, 249, 168); }
QWidget#homeDashboard QLabel#pclStatusText { color: #64b5f6; }
QToolButton#pclSecondaryButton, QPushButton#pclQuickButton {
    color: #f4f6f9; background: rgba(31, 35, 42, 220); border-color: rgba(255, 255, 255, 36); border-radius: 8px;
}
QCommandLinkButton { color: #f8fafc; border-radius: 10px; }
QTreeView, QListView, QTableView { border-radius: 10px; }
QProgressBar#pclLaunchProgress {
    background: rgba(255, 255, 255, 20);
    border-radius: 7px;
}
QProgressBar#pclLaunchProgress::chunk {
    background: #0067c0;
    border-radius: 7px;
}
QProgressBar#pclLaunchProgress[failed="true"]::chunk {
    background: #c42b1c;
}
PageDialog QLineEdit,
PageDialog QComboBox,
PageDialog QSpinBox,
PageDialog QDoubleSpinBox,
PageDialog QPlainTextEdit,
PageDialog QTextEdit,
NewInstanceDialog QLineEdit,
NewInstanceDialog QComboBox,
NewInstanceDialog QSpinBox,
NewInstanceDialog QDoubleSpinBox,
NewInstanceDialog QPlainTextEdit,
NewInstanceDialog QTextEdit,
InstanceWindow QLineEdit,
InstanceWindow QComboBox,
InstanceWindow QSpinBox,
InstanceWindow QDoubleSpinBox,
InstanceWindow QPlainTextEdit,
InstanceWindow QTextEdit,
PageContainer QLineEdit,
PageContainer QComboBox,
PageContainer QSpinBox,
PageContainer QDoubleSpinBox,
PageContainer QPlainTextEdit,
PageContainer QTextEdit,
PageDialog QPushButton,
PageDialog QToolButton,
NewInstanceDialog QPushButton,
NewInstanceDialog QToolButton,
InstanceWindow QPushButton,
InstanceWindow QToolButton,
PageContainer QPushButton,
PageContainer QToolButton {
    border-radius: 8px;
}
PageDialog QGroupBox,
NewInstanceDialog QGroupBox,
InstanceWindow QGroupBox,
PageContainer QGroupBox,
PageDialog QTabWidget::pane,
NewInstanceDialog QTabWidget::pane,
InstanceWindow QTabWidget::pane,
PageContainer QTabWidget::pane,
PageDialog QToolBar,
NewInstanceDialog QToolBar,
InstanceWindow QToolBar,
PageContainer QToolBar {
    border-radius: 10px;
}
PageDialog QTreeView,
PageDialog QListView,
PageDialog QTableView,
NewInstanceDialog QTreeView,
NewInstanceDialog QListView,
NewInstanceDialog QTableView,
InstanceWindow QTreeView,
InstanceWindow QListView,
InstanceWindow QTableView,
PageContainer QTreeView,
PageContainer QListView,
PageContainer QTableView {
    border-radius: 10px;
}
)QSS");
    }
    return style;
}

inline QString applicationStyleSheet(const QString& themeId = QString())
{
    QString style = QStringLiteral(R"QSS(
QMainWindow, QDialog {
    background: palette(window);
    color: palette(window-text);
}
QPushButton, QToolButton, QComboBox, QLineEdit, QPlainTextEdit, QTextEdit,
QSpinBox, QDoubleSpinBox, QDateEdit, QDateTimeEdit, QTimeEdit {
    border: 1px solid palette(mid);
    border-radius: 0;
}
QPushButton, QToolButton {
    min-height: 28px;
    padding: 0 10px;
    background: palette(button);
    color: palette(button-text);
}
QPushButton:hover, QToolButton:hover {
    background: palette(midlight);
    border-color: palette(dark);
}
QPushButton:pressed, QToolButton:pressed,
QPushButton:checked, QToolButton:checked {
    background: palette(highlight);
    color: palette(highlighted-text);
    border-color: palette(highlight);
}
QPushButton:disabled, QToolButton:disabled {
    color: palette(disabled-text);
    background: palette(button);
}
QComboBox, QLineEdit, QPlainTextEdit, QTextEdit, QSpinBox, QDoubleSpinBox,
QDateEdit, QDateTimeEdit, QTimeEdit {
    min-height: 28px;
    padding: 0 7px;
    background: palette(base);
    color: palette(text);
}
QMenu {
    padding: 3px;
    background: palette(base);
    color: palette(text);
    border: 1px solid palette(mid);
}
QMenu::item {
    padding: 5px 24px;
    border-radius: 0;
}
QMenu::item:selected {
    background: palette(highlight);
    color: palette(highlighted-text);
}
QMenu::separator {
    height: 1px;
    background: palette(mid);
    margin: 4px 6px;
}
QTabWidget::pane {
    border: 1px solid palette(mid);
    border-radius: 0;
    background: palette(base);
}
QTabBar::tab {
    min-height: 28px;
    padding: 0 12px;
    border: 1px solid palette(mid);
    border-bottom: 0;
    border-radius: 0;
    background: palette(window);
}
QTabBar::tab:selected {
    background: palette(base);
    border-color: palette(highlight);
}
QGroupBox {
    border: 1px solid palette(mid);
    border-radius: 0;
    margin-top: 12px;
    padding-top: 12px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 4px;
}
QTreeView, QListView, QTableView, QAbstractItemView {
    border: 1px solid palette(mid);
    border-radius: 0;
    background: palette(base);
    alternate-background-color: palette(alternate-base);
    selection-background-color: palette(highlight);
    selection-color: palette(highlighted-text);
}
QHeaderView::section {
    border: 0;
    border-right: 1px solid palette(mid);
    border-bottom: 1px solid palette(mid);
    padding: 6px;
    background: palette(window);
}
QScrollArea {
    border: 0;
    background: transparent;
}
QScrollBar:vertical {
    width: 10px;
    background: transparent;
}
QScrollBar::handle:vertical {
    min-height: 24px;
    background: palette(mid);
    border-radius: 0;
}
QScrollBar:horizontal {
    height: 10px;
    background: transparent;
}
QScrollBar::handle:horizontal {
    min-width: 24px;
    background: palette(mid);
    border-radius: 0;
}
QScrollBar::add-line, QScrollBar::sub-line {
    width: 0;
    height: 0;
}
QToolTip {
    padding: 5px;
    border: 1px solid palette(mid);
    border-radius: 0;
    background: palette(tooltip-base);
    color: palette(tooltip-text);
}
QProgressBar {
    min-height: 8px;
    max-height: 8px;
    border: 0;
    border-radius: 0;
    background: palette(alternate-base);
}
QProgressBar::chunk {
    background: palette(highlight);
}
)QSS");
    if (themeId.trimmed().compare(QStringLiteral("windows11"), Qt::CaseInsensitive) == 0) {
        style += QStringLiteral(R"QSS(
QPushButton, QToolButton, QComboBox, QLineEdit, QPlainTextEdit, QTextEdit,
QSpinBox, QDoubleSpinBox, QDateEdit, QDateTimeEdit, QTimeEdit,
QGroupBox, QTabWidget::pane, QTreeView, QListView, QTableView {
    border-radius: 8px;
}
QMenu { border-radius: 8px; padding: 5px; }
QMenu::item { border-radius: 5px; }
)QSS");
    }
    return style;
}
}  // namespace WorkspaceStyle
