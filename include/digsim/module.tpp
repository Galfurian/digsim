/// @file module.tpp
/// @brief Implementation of the module_t class methods.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/module.hpp"

#include "digsim/dependency_graph.hpp"
#include "digsim/scheduler.hpp"

namespace digsim
{

module_t::module_t(const std::string &_name)
    : named_object_t(_name)
{
    // Nothing to do here.
}

template <typename Module, typename T>
void module_t::add_sensitivity(void (Module::*method)(), const std::string _name, input_t<T> &signal)
{
    auto proc_info = digsim::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);

    add_sensitivity(proc_info, signal);
}

template <typename Module, typename T, typename... Signals>
void module_t::add_sensitivity(void (Module::*method)(), const std::string _name, input_t<T> &first, Signals &...rest)
{
    add_sensitivity(method, _name, first);
    (add_sensitivity(method, _name, rest), ...);
}

template <typename T> void module_t::add_sensitivity(const process_info_t &proc_info, input_t<T> &signal)
{
    signal.on_change(proc_info);
    scheduler.register_initializer(proc_info);

    // Register in dependency graph.
    dependency_graph.register_signal_consumer(&signal, proc_info);
}

template <typename Module, typename T>
void module_t::add_produces(void (Module::*method)(), const std::string _name, output_t<T> &signal)
{
    auto proc_info = digsim::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);

    add_produces(proc_info, signal);
}

template <typename Module, typename T, typename... Signals>
void module_t::add_produces(void (Module::*method)(), const std::string _name, output_t<T> &first, Signals &...rest)
{
    add_produces(method, _name, first);
    (add_produces(method, _name, rest), ...);
}

template <typename T> void module_t::add_produces(const process_info_t &proc_info, output_t<T> &signal)
{
    dependency_graph.register_signal_producer(&signal, proc_info);
}

} // namespace digsim

#define ADD_SENSITIVITY(object, method, ...) add_sensitivity(&object::method, #method, __VA_ARGS__)

#define ADD_PRODUCES(object, method, ...) add_produces(&object::method, #method, __VA_ARGS__)

