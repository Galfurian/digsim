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
signal_t<T>::signal_t(const std::string &_name, T _initial, discrete_time_t _default_delay)
    : isignal_t(_name)
    , value(_initial)
    , last_value(_initial)
    , stored_value(_initial)
    , default_delay(_default_delay)
    , processes()
{
    // Nothing to do here.
}

template <typename T> inline void signal_t<T>::set(T new_value, std::optional<discrete_time_t> delay)
{
    auto effective_delay = delay.value_or(default_delay);
    if (effective_delay > 0) {
        set_delayed(new_value, effective_delay);
    } else {
        set_now(new_value);
    }
}
template <typename T> inline T signal_t<T>::get() const { return value; }

template <typename T> inline void signal_t<T>::on_change(std::shared_ptr<process_t> process)
{
    processes.insert(process);
}

template <typename T> inline bool signal_t<T>::has_changed() const { return value != last_value; }

template <typename T> inline discrete_time_t signal_t<T>::get_delay() const { return default_delay; }

template <typename T> inline const char *signal_t<T>::get_type_name() const { return typeid(T).name(); }

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

// ============================================================================
// INPUT SIGNAL
// ============================================================================

template <typename T>
input_t<T>::input_t(const std::string &_name)
    : isignal_t(_name)
    , bound_signal(nullptr)
{
    // Nothing to do here.
}

template <typename T> void input_t<T>::bind(signal_t<T> &signal) { bound_signal = &signal; }

template <typename T> void input_t<T>::bind(output_t<T> &output) { bound_signal = output.bound_signal; }

template <typename T> T input_t<T>::get() const
{
    if (!bound_signal) {
        throw std::runtime_error("Input `" + get_name() + "`not bound to a signal.");
    }
    return bound_signal->get();
}

template <typename T> inline void input_t<T>::on_change(std::shared_ptr<process_t> process)
{
    if (!bound_signal) {
        throw std::runtime_error("Input `" + get_name() + "`not bound to a signal.");
    }
    // Register the process to be notified when the signal changes.
    bound_signal->on_change(process);
}

template <typename T> inline discrete_time_t input_t<T>::get_delay() const
{
    if (!bound_signal) {
        throw std::runtime_error("Input `" + get_name() + "`not bound to a signal.");
    }
    return bound_signal->get_delay();
}

template <typename T> inline const char *input_t<T>::get_type_name() const
{
    if (!bound_signal) {
        throw std::runtime_error("Input `" + get_name() + "`not bound to a signal.");
    }
    return bound_signal->get_type_name();
}

// =============================================================================
// OUTPUT SIGNAL
// =============================================================================

template <typename T>
output_t<T>::output_t(const std::string &_name)
    : isignal_t(_name)
    , bound_signal(nullptr)
{
    // Nothing to do here.
}

template <typename T> void output_t<T>::bind(signal_t<T> &signal) { bound_signal = &signal; }

template <typename T> void output_t<T>::set(T new_value, std::optional<discrete_time_t> delay)
{
    if (!bound_signal) {
        throw std::runtime_error("Output `" + get_name() + "`not bound to a signal.");
    }
    bound_signal->set(new_value, delay);
}

template <typename T> T output_t<T>::get() const
{
    if (!bound_signal) {
        throw std::runtime_error("Output `" + get_name() + "`not bound to a signal.");
    }
    return bound_signal->get();
}

template <typename T> inline discrete_time_t output_t<T>::get_delay() const
{
    if (!bound_signal) {
        throw std::runtime_error("Output `" + get_name() + "`not bound to a signal.");
    }
    return bound_signal->get_delay();
}

template <typename T> inline const char *output_t<T>::get_type_name() const
{
    if (!bound_signal) {
        throw std::runtime_error("Output `" + get_name() + "`not bound to a signal.");
    }
    return bound_signal->get_type_name();
}

} // namespace digsim
