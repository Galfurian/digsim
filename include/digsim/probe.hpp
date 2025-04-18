/// @file probe.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/module.hpp"

namespace digsim
{

/// @brief A support module that probes a signal and calls a callback function.
/// @tparam T the type of the signal to probe.
template <typename T> class probe_t : public module_t
{
public:
    /// @brief A reference to the input signal.
    digsim::input_t<T> in;

    /// @brief The callback function to call when the input signal changes.s
    std::function<void(const digsim::input_t<T> &)> callback;

    /// @brief Constructor for the probe module.
    /// @param _name the name of the module.
    /// @param _input the input signal to probe.
    /// @param _callback the callback function to call when the input signal changes.
    probe_t(const std::string &_name, std::function<void(const digsim::input_t<T> &)> _callback = nullptr);

private:
    /// @brief Evaluate the input signal and call the callback if it has changed.
    void evaluate();

    void default_callback(const digsim::input_t<T> &_in) const;
};

} // namespace digsim

#include "digsim/probe.tpp"
