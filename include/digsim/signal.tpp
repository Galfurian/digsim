/// @file signal.tpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "logger.hpp"
#include "scheduler.hpp"

#pragma once

namespace digsim
{

template <typename T>
signal_t<T>::signal_t(const std::string &_name, T initial)
    : named_object_t(_name)
    , value(initial)
    , last_value(initial)
    , stored_value(initial)
    , processes()
{
    // Nothing to do here.
}

template <typename T> inline void signal_t<T>::set(T new_value, discrete_time_t delay)
{
    if (delay > 0)
        set_delayed(new_value, delay);
    else
        set_now(new_value);
}
template <typename T> inline T signal_t<T>::get() const { return value; }

template <typename T> inline void signal_t<T>::on_change(std::shared_ptr<process_t> process)
{
    processes.insert(process);
}

template <typename T> inline bool signal_t<T>::has_changed() const { return value != last_value; }

template <typename T> inline void signal_t<T>::set_now(T new_value)
{
    if (new_value != value) {
        last_value = value;
        value      = new_value;
        for (auto &process : processes) {
            digsim::scheduler.schedule_now(process, get_name() + "_now");
        }
    }
}

template <typename T> inline void signal_t<T>::set_delayed(T new_value, discrete_time_t delay)
{
    stored_value = new_value;
    auto process = digsim::get_or_create_process(this, &signal_t::apply_stored);
    digsim::scheduler.schedule_after(process, delay, get_name() + "_delayed");
}

template <typename T> inline void signal_t<T>::apply_stored() { set(stored_value, 0); }

template <typename T> inline std::ostream &operator<<(std::ostream &os, const signal_t<T> &sig)
{
    os << sig.get();
    return os;
}

} // namespace digsim
