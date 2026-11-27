// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include "ThemePack.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

#include <algorithm>
#include <utility>

#include "archive/ArchiveReader.h"

namespace {
QString manifestString(const QJsonObject& object, const QString& key, bool required, QString& error)
{
    const auto value = object.value(key).toString().trimmed();
    if (required && value.isEmpty()) {
        error = QStringLiteral("theme.json is missing required field '%1'").arg(key);
    }
    return value;
}
}  // namespace

ZipThemePack::ZipThemePack(QString path) : m_path(std::move(path))
{
    m_valid = readManifest(*this) && readReferencedFiles(*this);
}

std::unique_ptr<ZipThemePack> ZipThemePack::load(const QString& path, QString* error)
{
    auto pack = std::make_unique<ZipThemePack>(path);
    if (!pack->isValid()) {
        if (error) {
            *error = pack->errorString();
        }
        return nullptr;
    }
    if (error) {
        error->clear();
    }
    return pack;
}

bool ZipThemePack::readManifest(ZipThemePack& pack)
{
    MMCZip::ArchiveReader archive(pack.m_path);
    auto file = archive.goToFile(QStringLiteral("theme.json"));
    if (!file) {
        pack.m_error = QStringLiteral("ZIP is missing theme.json at its root");
        return false;
    }

    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(file->readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        pack.m_error = QStringLiteral("Invalid theme.json: %1").arg(parseError.errorString());
        return false;
    }

    const auto root = document.object();
    QString error;
    pack.m_descriptor.id = manifestString(root, QStringLiteral("id"), true, error);
    pack.m_descriptor.name = manifestString(root, QStringLiteral("name"), true, error);
    pack.m_descriptor.version = manifestString(root, QStringLiteral("version"), true, error);
    pack.m_descriptor.author = manifestString(root, QStringLiteral("author"), false, error);
    pack.m_descriptor.accent = manifestString(root, QStringLiteral("accent"), false, error);
    pack.m_descriptor.wallpaper = manifestString(root, QStringLiteral("wallpaper"), false, error);
    pack.m_descriptor.styleSheetName = manifestString(root, QStringLiteral("stylesheet"), true, error);
    if (!error.isEmpty()) {
        pack.m_error = error;
        return false;
    }

    static const QRegularExpression validId(QStringLiteral("^[A-Za-z0-9._-]+$"));
    if (!validId.match(pack.m_descriptor.id).hasMatch()) {
        pack.m_error = QStringLiteral("Theme pack id may only contain letters, numbers, '.', '_' and '-'");
        return false;
    }
    return true;
}

bool ZipThemePack::readReferencedFiles(ZipThemePack& pack)
{
    MMCZip::ArchiveReader archive(pack.m_path);
    auto style = archive.goToFile(pack.m_descriptor.styleSheetName);
    if (!style) {
        pack.m_error = QStringLiteral("ZIP is missing stylesheet '%1'").arg(pack.m_descriptor.styleSheetName);
        return false;
    }
    pack.m_styleSheet = style->readAll();
    if (pack.m_styleSheet.isEmpty()) {
        pack.m_error = QStringLiteral("Theme stylesheet is empty");
        return false;
    }

    if (!pack.m_descriptor.wallpaper.isEmpty()) {
        MMCZip::ArchiveReader wallpaperArchive(pack.m_path);
        auto wallpaper = wallpaperArchive.goToFile(pack.m_descriptor.wallpaper);
        if (!wallpaper) {
            pack.m_error = QStringLiteral("ZIP is missing wallpaper '%1'").arg(pack.m_descriptor.wallpaper);
            return false;
        }
        pack.m_wallpaper = wallpaper->readAll();
    }
    return true;
}

ThemePackRegistry::ThemePackRegistry(QDir folder) : m_folder(std::move(folder))
{
    m_folder.mkpath(QStringLiteral("."));
}

void ThemePackRegistry::scan()
{
    m_packs.clear();
    const auto files = m_folder.entryInfoList({ QStringLiteral("*.zip") }, QDir::Files, QDir::Name);
    for (const auto& file : files) {
        QString error;
        auto pack = ZipThemePack::load(file.absoluteFilePath(), &error);
        if (!pack) {
            qWarning() << "Ignoring invalid theme pack" << file.absoluteFilePath() << error;
            continue;
        }
        const auto id = pack->descriptor().id;
        if (std::any_of(m_packs.cbegin(), m_packs.cend(), [&id](const auto& existing) { return existing->descriptor().id == id; })) {
            qWarning() << "Ignoring duplicate theme pack id" << id << file.absoluteFilePath();
            continue;
        }
        m_packs.push_back(std::move(pack));
    }
}

bool ThemePackRegistry::install(const QString& zipPath, QString* error)
{
    auto pack = ZipThemePack::load(zipPath, error);
    if (!pack) {
        return false;
    }
    const QString target = m_folder.filePath(pack->descriptor().id + QStringLiteral(".zip"));
    if (QFileInfo::exists(target)) {
        if (error) {
            *error = QStringLiteral("Theme pack '%1' is already installed").arg(pack->descriptor().id);
        }
        return false;
    }
    if (!QFile::copy(zipPath, target)) {
        if (error) {
            *error = QStringLiteral("Unable to copy theme pack to %1").arg(target);
        }
        return false;
    }
    m_packs.push_back(std::move(pack));
    return true;
}

QList<IThemePack*> ThemePackRegistry::packs() const
{
    QList<IThemePack*> result;
    result.reserve(m_packs.size());
    for (const auto& pack : m_packs) {
        result.append(pack.get());
    }
    return result;
}

std::unique_ptr<IThemePack> ThemePackRegistry::take(const QString& id)
{
    for (auto it = m_packs.begin(); it != m_packs.end(); ++it) {
        if ((*it)->descriptor().id == id) {
            auto pack = std::move(*it);
            m_packs.erase(it);
            return pack;
        }
    }
    return nullptr;
}
