// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "DownloadsPage.h"

#include <QCommandLinkButton>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>

#include "Application.h"
#include "plugin/PluginSystem.h"
#include "ui/WorkspaceStyle.h"

DownloadsPage::DownloadsPage(QWidget* parent) : QWidget(parent)
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
    layout->setSpacing(10);

    m_title = new QLabel(content);
    QFont titleFont = m_title->font();
    titleFont.setPointSize(titleFont.pointSize() + 7);
    titleFont.setWeight(QFont::DemiBold);
    m_title->setFont(titleFont);
    layout->addWidget(m_title);

    m_subtitle = new QLabel(content);
    m_subtitle->setWordWrap(true);
    m_subtitle->setEnabled(false);
    layout->addWidget(m_subtitle);
    layout->addSpacing(6);

    auto policy = new QFrame(content);
    policy->setObjectName(QStringLiteral("downloadPolicyNote"));
    policy->setFrameShape(QFrame::StyledPanel);
    auto policyLayout = new QHBoxLayout(policy);
    policyLayout->setContentsMargins(14, 12, 14, 12);
    m_policyNote = new QLabel(policy);
    m_policyNote->setWordWrap(true);
    policyLayout->addWidget(m_policyNote);
    layout->addWidget(policy);
    layout->addSpacing(4);

    m_cards = new QGridLayout();
    m_cards->setHorizontalSpacing(12);
    m_cards->setVerticalSpacing(12);
    m_cards->setColumnStretch(0, 1);
    m_cards->setColumnStretch(1, 1);
    m_cards->setColumnStretch(2, 1);
    layout->addLayout(m_cards);

    m_empty = new QLabel(content);
    m_empty->setWordWrap(true);
    m_empty->setAlignment(Qt::AlignCenter);
    m_empty->setMinimumHeight(160);
    layout->addWidget(m_empty);
    layout->addStretch(1);
    scroll->setWidget(content);

    connect(APPLICATION->plugins(), &Plugins::PluginSystem::configurationChanged, this, &DownloadsPage::rebuildProviderCards);
    retranslate();
}

void DownloadsPage::clearProviderCards()
{
    while (auto* item = m_cards->takeAt(0)) {
        if (auto* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
}

void DownloadsPage::rebuildProviderCards()
{
    clearProviderCards();

    QHash<QString, Plugins::PluginRegistration<Plugins::IResourceProvider>> registrations;
    for (const auto& registration : APPLICATION->plugins()->resourceProviders().registrations()) {
        registrations.insert(registration.id, registration);
    }

    Plugins::ResourcePageContext context;
    context.surface = Plugins::ResourceSurface::InstanceSource;
    int cardCount = 0;
    for (const auto* provider : APPLICATION->plugins()->resourceProviders().enabledInOrder()) {
        const auto registration = registrations.value(provider->id());
        const auto specs = provider->pages(context);
        for (const auto& spec : specs) {
            const auto title = spec.title.isEmpty() ? registration.displayName : spec.title;
            auto card = new QCommandLinkButton(title, spec.description, this);
            card->setIcon(WorkspaceStyle::icon(spec.iconName, QStyle::SP_FileDialogListView));
            card->setIconSize(QSize(36, 36));
            card->setMinimumHeight(112);
            card->setMaximumHeight(132);
            card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            card->setToolTip(registration.description);
            m_cards->addWidget(card, cardCount / 3, cardCount % 3);
            connect(card, &QCommandLinkButton::clicked, this, [this, pageId = spec.id] { emit providerSelected(pageId); });
            ++cardCount;
        }
    }
    m_empty->setVisible(cardCount == 0);
}

void DownloadsPage::retranslate()
{
    m_title->setText(tr("Downloads"));
    m_subtitle->setText(tr("Choose a resource platform. Disabled providers are hidden from this page."));
    m_policyNote->setText(
        tr("Minecraft game files are fetched only from Mojang or from mirrors that you explicitly configure. "
           "Mirror download failures fall back in the configured source order."));
    m_empty->setText(tr("No instance sources are enabled.\nEnable a platform in Settings > Plugins and sources."));
    rebuildProviderCards();
}
