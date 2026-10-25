// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#include "DirectoryPage.h"

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "Application.h"
#include "DesktopServices.h"
#include "plugin/PluginInterfaces.h"
#include "settings/SettingsObject.h"

DirectoryPage::DirectoryPage(QWidget* parent) : QWidget(parent)
{
    auto layout = new QVBoxLayout(this);
    m_intro = new QLabel(this);
    m_intro->setWordWrap(true);
    layout->addWidget(m_intro);

    auto group = new QGroupBox(this);
    auto form = new QFormLayout(group);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    const auto addFormRow = [this, form, group](const QString& settingId, const QString& defaultValue, const QString& title,
                                                const QString& hint, const QString& openTitle) {
        PathRow row{ settingId, defaultValue, title, hint, openTitle };
        auto field = new QWidget(this);
        auto fieldLayout = new QHBoxLayout(field);
        fieldLayout->setContentsMargins(0, 0, 0, 0);

        row.edit = new QLineEdit(field);
        row.browse = new QPushButton(tr("Browse..."), field);
        row.open = new QPushButton(openTitle, field);
        row.reset = new QPushButton(tr("Default"), field);
        fieldLayout->addWidget(row.edit, 1);
        fieldLayout->addWidget(row.browse);
        fieldLayout->addWidget(row.open);
        fieldLayout->addWidget(row.reset);

        row.titleLabel = new QLabel(title, group);
        form->addRow(row.titleLabel, field);
        row.hintLabel = new QLabel(hint, group);
        row.hintLabel->setWordWrap(true);
        row.hintLabel->setEnabled(false);
        form->addRow(QString(), row.hintLabel);

        const auto index = m_rows.size();
        connect(row.browse, &QPushButton::clicked, this, [this, index] { browse(m_rows[index]); });
        connect(row.open, &QPushButton::clicked, this, [this, index] { openDirectory(m_rows[index]); });
        connect(row.reset, &QPushButton::clicked, this, [this, index] { resetToDefault(m_rows[index]); });
        m_rows.append(row);
    };

    addFormRow(QStringLiteral("InstanceDir"), QStringLiteral("instances"), tr("Instance root"),
               tr("The primary root for new instances. Old roots remain available after you change this path."), tr("Open instances"));
    addFormRow(QStringLiteral("CustomGameDirRoot"), {}, tr("Game directory root"),
               tr("New instances use <root>/<instance ID>. Existing game directories are kept in place."), tr("Open game root"));
    addFormRow(QStringLiteral("CustomAssetsDirRoot"), {}, tr("Asset cache root"),
               tr("Stores indexes, objects, and virtual assets. Existing populated caches continue to be used."), tr("Open assets"));
    addFormRow(QStringLiteral("CustomLogsDirRoot"), {}, tr("Log search root"),
               tr("New or absent instance log folders use <root>/<instance ID>; existing logs remain discoverable."), tr("Open logs"));
    addFormRow(QStringLiteral("CustomCrashReportsDirRoot"), {}, tr("Crash report root"),
               tr("New or absent crash-report folders use <root>/<instance ID>; existing reports remain discoverable."),
               tr("Open crash reports"));

    layout->addWidget(group);
    layout->addStretch(1);

    loadSettings();
    retranslate();
}

QString DirectoryPage::displayName() const
{
    return tr("Directories");
}

void DirectoryPage::loadSettings()
{
    auto* settings = APPLICATION->settings();
    for (auto& row : m_rows) {
        row.edit->setText(settings->get(row.settingId).toString());
    }
}

void DirectoryPage::browse(PathRow& row)
{
    auto initial = row.edit->text();
    if (initial.isEmpty()) {
        initial = APPLICATION->dataRoot();
    }
    const auto directory = QFileDialog::getExistingDirectory(this, row.title, initial);
    if (!directory.isEmpty()) {
        row.edit->setText(QDir::cleanPath(QDir::fromNativeSeparators(directory)));
    }
}

void DirectoryPage::openDirectory(PathRow& row)
{
    auto path = row.edit->text().trimmed();
    if (path.isEmpty()) {
        if (row.settingId == QStringLiteral("InstanceDir")) {
            path = APPLICATION->resolveDirectory(Plugins::DirectoryRole::Instances);
        } else if (row.settingId == QStringLiteral("CustomAssetsDirRoot")) {
            path = APPLICATION->resolveDirectory(Plugins::DirectoryRole::Assets);
        } else {
            QMessageBox::information(
                this, tr("Directory not configured"),
                tr("Configure a root directory first. Per-instance folders become available after selecting an instance."));
            return;
        }
    } else if (!QDir::isAbsolutePath(path)) {
        path = QDir(APPLICATION->dataRoot()).absoluteFilePath(path);
    }

    QDir directory(path);
    if (!directory.exists() && !directory.mkpath(QStringLiteral("."))) {
        QMessageBox::warning(this, tr("Unable to open directory"), tr("The directory could not be created:\n%1").arg(path));
        return;
    }
    DesktopServices::openPath(directory.absolutePath(), true);
}

void DirectoryPage::resetToDefault(PathRow& row)
{
    row.edit->setText(row.defaultValue);
}

bool DirectoryPage::apply()
{
    auto* settings = APPLICATION->settings();
    for (const auto& row : m_rows) {
        auto value = QDir::cleanPath(QDir::fromNativeSeparators(row.edit->text().trimmed()));
        if (value == QLatin1String(".")) {
            value.clear();
        }
        if (row.settingId == QStringLiteral("InstanceDir") && value.isEmpty()) {
            value = row.defaultValue;
        }
        if (value.isEmpty()) {
            settings->reset(row.settingId);
        } else {
            settings->set(row.settingId, value);
        }
    }
    return true;
}

void DirectoryPage::retranslate()
{
    m_intro->setText(
        tr("These paths are resolved through the active directory strategy. Changing a root never deletes or moves existing files."));
    for (auto& row : m_rows) {
        row.titleLabel->setText(row.title);
        row.hintLabel->setText(row.hint);
        row.open->setText(row.openTitle);
        row.browse->setText(tr("Browse..."));
        row.reset->setText(tr("Default"));
    }
}
