// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#pragma once

#include "FusionTheme.h"

class IThemePack;

class ThemePackAdapter final : public FusionTheme {
   public:
    explicit ThemePackAdapter(const IThemePack* pack);

    QString id() override;
    QString name() override;
    QString tooltip() override;
    QString qtTheme() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;

   private:
    const IThemePack* m_pack;
};
