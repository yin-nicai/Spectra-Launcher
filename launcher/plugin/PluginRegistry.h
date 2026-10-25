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

#include <QHash>
#include <QList>
#include <QString>
#include <QVariantMap>

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace Plugins {

struct PluginState {
    QString id;
    bool enabled = true;
    int order = 0;
    QVariantMap options;
};

template <typename Interface>
struct PluginRegistration {
    QString id;
    QString displayName;
    QString description;
    bool enabledByDefault = false;
    std::function<std::unique_ptr<Interface>(const QVariantMap&)> create;
};

template <typename Interface>
class PluginRegistry {
   public:
    bool registerFactory(PluginRegistration<Interface> registration)
    {
        if (registration.id.isEmpty() || !registration.create || m_factories.contains(registration.id)) {
            return false;
        }

        m_factoryOrder.append(registration.id);
        m_factories.insert(registration.id, std::move(registration));
        return true;
    }

    bool configure(const QList<PluginState>& suppliedStates = {})
    {
        QHash<QString, PluginState> supplied;
        for (const auto& state : suppliedStates) {
            if (!state.id.isEmpty()) {
                supplied.insert(state.id, state);
            }
        }

        QList<PluginState> normalized;
        normalized.reserve(m_factoryOrder.size());
        int defaultOrder = 0;
        for (const auto& id : m_factoryOrder) {
            const auto& factory = m_factories[id];
            auto state = supplied.value(id, PluginState{
                                                id,
                                                factory.enabledByDefault,
                                                defaultOrder,
                                                {},
                                            });
            state.id = id;
            normalized.append(state);
            defaultOrder += 10;
        }

        QHash<QString, int> registrationOrder;
        for (int i = 0; i < m_factoryOrder.size(); ++i) {
            registrationOrder.insert(m_factoryOrder.at(i), i);
        }
        std::stable_sort(normalized.begin(), normalized.end(), [&registrationOrder](const PluginState& left, const PluginState& right) {
            if (left.order != right.order) {
                return left.order < right.order;
            }
            return registrationOrder.value(left.id) < registrationOrder.value(right.id);
        });

        m_instances.clear();
        m_states = normalized;
        for (const auto& state : m_states) {
            if (!state.enabled) {
                continue;
            }

            auto instance = m_factories.value(state.id).create(state.options);
            if (instance) {
                m_instances.push_back({ state.id, std::move(instance) });
            }
        }
        return true;
    }

    QList<Interface*> enabledInOrder() const
    {
        QList<Interface*> result;
        result.reserve(m_instances.size());
        for (const auto& entry : m_instances) {
            result.append(entry.instance.get());
        }
        return result;
    }

    Interface* firstEnabled() const { return m_instances.empty() ? nullptr : m_instances.front().instance.get(); }

    Interface* find(const QString& id) const
    {
        for (const auto& entry : m_instances) {
            if (entry.id == id) {
                return entry.instance.get();
            }
        }
        return nullptr;
    }

    QList<PluginState> currentStates() const { return m_states; }

    QList<PluginRegistration<Interface>> registrations() const
    {
        QList<PluginRegistration<Interface>> result;
        for (const auto& id : m_factoryOrder) {
            result.append(m_factories[id]);
        }
        return result;
    }

    bool hasFactory(const QString& id) const { return m_factories.contains(id); }

   private:
    struct InstanceEntry {
        QString id;
        std::unique_ptr<Interface> instance;
    };

    QList<QString> m_factoryOrder;
    QHash<QString, PluginRegistration<Interface>> m_factories;
    std::vector<InstanceEntry> m_instances;
    QList<PluginState> m_states;
};

}  // namespace Plugins
