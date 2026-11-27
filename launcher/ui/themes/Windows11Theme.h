// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#pragma once

#include "FusionTheme.h"

class Windows11Theme final : public FusionTheme {
   public:
    ~Windows11Theme() override = default;

    QString id() override;
    QString name() override;
    QString tooltip() override;
    QString qtTheme() override;
    bool hasStyleSheet() override;
    QString appStyleSheet() override;
    QPalette colorScheme() override;
    double fadeAmount() override;
    QColor fadeColor() override;
};
