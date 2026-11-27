// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include "AnimatedStackedWidget.h"

#include <QHideEvent>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

AnimatedStackedWidget::AnimatedStackedWidget(QWidget* parent) : QStackedWidget(parent) {}

void AnimatedStackedWidget::setCurrentIndex(int index)
{
    animateTo(index);
}

void AnimatedStackedWidget::setCurrentWidget(QWidget* widget)
{
    animateTo(indexOf(widget));
}

void AnimatedStackedWidget::animateTo(int index)
{
    if (index < 0 || index >= count()) {
        return;
    }

    if (m_animation) {
        if (index == m_targetIndex) {
            return;
        }
        // Start a new action from the page currently in front of the user.
        finishTransition();
    }

    if (index == currentIndex()) {
        return;
    }

    auto* previous = currentWidget();
    auto* incoming = widget(index);
    if (!incoming || incoming == previous) {
        return;
    }

    if (!isVisible() || !previous || previous->size().isEmpty()) {
        QStackedWidget::setCurrentIndex(index);
        return;
    }

    const bool forward = index > currentIndex();
    const int distance = qMax(1, width());
    const int outgoingEnd = forward ? -distance : distance;
    const int incomingStart = -outgoingEnd;

    m_outgoingWidget = previous;
    m_incomingWidget = incoming;
    m_targetIndex = index;
    m_outgoingWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_incomingWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    m_incomingWidget->show();
    m_incomingWidget->raise();
    m_outgoingWidget->setGeometry(rect());
    m_incomingWidget->setGeometry(rect().translated(incomingStart, 0));

    m_animation = new QParallelAnimationGroup(this);
    auto* outgoingSlide = new QPropertyAnimation(m_outgoingWidget, "geometry", m_animation);
    outgoingSlide->setDuration(200);
    outgoingSlide->setStartValue(rect());
    outgoingSlide->setEndValue(rect().translated(outgoingEnd, 0));
    outgoingSlide->setEasingCurve(QEasingCurve::OutCubic);
    auto* incomingSlide = new QPropertyAnimation(m_incomingWidget, "geometry", m_animation);
    incomingSlide->setDuration(200);
    incomingSlide->setStartValue(rect().translated(incomingStart, 0));
    incomingSlide->setEndValue(rect());
    incomingSlide->setEasingCurve(QEasingCurve::OutCubic);
    m_animation->addAnimation(outgoingSlide);
    m_animation->addAnimation(incomingSlide);
    connect(m_animation, &QParallelAnimationGroup::finished, this, [this] {
        finishTransition();
    });
    m_animation->start();
}

void AnimatedStackedWidget::finishTransition()
{
    if (m_animation) {
        disconnect(m_animation, nullptr, this, nullptr);
        m_animation->stop();
        m_animation->deleteLater();
        m_animation = nullptr;
    }

    auto* target = m_targetIndex >= 0 && m_targetIndex < count() ? widget(m_targetIndex) : currentWidget();
    if (m_outgoingWidget && m_outgoingWidget != target) {
        m_outgoingWidget->hide();
        m_outgoingWidget->setGeometry(rect());
        m_outgoingWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
    if (m_incomingWidget && m_incomingWidget == target) {
        m_incomingWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    }
    if (target && m_targetIndex >= 0) {
        QStackedWidget::setCurrentIndex(m_targetIndex);
        target->setGeometry(rect());
        target->show();
        target->raise();
        target->update();
    }
    m_outgoingWidget = nullptr;
    m_incomingWidget = nullptr;
    m_targetIndex = -1;
    update();
}

void AnimatedStackedWidget::resizeEvent(QResizeEvent* event)
{
    finishTransition();
    QStackedWidget::resizeEvent(event);
}

void AnimatedStackedWidget::hideEvent(QHideEvent* event)
{
    finishTransition();
    QStackedWidget::hideEvent(event);
}
