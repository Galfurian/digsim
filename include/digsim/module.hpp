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
    /// @param _name the name of the process.
    /// @param signal the signal to be added to the sensitivity list.
    template <typename Module, typename T>
    void add_sensitivity(void (Module::*method)(), const std::string _name, input_t<T> &signal);

    /// @brief Add a set of signals to the process sensitivity list.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the first signal.
    /// @tparam Signals the rest of the signals to be added to the sensitivity list.
    /// @param method the method to be called when the signal changes.
    /// @param first the first signal to be added to the sensitivity list.
    /// @param rest the rest of the signals to be added to the sensitivity list.
    template <typename Module, typename T, typename... Signals>
    void add_sensitivity(void (Module::*method)(), const std::string _name, input_t<T> &first, Signals &...rest);

    /// @brief Add a signal to the process sensitivity list that produces an output.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the signal.
    /// @param method the method to be called when the signal produces an output.
    /// @param signal the signal that is going to produce the output.
    template <typename Module, typename T>
    void add_produces(void (Module::*method)(), const std::string _name, output_t<T> &signal);

    /// @brief Add a set of signals to the process sensitivity list.
    /// @tparam Module the module type that contains the method to be called.
    /// @tparam T the type of the first signal.
    /// @tparam Signals the rest of the signals to be added to the sensitivity list.
    /// @param method the method to be called when the signal changes.
    /// @param first the first signal to be added to the sensitivity list.
    /// @param ...rest the rest of the signals to be added to the sensitivity list.
    template <typename Module, typename T, typename... Signals>
    void add_produces(void (Module::*method)(), const std::string _name, output_t<T> &first, Signals &...rest);

protected:
    /// @brief Add the signal to the process sensitivity list.
    /// @tparam T the type of the signal.
    /// @param proc_info the process information containing the process to be executed.
    /// @param signal the signal that is going to trigger the process.
    template <typename T> void add_sensitivity(const process_info_t &proc_info, input_t<T> &signal);

    /// @brief Add the signal to the process sensitivity list.
    /// @tparam T the type of the signal.
    /// @param proc_info the process information containing the process to be executed
    /// @param signal the signal that is going to produce the output.
    template <typename T> void add_produces(const process_info_t &proc_info, output_t<T> &signal);
};

} // namespace digsim

#include "module.tpp"
