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
probe_t<T>::probe_t(
    const std::string &_name,
    digsim::signal_t<T> &_input,
    std::function<void(const digsim::signal_t<T> &)> _callback)
    : module_t(_name)
    , input(_input)
    , callback(std::move(_callback))
{
    add_sensitivity(&probe_t::evaluate, input);
}

template <typename T> void probe_t<T>::evaluate()
{
    if (input.has_changed()) {
        if (callback) {
            callback(input);
        }
    }
}

} // namespace digsim
