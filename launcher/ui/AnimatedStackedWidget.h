// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#pragma once

#include <QPointer>
#include <QStackedWidget>

class QHideEvent;
class QParallelAnimationGroup;

class AnimatedStackedWidget final : public QStackedWidget {
    Q_OBJECT

   public:
    explicit AnimatedStackedWidget(QWidget* parent = nullptr);

    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget* widget);

   protected:
    void resizeEvent(QResizeEvent* event) override;
    void hideEvent(QHideEvent* event) override;

   private:
    void animateTo(int index);
    void finishTransition();

    QPointer<QWidget> m_outgoingWidget;
    QPointer<QWidget> m_incomingWidget;
    QPointer<QParallelAnimationGroup> m_animation;
    int m_targetIndex = -1;
};
