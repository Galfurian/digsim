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
signal_t<T>::signal_t(const std::string &_name, T _initial, discrete_time_t _delay)
    : isignal_t(_name)
    , value(_initial)
    , last_value(_initial)
    , stored_value(T{})
    , delay(_delay)
{
    // Nothing to do here.
}

template <typename T> inline void signal_t<T>::initialize(T _value)
{
    value        = _value;
    last_value   = _value;
    stored_value = T{};
}

template <typename T> inline void signal_t<T>::set(T new_value)
{
    if (delay > 0) {
        this->set_delayed(new_value, delay);
    } else {
        this->set_now(new_value);
    }
}

template <typename T> inline T signal_t<T>::get() const { return value; }

template <typename T> inline bool signal_t<T>::has_changed() const { return value != last_value; }

template <typename T> inline void signal_t<T>::operator()(isignal_t &_signal)
{
    throw std::runtime_error(
        "You are trying to bind " + get_name() + " to signal " + _signal.get_name() +
        ", but this is not allowed. "
        "Use input_t or output_t to bind signals.");
}

template <typename T> inline discrete_time_t signal_t<T>::get_delay() const { return delay; }

template <typename T> inline bool signal_t<T>::bound() const { return !processes.empty(); }

template <typename T> const isignal_t *signal_t<T>::get_bound_signal() const { return nullptr; }

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

template <typename T> inline void signal_t<T>::set_delayed(T new_value, discrete_time_t _delay)
{
    stored_value = new_value;
    auto process = digsim::get_or_create_process(this, &signal_t::apply_stored);
    digsim::scheduler.schedule_after(process, _delay, get_name() + "_delayed");
}

template <typename T> inline void signal_t<T>::apply_stored() { this->set_now(stored_value); }

// =============================================================================
// OUTPUT SIGNAL
// =============================================================================

template <typename T>
output_t<T>::output_t(const std::string &_name)
    : isignal_t(_name)
    , bound_signal()
{
    // Nothing to do here.
}

template <typename T> void output_t<T>::set(T new_value)
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Output `" + get_name() + "` not bound to a signal.");
    }
    signal->set(new_value);
}

template <typename T> T output_t<T>::get() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Output `" + get_name() + "` not bound to a signal.");
    }
    return signal->get();
}

template <typename T> void output_t<T>::operator()(isignal_t &_signal)
{
    auto signal = dynamic_cast<signal_t<T> *>(&_signal);
    if (!signal) {
        throw std::runtime_error("Output `" + get_name() + "` not bound to a signal.");
    }
    // Set the bound signal to the provided signal.
    bound_signal = signal;
}

template <typename T> discrete_time_t output_t<T>::get_delay() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (signal) {
        return signal->get_delay();
    }
    return 0;
}

template <typename T> bool output_t<T>::bound() const { return bound_signal != nullptr; }

template <typename T> const isignal_t *output_t<T>::get_bound_signal() const { return bound_signal; }

template <typename T> inline const char *output_t<T>::get_type_name() const { return typeid(T).name(); }

// ============================================================================
// INPUT SIGNAL
// ============================================================================

template <typename T>
input_t<T>::input_t(const std::string &_name)
    : isignal_t(_name)
    , bound_signal()
    , processes()
{
    // Nothing to do here.
}

template <typename T> T input_t<T>::get() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Input `" + get_name() + "` not bound to a signal.");
    }
    return signal->get();
}

template <typename T> inline void input_t<T>::on_change(std::shared_ptr<process_t> process)
{
    processes.insert(process);
}

template <typename T> void input_t<T>::operator()(isignal_t &_signal)
{
    auto signal = dynamic_cast<signal_t<T> *>(&_signal);
    if (!signal) {
        throw std::runtime_error("Input `" + get_name() + "` not bound to a signal.");
    }
    // Set the bound signal to the provided signal.
    bound_signal      = signal;
    // Set the processes to the signal's processes.
    signal->processes = processes;
}

template <typename T> discrete_time_t input_t<T>::get_delay() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (signal) {
        return signal->get_delay();
    }
    return 0;
}

template <typename T> bool input_t<T>::bound() const { return bound_signal != nullptr; }

template <typename T> const isignal_t *input_t<T>::get_bound_signal() const { return bound_signal; }

template <typename T> inline const char *input_t<T>::get_type_name() const { return typeid(T).name(); }

template <typename T> inline std::ostream &operator<<(std::ostream &os, const signal_t<T> &sig)
{
    os << sig.get();
    return os;
}

} // namespace digsim
