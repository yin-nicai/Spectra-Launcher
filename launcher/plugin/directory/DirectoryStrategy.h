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

#include "plugin/PluginInterfaces.h"

namespace Plugins {

class PrismDirectoryStrategy final : public IDirectoryStrategy {
   public:
    QString id() const override { return QStringLiteral("prism-directories"); }
    QString resolve(DirectoryRole role, const DirectoryContext& context) const override;
};

}  // namespace Plugins
