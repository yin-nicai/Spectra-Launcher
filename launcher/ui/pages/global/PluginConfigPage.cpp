// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#include "PluginConfigPage.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "Application.h"
#include "plugin/PluginSystem.h"

QString PluginConfigPage::displayName() const
{
    return tr("Plugins and sources");
}

PluginConfigPage::PluginConfigPage(QWidget* parent) : QWidget(parent)
{
    auto outer = new QVBoxLayout(this);
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    outer->addWidget(scroll);

    auto content = new QWidget(scroll);
    auto layout = new QVBoxLayout(content);
    auto intro = new QLabel(
        tr("Sources are queried in the order shown below. Disabled sources are skipped. Changes apply after you save this page."), content);
    intro->setWordWrap(true);
    layout->addWidget(intro);

    addGroup(
        content, m_metadataSources, tr("Metadata sources"),
        tr("Provides version lists, dependencies, file hashes, and download descriptions. The default Spectra source remains available."),
        QStringLiteral("metadataSources"));
    addGroup(content, m_downloadSources, tr("Minecraft download sources"),
             tr("Controls Minecraft clients, libraries, assets, and asset indexes. Configured domestic mirrors can be prioritized and "
                "distributed across parallel downloads. Modrinth and CurseForge downloads are not affected."),
             QStringLiteral("downloadSources"));
    addGroup(content, m_directoryStrategies, tr("Directory strategies"),
             tr("Resolves instance, game, asset, log, and crash-report paths. A damaged configuration falls back to Spectra directories."),
             QStringLiteral("directoryStrategies"));
    addGroup(content, m_resourceProviders, tr("Resource platforms"),
             tr("Controls which modpack and project catalogs appear in new-instance and download pages."),
             QStringLiteral("resourceProviders"));

    auto advancedBox = new QGroupBox(tr("Advanced mirror configuration"), content);
    advancedBox->setCheckable(true);
    auto advancedContent = new QWidget(advancedBox);
    auto advancedOuter = new QVBoxLayout(advancedBox);
    advancedOuter->addWidget(advancedContent);
    auto advancedLayout = new QFormLayout(advancedContent);
    advancedLayout->setContentsMargins(0, 0, 0, 0);
    m_metadataUrls = new QPlainTextEdit(advancedContent);
    m_metadataUrls->setPlaceholderText(tr("One metadata base URL per line"));
    m_metadataUrls->setMaximumHeight(90);
    m_foreignMirrors = new QPlainTextEdit(advancedContent);
    m_foreignMirrors->setPlaceholderText(tr("One URL or template per line, for example https://mirror.example/{path}"));
    m_foreignMirrors->setMaximumHeight(90);
    m_domesticMirrors = new QPlainTextEdit(advancedContent);
    m_domesticMirrors->setPlaceholderText(tr("One URL or template per line, for example https://mirror.example/{path}"));
    m_domesticMirrors->setMaximumHeight(90);
    m_preferDomestic = new QCheckBox(tr("国内镜像优先"), advancedContent);
    m_preferDomestic->setToolTip(tr("已配置国内镜像时，Minecraft 文件会优先从国内镜像下载，并在多个镜像之间轮转分摊连接。"));
    m_domesticConcurrency = new QSpinBox(advancedContent);
    m_domesticConcurrency->setRange(2, 32);
    m_domesticConcurrency->setValue(12);
    m_domesticConcurrency->setToolTip(tr("启用国内镜像优先时，Minecraft 文件下载任务至少使用此并发数；不会影响模组和整合包下载。"));
    advancedLayout->addRow(tr("Additional metadata URLs"), m_metadataUrls);
    advancedLayout->addRow(tr("Foreign mirror URLs"), m_foreignMirrors);
    advancedLayout->addRow(tr("Domestic mirror URLs"), m_domesticMirrors);
    advancedLayout->addRow(QString(), m_preferDomestic);
    advancedLayout->addRow(tr("国内镜像并发数"), m_domesticConcurrency);
    advancedBox->setChecked(false);
    advancedContent->setVisible(false);
    connect(advancedBox, &QGroupBox::toggled, advancedContent, &QWidget::setVisible);
    layout->addWidget(advancedBox);
    layout->addStretch(1);
    scroll->setWidget(content);

    populateGroup(m_metadataSources, APPLICATION->plugins()->metadataSources());
    populateGroup(m_downloadSources, APPLICATION->plugins()->downloadSources());
    populateGroup(m_directoryStrategies, APPLICATION->plugins()->directoryStrategies());
    populateGroup(m_resourceProviders, APPLICATION->plugins()->resourceProviders());

    m_metadataUrls->setPlainText(m_metadataSources.options.value(QStringLiteral("configured-http-meta"))
                                     .value(QStringLiteral("urls"))
                                     .toStringList()
                                     .join(QLatin1Char('\n')));
    const auto mirrorOptions = m_downloadSources.options.value(QStringLiteral("priority-mirrors"));
    m_foreignMirrors->setPlainText(mirrorOptions.value(QStringLiteral("foreignMirrors")).toStringList().join(QLatin1Char('\n')));
    m_domesticMirrors->setPlainText(mirrorOptions.value(QStringLiteral("domesticMirrors")).toStringList().join(QLatin1Char('\n')));
    m_preferDomestic->setChecked(mirrorOptions.value(QStringLiteral("preferDomestic"), true).toBool());
    m_domesticConcurrency->setValue(qBound(2, mirrorOptions.value(QStringLiteral("domesticConcurrency"), 12).toInt(), 32));
    m_domesticConcurrency->setEnabled(m_preferDomestic->isChecked());
    connect(m_preferDomestic, &QCheckBox::toggled, m_domesticConcurrency, &QWidget::setEnabled);
}

void PluginConfigPage::addGroup(QWidget* parent, Group& group, const QString& title, const QString& description, const QString& jsonKey)
{
    group.jsonKey = jsonKey;
    auto box = new QGroupBox(title, parent);
    auto layout = new QVBoxLayout(box);
    auto descriptionLabel = new QLabel(description, box);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setToolTip(description);
    layout->addWidget(descriptionLabel);

    group.tree = new QTreeWidget(box);
    group.tree->setColumnCount(3);
    group.tree->setHeaderLabels({ tr("Enabled"), tr("Source"), tr("Order") });
    group.tree->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    group.tree->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    group.tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    group.tree->setRootIsDecorated(false);
    group.tree->setUniformRowHeights(true);
    group.tree->setMinimumHeight(120);
    layout->addWidget(group.tree);

    auto buttons = new QHBoxLayout();
    group.up = new QPushButton(tr("Move up"), box);
    group.down = new QPushButton(tr("Move down"), box);
    group.summary = new QLabel(box);
    group.summary->setEnabled(false);
    buttons->addWidget(group.up);
    buttons->addWidget(group.down);
    buttons->addStretch(1);
    buttons->addWidget(group.summary);
    layout->addLayout(buttons);

    connect(group.up, &QPushButton::clicked, this, [this, tree = group.tree] { moveItem(tree, -1); });
    connect(group.down, &QPushButton::clicked, this, [this, tree = group.tree] { moveItem(tree, 1); });
    parent->layout()->addWidget(box);
}

template <typename Interface>
void PluginConfigPage::populateGroup(Group& group, Plugins::PluginRegistry<Interface>& registry)
{
    QHash<QString, Plugins::PluginRegistration<Interface>> registrations;
    for (const auto& registration : registry.registrations()) {
        registrations.insert(registration.id, registration);
    }

    const auto states = registry.currentStates();
    for (const auto& state : states) {
        const auto registration = registrations.value(state.id);
        auto item = new QTreeWidgetItem(group.tree);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, state.enabled ? Qt::Checked : Qt::Unchecked);
        item->setText(1, registration.displayName.isEmpty() ? state.id : registration.displayName);
        item->setData(1, Qt::UserRole, state.id);
        item->setToolTip(1, registration.description);
        group.options.insert(state.id, state.options);
    }
    for (int i = 0; i < group.tree->topLevelItemCount(); ++i) {
        group.tree->topLevelItem(i)->setText(2, QString::number(i + 1));
    }
    group.summary->setText(tr("%n source(s)", nullptr, group.tree->topLevelItemCount()));
}

void PluginConfigPage::moveItem(QTreeWidget* tree, int offset)
{
    const auto current = tree->currentItem();
    if (!current) {
        return;
    }
    const int row = tree->indexOfTopLevelItem(current);
    const int target = row + offset;
    if (target < 0 || target >= tree->topLevelItemCount()) {
        return;
    }
    tree->takeTopLevelItem(row);
    tree->insertTopLevelItem(target, current);
    tree->setCurrentItem(current);
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        tree->topLevelItem(i)->setText(2, QString::number(i + 1));
    }
}

QStringList PluginConfigPage::lines(QPlainTextEdit* editor) const
{
    QStringList result;
    for (const auto& line : editor->toPlainText().split(QLatin1Char('\n'))) {
        const auto value = line.trimmed();
        if (!value.isEmpty() && !result.contains(value)) {
            result.append(value);
        }
    }
    return result;
}

QJsonArray PluginConfigPage::groupJson(Group& group)
{
    QJsonArray result;
    for (int i = 0; i < group.tree->topLevelItemCount(); ++i) {
        const auto* item = group.tree->topLevelItem(i);
        const auto id = item->data(1, Qt::UserRole).toString();
        auto options = group.options.value(id);
        if (id == QStringLiteral("configured-http-meta")) {
            options.insert(QStringLiteral("urls"), lines(m_metadataUrls));
        } else if (id == QStringLiteral("priority-mirrors")) {
            options.insert(QStringLiteral("foreignMirrors"), lines(m_foreignMirrors));
            options.insert(QStringLiteral("domesticMirrors"), lines(m_domesticMirrors));
            options.insert(QStringLiteral("preferDomestic"), m_preferDomestic->isChecked());
            options.insert(QStringLiteral("domesticConcurrency"), m_domesticConcurrency->value());
        }
        result.append(QJsonObject{
            { QStringLiteral("id"), id },
            { QStringLiteral("enabled"), item->checkState(0) == Qt::Checked },
            { QStringLiteral("order"), i * 10 },
            { QStringLiteral("options"), QJsonObject::fromVariantMap(options) },
        });
    }
    return result;
}

bool PluginConfigPage::apply()
{
    auto configuration = APPLICATION->plugins()->configuration();
    configuration.insert(m_metadataSources.jsonKey, groupJson(m_metadataSources));
    configuration.insert(m_downloadSources.jsonKey, groupJson(m_downloadSources));
    configuration.insert(m_directoryStrategies.jsonKey, groupJson(m_directoryStrategies));
    configuration.insert(m_resourceProviders.jsonKey, groupJson(m_resourceProviders));
    APPLICATION->plugins()->applyConfiguration(configuration, true);
    return true;
}
