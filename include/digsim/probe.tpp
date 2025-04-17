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
    , in(_name + "_in")
    , callback(std::move(_callback))
{
    add_sensitivity(&probe_t::evaluate, in);
}

template <typename T> void probe_t<T>::evaluate()
{
    if (callback) {
        callback(in);
    }
}

} // namespace digsim
