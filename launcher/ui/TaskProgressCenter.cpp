// SPDX-License-Identifier: GPL-3.0-only
/*
 * Spectra Launcher
 * Copyright (C) 2026 Spectra Contributors
 */

#include "TaskProgressCenter.h"

#include <QEventLoop>
#include <QMetaObject>

#include "tasks/Task.h"

TaskProgressCenter& TaskProgressCenter::instance()
{
    static TaskProgressCenter center;
    return center;
}

TaskProgressCenter::TaskProgressCenter(QObject* parent) : QObject(parent) {}

void TaskProgressCenter::track(Task* task)
{
    if (!task || m_tasks.contains(task)) {
        return;
    }
    if (task->isFinished()) {
        emit taskStarted(task);
        emit taskFinished(task, task->wasSuccessful(), task->failReason());
        return;
    }

    m_tasks.append(task);
    connect(task, &Task::started, this, [this, task] { emit taskStarted(task); });
    connect(task, &Task::status, this, [this, task](const QString& status) { emit taskStatusChanged(task, status); });
    connect(task, &Task::details, this, [this, task](const QString& details) { emit taskDetailsChanged(task, details); });
    connect(task, &Task::progress, this,
            [this, task](qint64 current, qint64 total) { emit taskProgressChanged(task, current, total); });
    connect(task, &Task::abortStatusChanged, this,
            [this, task](bool canAbort) { emit taskAbortStateChanged(task, canAbort, task->tr("Abort")); });
    connect(task, &Task::abortButtonTextChanged, this,
            [this, task](const QString& text) { emit taskAbortStateChanged(task, task->canAbort(), text); });
    connect(task, &Task::finished, this, [this, task] {
        emit taskFinished(task, task->wasSuccessful(), task->failReason());
        m_tasks.removeAll(task);
        for (auto it = m_tasks.crbegin(); it != m_tasks.crend(); ++it) {
            if (*it && !(*it)->isFinished()) {
                emit taskStarted(*it);
                return;
            }
        }
    });
    connect(task, &QObject::destroyed, this, [this, task] { m_tasks.removeAll(task); });

    if (task->isRunning()) {
        emit taskStarted(task);
        emit taskStatusChanged(task, task->getStatus());
        emit taskDetailsChanged(task, task->getDetails());
        emit taskProgressChanged(task, task->getProgress(), task->getTotalProgress());
        emit taskAbortStateChanged(task, task->canAbort(), tr("Abort"));
    }
}

bool TaskProgressCenter::runTask(Task* task)
{
    if (!task) {
        return true;
    }

    track(task);
    if (task->isFinished()) {
        return task->wasSuccessful();
    }

    QEventLoop loop;
    QPointer<Task> guardedTask(task);
    connect(task, &Task::finished, &loop, &QEventLoop::quit);
    connect(task, &QObject::destroyed, &loop, &QEventLoop::quit);
    if (!task->isRunning()) {
        QMetaObject::invokeMethod(task, &Task::start, Qt::QueuedConnection);
    }
    if (guardedTask && !guardedTask->isFinished()) {
        loop.exec();
    }
    return guardedTask && guardedTask->wasSuccessful();
}
