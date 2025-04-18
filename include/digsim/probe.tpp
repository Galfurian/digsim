/// @file probe.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/probe.hpp"

namespace digsim
{

template <typename T>
probe_t<T>::probe_t(const std::string &_name, std::function<void(const digsim::input_t<T> &)> _callback)
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

    add_sensitivity(&probe_t::evaluate, in);
}

template <typename T> void probe_t<T>::evaluate()
{
    if (callback) {
        callback(in);
    }
}

template <typename T> void probe_t<T>::default_callback(const digsim::input_t<T> &_in) const
{
    const auto &sig = _in.get_bound_signal();
    if (sig) {
        // Print the signal value.
        std::stringstream ss;
        ss << sig->get_name() << " = " << _in.get();
        digsim::info(get_name(), ss.str());
    }
}

} // namespace digsim
