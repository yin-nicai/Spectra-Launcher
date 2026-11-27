// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include "ThemePackAdapter.h"

#include <QObject>
#include <QStringList>

#include "ThemePack.h"
#include "Windows11Theme.h"

ThemePackAdapter::ThemePackAdapter(const IThemePack* pack) : m_pack(pack) {}

QString ThemePackAdapter::id()
{
    return QStringLiteral("theme-pack/%1").arg(m_pack->descriptor().id);
}

QString ThemePackAdapter::name()
{
    return m_pack->descriptor().name;
}

QString ThemePackAdapter::tooltip()
{
    const auto& descriptor = m_pack->descriptor();
    QStringList details;
    if (!descriptor.author.isEmpty()) {
        details.append(QObject::tr("作者：%1").arg(descriptor.author));
    }
    if (!descriptor.version.isEmpty()) {
        details.append(QObject::tr("版本：%1").arg(descriptor.version));
    }
    details.append(QObject::tr("ZIP 主题包"));
    return details.join(QStringLiteral(" · "));
}

QString ThemePackAdapter::qtTheme()
{
    return Windows11Theme().qtTheme();
}

bool ThemePackAdapter::hasStyleSheet()
{
    return true;
}

QString ThemePackAdapter::appStyleSheet()
{
    return QString::fromUtf8(m_pack->styleSheet());
}

QPalette ThemePackAdapter::colorScheme()
{
    static Windows11Theme fallback;
    auto palette = fallback.colorScheme();
    const QColor accent(m_pack->descriptor().accent);
    if (accent.isValid()) {
        palette.setColor(QPalette::Highlight, accent);
    }
    return palette;
}

double ThemePackAdapter::fadeAmount()
{
    static Windows11Theme fallback;
    return fallback.fadeAmount();
}

QColor ThemePackAdapter::fadeColor()
{
    static Windows11Theme fallback;
    return fallback.fadeColor();
}
