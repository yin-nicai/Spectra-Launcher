// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include "Windows11Theme.h"

#include <QObject>
#include <QStyleFactory>

QString Windows11Theme::id()
{
    return QStringLiteral("windows11");
}

QString Windows11Theme::name()
{
    return QObject::tr("Windows 11");
}

QString Windows11Theme::tooltip()
{
    return QObject::tr("默认深色圆角主题。");
}

QString Windows11Theme::qtTheme()
{
    const auto styles = QStyleFactory::keys();
    return styles.contains(QStringLiteral("windows11"), Qt::CaseInsensitive) ? QStringLiteral("windows11") : QStringLiteral("Fusion");
}

QPalette Windows11Theme::colorScheme()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(24, 26, 30));
    palette.setColor(QPalette::WindowText, QColor(244, 246, 249));
    palette.setColor(QPalette::Base, QColor(17, 19, 23));
    palette.setColor(QPalette::AlternateBase, QColor(31, 34, 40));
    palette.setColor(QPalette::ToolTipBase, QColor(244, 246, 249));
    palette.setColor(QPalette::ToolTipText, QColor(17, 19, 23));
    palette.setColor(QPalette::Text, QColor(244, 246, 249));
    palette.setColor(QPalette::Button, QColor(38, 41, 47));
    palette.setColor(QPalette::ButtonText, QColor(244, 246, 249));
    palette.setColor(QPalette::BrightText, QColor(255, 99, 110));
    palette.setColor(QPalette::Link, QColor(100, 181, 246));
    palette.setColor(QPalette::Highlight, QColor(0, 103, 192));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::PlaceholderText, QColor(150, 157, 168));
    palette.setColor(QPalette::Light, QColor(65, 70, 80));
    palette.setColor(QPalette::Midlight, QColor(50, 54, 63));
    palette.setColor(QPalette::Mid, QColor(68, 73, 84));
    palette.setColor(QPalette::Dark, QColor(10, 12, 15));
    palette.setColor(QPalette::Shadow, QColor(5, 6, 8));
    return fadeInactive(palette, fadeAmount(), fadeColor());
}

double Windows11Theme::fadeAmount()
{
    return 0.5;
}

QColor Windows11Theme::fadeColor()
{
    return QColor(24, 26, 30);
}

bool Windows11Theme::hasStyleSheet()
{
    return true;
}

QString Windows11Theme::appStyleSheet()
{
    return {};
}
