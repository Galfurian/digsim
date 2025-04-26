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
    /// @param _callback the callback function to call when the input signal changes.
    probe_t(const std::string &_name, std::function<void(const digsim::input_t<T> &)> _callback = nullptr)
        : module_t(_name)
        , in("in")
        , callback(std::move(_callback))
    {
        // If the user did not provide a callback, assign the default one.
        if (_callback) {
            callback = _callback;
        } else {
            callback = [this](const digsim::input_t<T> &sig) { this->default_callback(sig); };
        }

        ADD_SENSITIVITY(probe_t, evaluate, in);
    }

private:
    /// @brief Evaluate the input signal and call the callback if it has changed.
    void evaluate()
    {
        if (callback) {
            callback(in);
        }
    }

    void default_callback(const digsim::input_t<T> &_in) const
    {
        const auto &sig = _in.get_bound_signal();
        if (sig) {
            // Print the signal value.
            std::stringstream ss;
            ss << sig->get_name() << " = " << _in.get();
            digsim::info(get_name(), ss.str());
        }
    }
};

} // namespace digsim

