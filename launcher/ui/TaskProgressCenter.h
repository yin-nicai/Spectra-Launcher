// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#pragma once

#include <QList>
#include <QObject>
#include <QPointer>

class Task;

class TaskProgressCenter final : public QObject {
    Q_OBJECT

   public:
    static TaskProgressCenter& instance();

    void track(Task* task);
    bool runTask(Task* task);

   signals:
    void taskStarted(Task* task);
    void taskStatusChanged(Task* task, const QString& status);
    void taskDetailsChanged(Task* task, const QString& details);
    void taskProgressChanged(Task* task, qint64 current, qint64 total);
    void taskAbortStateChanged(Task* task, bool canAbort, const QString& actionText);
    void taskFinished(Task* task, bool successful, const QString& error);

   private:
    explicit TaskProgressCenter(QObject* parent = nullptr);

    QList<QPointer<Task>> m_tasks;
};
