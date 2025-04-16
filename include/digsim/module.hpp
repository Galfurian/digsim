/// @file module.hpp
/// @brief Definition of the module_t class.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "signal.hpp"

#include <set>

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
    /// @param signal the signal to be added to the sensitivity list.
    template <typename Module, typename T> void add_sensitivity(void (Module::*method)(), signal_t<T> &signal);

    /// @brief Add a set of signals to the process sensitivity list.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the first signal.
    /// @tparam Signals
    /// @param method the method to be called when the signal changes.
    /// @param first the first signal to be added to the sensitivity list.
    /// @param rest the rest of the signals to be added to the sensitivity list.
    template <typename Module, typename T, typename... Signals>
    void add_sensitivity(void (Module::*method)(), signal_t<T> &first, Signals &...rest);

protected:
    /// @brief Add the signal to the process sensitivity list.
    /// @tparam T the type of the signal.
    /// @param process the process that is going to be triggered by the signal.
    /// @param signal the signal that is going to trigger the process.
    template <typename T> void add_sensitivity(std::shared_ptr<process_t> process, signal_t<T> &signal);
};

} // namespace digsim

#include "module.tpp"
