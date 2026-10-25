// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 */

#include "ResourceProviders.h"

#include <QCoreApplication>
#include <QHash>
#include <functional>

#include "Application.h"
#include "modplatform/flame/FlameAPI.h"
#include "modplatform/modrinth/ModrinthAPI.h"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "ui/dialogs/NewInstanceDialog.h"
#include "ui/dialogs/ResourceDownloadDialog.h"
#include "ui/pages/modplatform/CustomPage.h"
#include "ui/pages/modplatform/ImportPage.h"
#include "ui/pages/modplatform/atlauncher/AtlPage.h"
#include "ui/pages/modplatform/flame/FlamePage.h"
#include "ui/pages/modplatform/flame/FlameResourcePages.h"
#include "ui/pages/modplatform/ftb/FtbPage.h"
#include "ui/pages/modplatform/import_ftb/ImportFTBPage.h"
#include "ui/pages/modplatform/legacy_ftb/Page.h"
#include "ui/pages/modplatform/modrinth/ModrinthPage.h"
#include "ui/pages/modplatform/modrinth/ModrinthResourcePages.h"
#include "ui/pages/modplatform/technic/TechnicPage.h"

namespace {

using namespace ResourceDownload;

class BuiltinResourceProvider final : public Plugins::IResourceProvider {
   public:
    using Factory = std::function<BasePage*(const Plugins::ResourcePageContext&, QWidget*)>;
    using Availability = std::function<bool(const Plugins::ResourcePageContext&)>;

    struct Surface {
        Factory factory;
        Availability availability;
    };

    BuiltinResourceProvider(QString id, QString description, QString icon, QHash<int, Surface> surfaces)
        : m_id(std::move(id))
        , m_description(std::move(description))
        , m_icon(std::move(icon))
        , m_surfaces(std::move(surfaces))
    {}

    QString id() const override { return m_id; }

    QList<Plugins::ResourcePageSpec> pages(const Plugins::ResourcePageContext& context) const override
    {
        const auto iter = m_surfaces.find(static_cast<int>(context.surface));
        if (iter == m_surfaces.end() || !iter->factory) {
            return {};
        }
        if (iter->availability && !iter->availability(context)) {
            return {};
        }

        auto factory = iter->factory;
        return { {
            m_id,
            {},
            m_description,
            m_icon,
            [factory = std::move(factory), context](QWidget* parent) { return factory(context, parent); },
        } };
    }

   private:
    QString m_id;
    QString m_description;
    QString m_icon;
    QHash<int, Surface> m_surfaces;
};

QString tr(const char* text)
{
    return QCoreApplication::translate("ResourceProviders", text);
}

bool supportsFlame()
{
    return (APPLICATION->capabilities() & Application::SupportsFlame) != 0;
}

auto minecraftInstance(const Plugins::ResourcePageContext& context)
{
    return dynamic_cast<MinecraftInstance*>(context.instance);
}

BuiltinResourceProvider::Availability instanceLoadersAvailable(const QString& providerId)
{
    return [providerId](const Plugins::ResourcePageContext& context) {
        if (context.surface != Plugins::ResourceSurface::Mod) {
            return true;
        }
        const auto* instance = minecraftInstance(context);
        if (!instance) {
            return false;
        }
        const auto loaders = instance->getPackProfile()->getSupportedModLoaders().value_or(ModPlatform::ModLoaderTypes(0));
        return providerId == QStringLiteral("modrinth") ? ModrinthAPI::validateModLoaders(loaders)
                                                       : FlameAPI::validateModLoaders(loaders);
    };
}

BuiltinResourceProvider::Availability flameAvailable()
{
    return [](const Plugins::ResourcePageContext&) { return supportsFlame(); };
}

BuiltinResourceProvider::Availability flameModAvailable()
{
    auto loaders = instanceLoadersAvailable(QStringLiteral("flame"));
    return [loaders = std::move(loaders)](const Plugins::ResourcePageContext& context) {
        return supportsFlame() && loaders(context);
    };
}

BuiltinResourceProvider::Surface makeSurface(BuiltinResourceProvider::Factory factory,
                                             BuiltinResourceProvider::Availability availability = {})
{
    return { std::move(factory), std::move(availability) };
}

Plugins::PluginRegistration<Plugins::IResourceProvider> makeRegistration(
    const QString& id,
    const QString& name,
    const QString& description,
    const QString& icon,
    QHash<int, BuiltinResourceProvider::Surface> surfaces)
{
    return {
        id,
        name,
        description,
        true,
        [id, description, icon, surfaces = std::move(surfaces)](const QVariantMap&) {
            return std::make_unique<BuiltinResourceProvider>(id, description, icon, surfaces);
        },
    };
}

QHash<int, BuiltinResourceProvider::Surface> singleInstanceSource(
    BuiltinResourceProvider::Factory factory,
    BuiltinResourceProvider::Availability availability = {})
{
    return { { static_cast<int>(Plugins::ResourceSurface::InstanceSource), makeSurface(std::move(factory), std::move(availability)) } };
}

}  // namespace

namespace Plugins {

void registerBuiltinResourceProviders(PluginRegistry<IResourceProvider>& registry)
{
    const auto add = [&registry](const QString& id,
                                 const QString& name,
                                 const QString& description,
                                 const QString& icon,
                                 QHash<int, BuiltinResourceProvider::Surface> surfaces) {
        registry.registerFactory(makeRegistration(id, name, description, icon, std::move(surfaces)));
    };

    add(QStringLiteral("vanilla"),
        tr("Vanilla"),
        tr("Create a Minecraft instance from an official version."),
        QStringLiteral("minecraft"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new CustomPage(qobject_cast<NewInstanceDialog*>(parent));
        }));
    add(QStringLiteral("import"),
        tr("Import"),
        tr("Import an existing instance archive or local folder."),
        QStringLiteral("archive-insert"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new ImportPage(qobject_cast<NewInstanceDialog*>(parent));
        }));
    add(QStringLiteral("atl"),
        tr("ATLauncher"),
        tr("Browse modpacks published on ATLauncher."),
        QStringLiteral("atlauncher"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new AtlPage(qobject_cast<NewInstanceDialog*>(parent));
        }));
    add(QStringLiteral("flame"),
        tr("CurseForge"),
        tr("Browse CurseForge modpacks and projects when the required API key is available."),
        QStringLiteral("curseforge"),
        QHash<int, BuiltinResourceProvider::Surface>{
            { static_cast<int>(ResourceSurface::InstanceSource),
              makeSurface(
                  [](const ResourcePageContext&, QWidget* parent) {
                      return new FlamePage(qobject_cast<NewInstanceDialog*>(parent));
                  },
                  [](const ResourcePageContext&) { return supportsFlame(); }) },
            { static_cast<int>(ResourceSurface::Mod),
              makeSurface(
                  [](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                      if (!context.instance) {
                          return nullptr;
                      }
                      return FlameModPage::create(qobject_cast<ModDownloadDialog*>(parent), *context.instance);
                  },
                  flameModAvailable()) },
            { static_cast<int>(ResourceSurface::ResourcePack),
              makeSurface(
                  [](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                      return context.instance ? FlameResourcePackPage::create(qobject_cast<ResourcePackDownloadDialog*>(parent),
                                                                             *context.instance)
                                              : nullptr;
                  },
                  flameAvailable()) },
            { static_cast<int>(ResourceSurface::TexturePack),
              makeSurface(
                  [](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                      return context.instance ? FlameTexturePackPage::create(qobject_cast<TexturePackDownloadDialog*>(parent),
                                                                            *context.instance)
                                              : nullptr;
                  },
                  flameAvailable()) },
            { static_cast<int>(ResourceSurface::ShaderPack),
              makeSurface(
                  [](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                      return context.instance ? FlameShaderPackPage::create(qobject_cast<ShaderPackDownloadDialog*>(parent),
                                                                           *context.instance)
                                              : nullptr;
                  },
                  flameAvailable()) },
            { static_cast<int>(ResourceSurface::DataPack),
              makeSurface(
                  [](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                      return context.instance ? FlameDataPackPage::create(qobject_cast<DataPackDownloadDialog*>(parent),
                                                                         *context.instance)
                                              : nullptr;
                  },
                  flameAvailable()) },
        });
    add(QStringLiteral("ftb"),
        tr("FTB"),
        tr("Browse Feed The Beast modpacks."),
        QStringLiteral("ftb"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new FtbPage(qobject_cast<NewInstanceDialog*>(parent));
        }));
    add(QStringLiteral("legacy_ftb"),
        tr("FTB Legacy"),
        tr("Browse older Feed The Beast modpacks."),
        QStringLiteral("ftb"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new LegacyFTB::Page(qobject_cast<NewInstanceDialog*>(parent));
        }));
    add(QStringLiteral("import_ftb"),
        tr("FTB App import"),
        tr("Import modpacks from the FTB App installation."),
        QStringLiteral("ftb"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new FTBImportAPP::ImportFTBPage(qobject_cast<NewInstanceDialog*>(parent));
        }));
    add(QStringLiteral("modrinth"),
        tr("Modrinth"),
        tr("Browse modpacks and projects published on Modrinth."),
        QStringLiteral("modrinth"),
        QHash<int, BuiltinResourceProvider::Surface>{
            { static_cast<int>(ResourceSurface::InstanceSource),
              makeSurface([](const ResourcePageContext&, QWidget* parent) {
                  return new ModrinthPage(qobject_cast<NewInstanceDialog*>(parent));
              }) },
            { static_cast<int>(ResourceSurface::Mod),
              makeSurface(
                  [](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                      return context.instance ? ModrinthModPage::create(qobject_cast<ModDownloadDialog*>(parent), *context.instance)
                                              : nullptr;
                  },
                  instanceLoadersAvailable(QStringLiteral("modrinth"))) },
            { static_cast<int>(ResourceSurface::ResourcePack),
              makeSurface([](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                  return context.instance ? ModrinthResourcePackPage::create(qobject_cast<ResourcePackDownloadDialog*>(parent),
                                                                             *context.instance)
                                          : nullptr;
              }) },
            { static_cast<int>(ResourceSurface::TexturePack),
              makeSurface([](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                  return context.instance ? ModrinthTexturePackPage::create(qobject_cast<TexturePackDownloadDialog*>(parent),
                                                                            *context.instance)
                                          : nullptr;
              }) },
            { static_cast<int>(ResourceSurface::ShaderPack),
              makeSurface([](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                  return context.instance ? ModrinthShaderPackPage::create(qobject_cast<ShaderPackDownloadDialog*>(parent),
                                                                           *context.instance)
                                          : nullptr;
              }) },
            { static_cast<int>(ResourceSurface::DataPack),
              makeSurface([](const ResourcePageContext& context, QWidget* parent) -> BasePage* {
                  return context.instance ? ModrinthDataPackPage::create(qobject_cast<DataPackDownloadDialog*>(parent),
                                                                         *context.instance)
                                          : nullptr;
              }) },
        });
    add(QStringLiteral("technic"),
        tr("Technic"),
        tr("Browse Technic modpacks."),
        QStringLiteral("technic"),
        singleInstanceSource([](const ResourcePageContext&, QWidget* parent) {
            return new TechnicPage(qobject_cast<NewInstanceDialog*>(parent));
        }));
}

}  // namespace Plugins
