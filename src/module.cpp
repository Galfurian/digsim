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

} // namespace digsim
