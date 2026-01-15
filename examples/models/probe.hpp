/// @file probe.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "simcore/module.hpp"

/// @brief A support module that probes a signal and calls a callback function.
/// @tparam T the type of the signal to probe.
template <typename T>
class Probe : public simcore::module_t
{
public:
    /// @brief A reference to the input signal.
    simcore::input_t<T> in;

    /// @brief The callback function to call when the input signal changes.s
    std::function<void(const simcore::input_t<T> &)> callback;

    /// @brief Constructor for the probe module.
    /// @param _name the name of the module.
    /// @param _callback the callback function to call when the input signal changes.
    Probe(const std::string &_name, std::function<void(const simcore::input_t<T> &)> _callback = nullptr)
        : module_t(_name, nullptr)
        , in("in")
        , callback(std::move(_callback))
    {
        // If the user did not provide a callback, assign the default one.
        if (_callback) {
            callback = _callback;
        } else {
            callback = [this](const simcore::input_t<T> &sig) { this->default_callback(sig); };
        }

        ADD_SENSITIVITY(Probe, evaluate, in);
    }

private:
    /// @brief Evaluate the input signal and call the callback if it has changed.
    void evaluate()
    {
        if (callback) {
            callback(in);
        }
    }

    void default_callback(const simcore::input_t<T> &_in) const
    {
        const auto &sig = _in.get_bound_signal();
        if (sig) {
            // Print the signal value.
            std::stringstream ss;
            ss << sig->get_name() << " = " << _in.get();
            simcore::info(get_name(), ss.str());
        }
    }
};
