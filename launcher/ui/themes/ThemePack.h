// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#pragma once

#include <QByteArray>
#include <QDir>
#include <QList>
#include <QString>
#include <memory>
#include <vector>

struct ThemePackDescriptor {
    QString id;
    QString name;
    QString version;
    QString author;
    QString accent;
    QString wallpaper;
    QString styleSheetName;
};

class IThemePack {
   public:
    virtual ~IThemePack() = default;
    virtual const ThemePackDescriptor& descriptor() const = 0;
    virtual QString packagePath() const = 0;
    virtual QByteArray styleSheet() const = 0;
    virtual QByteArray wallpaper() const = 0;
};

class ZipThemePack final : public IThemePack {
   public:
    explicit ZipThemePack(QString path);

    const ThemePackDescriptor& descriptor() const override { return m_descriptor; }
    QString packagePath() const override { return m_path; }
    QByteArray styleSheet() const override { return m_styleSheet; }
    QByteArray wallpaper() const override { return m_wallpaper; }
    bool isValid() const { return m_valid; }
    QString errorString() const { return m_error; }

    static std::unique_ptr<ZipThemePack> load(const QString& path, QString* error = nullptr);

   private:
    static bool readManifest(ZipThemePack& pack);
    static bool readReferencedFiles(ZipThemePack& pack);

    QString m_path;
    ThemePackDescriptor m_descriptor;
    QByteArray m_styleSheet;
    QByteArray m_wallpaper;
    QString m_error;
    bool m_valid = false;
};

class ThemePackRegistry {
   public:
    explicit ThemePackRegistry(QDir folder);

    QDir folder() const { return m_folder; }
    void scan();
    bool install(const QString& zipPath, QString* error = nullptr);
    QList<IThemePack*> packs() const;
    std::unique_ptr<IThemePack> take(const QString& id);

   private:
    QDir m_folder;
    std::vector<std::unique_ptr<IThemePack>> m_packs;
};
