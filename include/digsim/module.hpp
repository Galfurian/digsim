/// @file module.hpp
/// @brief Definition of the module_t class.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/common.hpp"
#include "digsim/signal.hpp"

namespace digsim
{

/// @brief Base class for all modules in the digital simulator.
class module_t : public named_object_t
{
public:
    /// @brief Constructor for the module_t class.
    /// @param _name the name of the module.
    explicit module_t(const std::string &_name);

    /// @brief Add a signal to the process sensitivity list.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the signal.
    /// @param method the method to be called when the signal changes.
    /// @param _name the name of the process.
    /// @param signal the signal to be added to the sensitivity list.
    template <typename Module>
    void add_sensitivity(void (Module::*method)(), const std::string _name, isignal_t &signal)
    {
        auto proc_info = digsim::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);
        add_sensitivity(proc_info, signal);
    }

    /// @brief Add a set of signals to the process sensitivity list.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the first signal.
    /// @tparam Signals the rest of the signals to be added to the sensitivity list.
    /// @param method the method to be called when the signal changes.
    /// @param first the first signal to be added to the sensitivity list.
    /// @param rest the rest of the signals to be added to the sensitivity list.
    template <typename Module, typename... Signals>
    void add_sensitivity(void (Module::*method)(), const std::string _name, isignal_t &first, Signals &...rest)
    {
        add_sensitivity(method, _name, first);
        (add_sensitivity(method, _name, rest), ...);
    }

    /// @brief Add a signal to the process sensitivity list that produces an output.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the signal.
    /// @param method the method to be called when the signal produces an output.
    /// @param signal the signal that is going to produce the output.
    template <typename Module> void add_produces(void (Module::*method)(), const std::string _name, isignal_t &signal)
    {
        auto proc_info = digsim::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);

        add_produces(proc_info, signal);
    }

    /// @brief Add a set of signals to the process sensitivity list.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the first signal.
    /// @tparam Signals the rest of the signals to be added to the sensitivity list.
    /// @param method the method to be called when the signal changes.
    /// @param first the first signal to be added to the sensitivity list.
    /// @param ...rest the rest of the signals to be added to the sensitivity list.
    template <typename Module, typename... Signals>
    void add_produces(void (Module::*method)(), const std::string _name, isignal_t &first, Signals &...rest)
    {
        add_produces(method, _name, first);
        (add_produces(method, _name, rest), ...);
    }

protected:
    /// @brief Add the signal to the process sensitivity list.
    /// @tparam T the type of the signal.
    /// @param proc_info the process information containing the process to be executed.
    /// @param signal the signal that is going to trigger the process.
    void add_sensitivity(const process_info_t &proc_info, isignal_t &signal);

    /// @brief Add the signal to the process sensitivity list.
    /// @tparam T the type of the signal.
    /// @param proc_info the process information containing the process to be executed
    /// @param signal the signal that is going to produce the output.
    void add_produces(const process_info_t &proc_info, isignal_t &signal);
};

} // namespace digsim

#define ADD_SENSITIVITY(object, method, ...) add_sensitivity(&object::method, #method, __VA_ARGS__)

#define ADD_PRODUCES(object, method, ...) add_produces(&object::method, #method, __VA_ARGS__)
