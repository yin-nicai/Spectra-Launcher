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

#include "net/Download.h"
#include "plugin/PluginInterfaces.h"

namespace MinecraftDownloads {

Net::Download::Ptr makeCached(const Plugins::DownloadRequest& request,
                              MetaEntryPtr entry,
                              Net::Download::Options options = Net::Download::Option::NoOptions);

Net::Download::Ptr makeFile(const Plugins::DownloadRequest& request,
                            const QString& path,
                            Net::Download::Options options = Net::Download::Option::NoOptions);

}  // namespace MinecraftDownloads
