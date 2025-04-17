/// @file module.tpp
/// @brief Implementation of the module_t class methods.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "module.hpp"
#include "scheduler.hpp"

#pragma once

namespace digsim
{

module_t::module_t(const std::string &_name)
    : named_object_t(_name)
{
    // Nothing to do here.
}

template <typename Module, typename T> void module_t::add_sensitivity(void (Module::*method)(), signal_t<T> &signal)
{
    add_sensitivity(digsim::get_or_create_process<Module>(static_cast<Module *>(this), method), signal);
}

template <typename Module, typename T, typename... Signals>
void module_t::add_sensitivity(void (Module::*method)(), signal_t<T> &first, Signals &...rest)
{
    add_sensitivity(method, first);
    (add_sensitivity(method, rest), ...);
}

template <typename T> void module_t::add_sensitivity(std::shared_ptr<process_t> process, signal_t<T> &signal)
{
    signal.on_change(process);
    scheduler.register_initializer(process);

    // Register in dependency graph.
    dependency_graph.register_signal_consumer(&signal, process, this);
}

template <typename Module, typename T> void module_t::add_produces(void (Module::*method)(), signal_t<T> &signal)
{
    add_produces(digsim::get_or_create_process<Module>(static_cast<Module *>(this), method), signal);
}

template <typename Module, typename T, typename... Signals>
void module_t::add_produces(void (Module::*method)(), signal_t<T> &first, Signals &...rest)
{
    add_produces(method, first);
    (add_produces(method, rest), ...);
}

template <typename T> void module_t::add_produces(std::shared_ptr<process_t> process, signal_t<T> &signal)
{
    dependency_graph.register_signal_producer(&signal, process, this);
}

} // namespace digsim

