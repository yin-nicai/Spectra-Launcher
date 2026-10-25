// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#pragma once

#include "plugin/PluginInterfaces.h"
#include "plugin/PluginRegistry.h"

namespace Plugins {

void registerBuiltinResourceProviders(PluginRegistry<IResourceProvider>& registry);

}  // namespace Plugins
