// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "SettingsHubPage.h"

#include <QCommandLinkButton>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include "Application.h"
#include "ui/WorkspaceStyle.h"

namespace {
QLabel* makeSectionTitle(QWidget* parent)
{
    auto title = new QLabel(parent);
    QFont font = title->font();
    font.setWeight(QFont::DemiBold);
    font.setPointSize(font.pointSize() + 2);
    title->setFont(font);
    return title;
}

QGridLayout* makeGrid()
{
    auto grid = new QGridLayout();
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(12);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);
    return grid;
}
}  // namespace

SettingsHubPage::SettingsHubPage(QWidget* parent) : QWidget(parent)
{
    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outer->addWidget(scroll);

    auto content = new QWidget(scroll);
    auto layout = new QVBoxLayout(content);
    layout->setContentsMargins(28, 24, 28, 28);
    layout->setSpacing(12);

    auto title = new QLabel(tr("Settings"), content);
    QFont titleFont = title->font();
    titleFont.setPointSize(titleFont.pointSize() + 7);
    titleFont.setWeight(QFont::DemiBold);
    title->setFont(titleFont);
    layout->addWidget(title);

    auto subtitle = new QLabel(
        tr("Choose a settings category. Existing settings are reused and are applied with the same validation and save flow."), content);
    subtitle->setWordWrap(true);
    subtitle->setEnabled(false);
    layout->addWidget(subtitle);
    layout->addSpacing(4);

    auto commonTitle = makeSectionTitle(content);
    commonTitle->setText(tr("Common"));
    layout->addWidget(commonTitle);
    auto common = makeGrid();
    addCard(common, 0, 0, tr("Java and memory"), tr("Java runtime, memory limits, and JVM arguments."),
            QStringLiteral("settings-configure"), QStringLiteral("java-settings"));
    addCard(common, 0, 1, tr("Launcher"), tr("Concurrency, folders, and launcher behavior."), QStringLiteral("settings"),
            QStringLiteral("launcher-settings"));
    addCard(common, 1, 0, tr("Minecraft"), tr("Window, quick play, and game compatibility settings."), QStringLiteral("minecraft"),
            QStringLiteral("minecraft-settings"));
    addCard(common, 1, 1, tr("Appearance"), tr("Theme, language, and interface display."), QStringLiteral("preferences-desktop-theme"),
            QStringLiteral("appearance-settings"));
    layout->addLayout(common);

    auto networkTitle = makeSectionTitle(content);
    networkTitle->setText(tr("Sources and network"));
    layout->addWidget(networkTitle);
    auto network = makeGrid();
    addCard(network, 0, 0, tr("Plugins and sources"), tr("Metadata, downloads, and resource platforms."), QStringLiteral("plugins"),
            QStringLiteral("plugins"));
    addCard(network, 0, 1, tr("Directories"), tr("Instance, game, asset, log, and crash-report roots."), QStringLiteral("folder"),
            QStringLiteral("directories"));
    addCard(network, 1, 0, tr("APIs and downloads"), tr("Metadata URLs, paste services, and download configuration."),
            QStringLiteral("download"), QStringLiteral("apis"));
    addCard(network, 1, 1, tr("Proxy"), tr("Network proxy configuration."), QStringLiteral("network-workgroup"),
            QStringLiteral("proxy-settings"));
    layout->addLayout(network);

    auto advancedTitle = makeSectionTitle(content);
    advancedTitle->setText(tr("Advanced and help"));
    layout->addWidget(advancedTitle);
    auto advanced = makeGrid();
    addCard(advanced, 0, 0, tr("Language"), tr("Select the launcher language."), QStringLiteral("preferences-desktop-locale"),
            QStringLiteral("language-settings"));
    addCard(advanced, 0, 1, tr("External tools"), tr("Configure external programs and integrations."),
            QStringLiteral("applications-system"), QStringLiteral("external-tools"));
    addCard(advanced, 1, 0, tr("Help and documentation"), tr("Open the wiki, logs, and community links."), QStringLiteral("help"),
            QStringLiteral("__help__"));
    addCard(advanced, 1, 1, tr("About this build"), tr("Version, license, disclaimer, and third-party notices."), QStringLiteral("about"),
            QStringLiteral("__about__"));
    layout->addLayout(advanced);
    layout->addStretch(1);
    scroll->setWidget(content);
}

void SettingsHubPage::addCard(QGridLayout* layout,
                              int row,
                              int column,
                              const QString& title,
                              const QString& description,
                              const QString& icon,
                              const QString& pageId)
{
    auto card = new QCommandLinkButton(title, description, this);
    auto fallback = QStyle::SP_FileDialogDetailedView;
    if (pageId == QStringLiteral("directories")) {
        fallback = QStyle::SP_DirOpenIcon;
    } else if (pageId == QStringLiteral("proxy-settings")) {
        fallback = QStyle::SP_DriveNetIcon;
    } else if (pageId == QStringLiteral("external-tools")) {
        fallback = QStyle::SP_ComputerIcon;
    } else if (pageId == QStringLiteral("__help__")) {
        fallback = QStyle::SP_MessageBoxQuestion;
    } else if (pageId == QStringLiteral("__about__")) {
        fallback = QStyle::SP_MessageBoxInformation;
    }
    card->setIcon(WorkspaceStyle::icon(icon, fallback));
    card->setIconSize(QSize(32, 32));
    card->setMinimumHeight(92);
    card->setMaximumHeight(112);
    layout->addWidget(card, row, column);
    connect(card, &QCommandLinkButton::clicked, this, [this, pageId] {
        if (pageId == QStringLiteral("__about__")) {
            emit aboutRequested();
        } else if (pageId == QStringLiteral("__help__")) {
            emit helpRequested();
        } else {
            APPLICATION->ShowGlobalSettings(this, pageId);
        }
    });
}
