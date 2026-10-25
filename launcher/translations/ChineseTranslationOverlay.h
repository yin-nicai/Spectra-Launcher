// SPDX-License-Identifier: GPL-3.0-only
/*
 * Prism Launcher - Minecraft Launcher
 * Copyright (C) 2026 Prism Launcher Contributors
 */

#pragma once

#include <QTranslator>

class ChineseTranslationOverlay final : public QTranslator {
   public:
    using QTranslator::QTranslator;

    QString translate(const char* context, const char* sourceText, const char* disambiguation, int n) const override;
};
