/// @file signal.tpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/signal.hpp"

#include "digsim/logger.hpp"
#include "digsim/scheduler.hpp"

namespace digsim
{

inline std::string binding_chain_to_string(const isignal_t *signal)
{
    if (!signal) {
        return "null";
    }
    std::stringstream ss;
    do {
        ss << signal->get_name();
        if (signal->get_bound_signal()) {
            ss << " -> ";
        }
    } while ((signal = signal->get_bound_signal()));
    return ss.str();
}

template <typename T> signal_t<T> *resolve_signal(const isignal_t *signal_if)
{
    if (!signal_if) {
        return nullptr;
    }
    auto signal = dynamic_cast<const signal_t<T> *>(signal_if);
    if (signal) {
        return const_cast<signal_t<T> *>(signal);
    }
    return resolve_signal<T>(signal_if->get_bound_signal());
}

template <typename T> inline output_t<T> *resolve_final_output(const output_t<T> *start)
{
    const output_t<T> *curr = start;
    while (auto *next = dynamic_cast<const output_t<T> *>(curr->get_bound_signal())) {
        curr = next;
    }
    return const_cast<output_t<T> *>(curr);
}

template <typename T> inline input_t<T> *resolve_final_input(const input_t<T> *start)
{
    const input_t<T> *curr = start;
    while (auto *next = dynamic_cast<const input_t<T> *>(curr->get_bound_signal())) {
        curr = next;
    }
    return const_cast<input_t<T> *>(curr);
}

// =============================================================================
// SIGNAL
// =============================================================================

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

template <typename T> inline void signal_t<T>::set_delay(discrete_time_t _delay) { delay = _delay; }

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

template <typename T> inline void signal_t<T>::subscribe(const process_info_t &proc_info)
{
    if (!proc_info.process) {
        throw std::runtime_error("Cannot subscribe an invalid process to signal `" + get_name() + "`.");
    }
    if (!proc_info.key) {
        throw std::runtime_error("Cannot subscribe a process with a null key to signal `" + get_name() + "`.");
    }
    if (processes.find(proc_info) != processes.end()) {
        digsim::trace("input_t", "Process already subscribed for signal `{}`", get_name());
        return;
    }
    digsim::trace("signal_t", "Subscribing process `{}` for signal `{}`", proc_info.to_string(), get_name());
    processes.insert(proc_info);
}

template <typename T> inline discrete_time_t signal_t<T>::get_delay() const { return delay; }

template <typename T> inline bool signal_t<T>::bound() const { return !processes.empty(); }

template <typename T> const isignal_t *signal_t<T>::get_bound_signal() const { return nullptr; }

template <typename T> inline const char *signal_t<T>::get_type_name() const { return typeid(T).name(); }

template <typename T> inline void signal_t<T>::set_now(T new_value)
{
    if (new_value != value) {
        digsim::trace("signal_t", "{}: {} -> {} (now)", get_name(), value, new_value);
        // Update the last value to the current value before changing it.
        last_value = value;
        // Update the value to the new value.
        value      = new_value;
        for (auto &proc_info : processes) {
            // Schedule the process to be executed immediately.
            digsim::scheduler.schedule_now(proc_info);
        }
    }
}

template <typename T> inline void signal_t<T>::set_delayed(T new_value, discrete_time_t _delay)
{
    digsim::trace("signal_t", "{}: {} -> {} (delayed by {})", get_name(), value, new_value, _delay);
    // Store the new value to be applied after the delay.
    stored_value = new_value;
    // Create a process that will apply the stored value after the delay.
    auto process = digsim::get_or_create_process(this, &signal_t::apply_stored, "delayed");
    // Schedule the process to be executed after the specified delay.
    digsim::scheduler.schedule_after(process, _delay);
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
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Output `" + get_name() + "` not bound to a signal.");
    }
    signal->set(new_value);
}

template <typename T> T output_t<T>::get() const
{
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Output `" + get_name() + "` not bound to a signal.");
    }
    return signal->get();
}

template <typename T> void output_t<T>::operator()(isignal_t &binding)
{
    if (auto *output = dynamic_cast<output_t<T> *>(&binding)) {
        // Bind the output to this output.
        output->bound_signal = this;

    } else if (auto *signal = dynamic_cast<signal_t<T> *>(&binding)) {
        // Resolve the final output in the chain.
        output_t<T> *resolved = resolve_final_output(this);
        // Check if the output is already bound to a signal.
        if (resolved->bound_signal) {
            throw std::runtime_error("Output `" + resolved->get_name() + "` already bound to a signal.");
        }
        // Bind the output to the signal.
        resolved->bound_signal = signal;

    } else {
        throw std::runtime_error("Output `" + get_name() + "` cannot be bound to `" + binding.get_name() + "`.");
    }
}

template <typename T> inline void output_t<T>::subscribe(const process_info_t &)
{
    throw std::runtime_error("Cannot use an output to subscribe a process to be notified.");
}

template <typename T> discrete_time_t output_t<T>::get_delay() const
{
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Output `" + get_name() + "` not bound to a signal.");
    }
    return signal->get_delay();
}

template <typename T> bool output_t<T>::bound() const { return resolve_signal<T>(this) != nullptr; }

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
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Input `" + get_name() + "` not bound to a signal.");
    }
    return signal->get();
}

template <typename T> inline void input_t<T>::subscribe(const process_info_t &proc_info)
{
    if (!proc_info.process) {
        throw std::runtime_error("Cannot subscribe an invalid process to input `" + get_name() + "`.");
    }
    if (!proc_info.key) {
        throw std::runtime_error("Cannot subscribe a process with a null key to input `" + get_name() + "`.");
    }
    if (processes.find(proc_info) != processes.end()) {
        digsim::trace("input_t", "Process already subscribed for input `{}`", get_name());
        return;
    }
    digsim::trace("input_t", "Subscribing process `{}` for input `{}`", proc_info.to_string(), get_name());
    processes.insert(proc_info);
}

template <typename T> void input_t<T>::operator()(isignal_t &binding)
{
    if (auto *input = dynamic_cast<input_t<T> *>(&binding)) {
        // Bind the input to this input.
        input->bound_signal = this;

    } else if (auto *signal = dynamic_cast<signal_t<T> *>(&binding)) {
        // Resolve the final input in the chain.
        input_t<T> *resolved = resolve_final_input(this);
        // Check if the input is already bound to a signal.
        if (resolved->bound_signal) {
            throw std::runtime_error("Input `" + resolved->get_name() + "` already bound to a signal.");
        }
        // Bind the input to the signal.
        resolved->bound_signal = signal;
        // Share process subscriptions.
        signal->processes.insert(resolved->processes.begin(), resolved->processes.end());

    } else {
        throw std::runtime_error("Input `" + get_name() + "` cannot be bound to `" + binding.get_name() + "`.");
    }
}

template <typename T> template <typename U> std::enable_if_t<std::is_same_v<U, bool>, bool> input_t<T>::posedge() const
{
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Input `" + get_name() + "` not bound to a signal.");
    }
    return signal->value && !signal->last_value;
}

template <typename T> template <typename U> std::enable_if_t<std::is_same_v<U, bool>, bool> input_t<T>::negedge() const
{
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Input `" + get_name() + "` not bound to a signal.");
    }
    return !signal->value && signal->last_value;
}

template <typename T> discrete_time_t input_t<T>::get_delay() const
{
    auto signal = resolve_signal<T>(this);
    if (!signal) {
        throw std::runtime_error("Input `" + get_name() + "` not bound to a signal.");
    }
    return signal->get_delay();
}

template <typename T> bool input_t<T>::bound() const { return resolve_signal<T>(this) != nullptr; }

template <typename T> const isignal_t *input_t<T>::get_bound_signal() const { return bound_signal; }

template <typename T> inline const char *input_t<T>::get_type_name() const { return typeid(T).name(); }

template <typename T> inline std::ostream &operator<<(std::ostream &os, const signal_t<T> &sig)
{
    os << sig.get();
    return os;
}

} // namespace digsim
