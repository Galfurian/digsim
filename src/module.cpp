/// @file module.cpp
/// @brief Implementation of the module_t class methods.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "digsim/module.hpp"

#include "digsim/dependency_graph.hpp"
#include "digsim/scheduler.hpp"
#include "digsim/signal.hpp"

namespace digsim
{

module_t::module_t(const std::string &_name, module_t *_parent_module)
    : named_object_t(_name)
    , parent_module(_parent_module)
{
    // Nothing to do here.
}

void module_t::add_sensitivity(const process_info_t &proc_info, isignal_t &signal)
{
    signal.subscribe(proc_info);
    scheduler.register_initializer(proc_info);
}

void module_t::add_consumer(const process_info_t &proc_info, isignal_t &signal)
{
    dependency_graph.register_signal_consumer(&signal, proc_info);
}

void module_t::add_producer(const process_info_t &proc_info, isignal_t &signal)
{
    dependency_graph.register_signal_producer(&signal, proc_info);
}

std::string get_signal_location_string(const isignal_t *signal)
{
    if (!signal) {
        return "null";
    }
    std::stringstream ss;
    if (!signal) {
        return "<null signal>";
    }
    // Print module hierarchy if owner is known.
    module_t *module = signal->get_owner();
    std::vector<std::string> hierarchy;

    while (module) {
        hierarchy.push_back(module->get_name());
        module = module->get_parent();
    }

    // Build hierarchy string in reverse order (top-down)
    for (auto it = hierarchy.rbegin(); it != hierarchy.rend(); ++it) {
        ss << *it << "::";
    }

    ss << signal->get_name();
    return ss.str();
}

} // namespace digsim
