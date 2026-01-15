/// @file signal.hpp
/// @brief Defines a signal class that can be used to represent a signal in a digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "simcore/isignal.hpp"
#include "simcore/logger.hpp"
#include "simcore/scheduler.hpp"

#include <cmath>
#include <limits>
#include <type_traits>
#include <unordered_set>

namespace simcore
{

/// @brief The signal_t class represents a signal in a digital simulation.
/// @tparam T the type of the signal value.
template <typename T> class signal_t : public isignal_t
{
public:
    /// @brief Constructor for the signal_t class.
    /// @param _name the name of the signal.
    /// @param _initial the initial value of the signal, defaulting to T{}.
    /// @param _delay the default delay for this signal, defaulting to 0.
    signal_t(const std::string &_name, T _initial = T{}, discrete_time_t _delay = 0);

    /// @brief Initializes the signal with a value.
    /// @param _value the value to initialize the signal with.
    void initialize(T _value);

    /// @brief Sets the default delay for this signal.
    /// @param _delay the default delay to set for this signal.
    void set_delay(discrete_time_t _delay);

    /// @brief Sets the value of the signal.
    /// @param new_value the new value to set the signal to.
    void set(T new_value);

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    /// @brief Checks if the signal has changed since the last time it was checked.
    /// @return true if the signal has changed, false otherwise.
    bool has_changed() const;

    /// @brief Returns the module that owns this signal, however, signals do not belong to any module.
    /// @return a pointer to the module that owns this signal.
    module_t *get_owner() const override { return nullptr; }

    void operator()(isignal_t &_signal) override;

    void subscribe(const process_info_t &proc_info) override;

    discrete_time_t get_delay() const override;

    bool bound() const override;

    const isignal_t *get_bound_signal() const override;

    const char *get_type_name() const override;

private:
    /// @brief Sets the value of the signal immediately.
    /// @param new_value the new value to set the signal to.
    void set_now(T new_value);

    /// @brief Sets the value of the signal after a delay.
    /// @param new_value the new value to set the signal to.
    /// @param _delay the delay before the value is set.
    void set_delayed(T new_value, discrete_time_t _delay);

    /// @brief Applies the stored value to the signal.
    void apply_stored();

    /// @brief The current value of the signal.
    T value;
    /// @brief The last value of the signal, used to detect changes.
    T last_value;
    /// @brief The value to be stored for delayed application.
    T stored_value;
    /// @brief The default delay for this signal.
    discrete_time_t delay;
    /// @brief A set of processes that are registered to be notified when the signal changes.
    std::unordered_set<process_info_t, process_info_hash, process_info_equal> processes;

    friend class input_t<T>;
    friend class output_t<T>;
};

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

template <typename T> inline bool signal_t<T>::has_changed() const
{
    if constexpr (std::is_floating_point_v<T>) {
        T diff = std::abs(value - last_value);
        T scale = std::max(std::abs(value), std::abs(last_value));
        return diff > std::numeric_limits<T>::epsilon() * (scale > 0 ? scale : 1);
    } else {
        return value != last_value;
    }
}

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
        simcore::trace("input_t", "Process already subscribed for signal `{}`", get_name());
        return;
    }
    simcore::trace("signal_t", "Subscribing process `{}` for signal `{}`", proc_info.to_string(), get_name());
    processes.insert(proc_info);
}

template <typename T> inline discrete_time_t signal_t<T>::get_delay() const { return delay; }

template <typename T> inline bool signal_t<T>::bound() const { return !processes.empty(); }

template <typename T> const isignal_t *signal_t<T>::get_bound_signal() const { return nullptr; }

template <typename T> inline const char *signal_t<T>::get_type_name() const { return typeid(T).name(); }

template <typename T> inline void signal_t<T>::set_now(T new_value)
{
    bool has_changed = false;
    if constexpr (std::is_floating_point_v<T>) {
        // For floating point types, use epsilon-based comparison to avoid precision issues
        T diff = std::abs(new_value - value);
        T scale = std::max(std::abs(value), std::abs(new_value));
        has_changed = diff > std::numeric_limits<T>::epsilon() * (scale > 0 ? scale : 1);
    } else {
        has_changed = (new_value != value);
    }

    if (has_changed) {
        // Update the last value to the current value before changing it.
        last_value = value;
        // Update the value to the new value.
        value      = new_value;
        simcore::trace("signal_t", "{}: {} -> {} (now)", get_name(), last_value, value);
        for (auto &proc_info : processes) {
            // Schedule the process to be executed immediately.
            simcore::scheduler.schedule_now(proc_info);
        }
    }
}

template <typename T> inline void signal_t<T>::set_delayed(T new_value, discrete_time_t _delay)
{
    simcore::trace("signal_t", "{}: {} -> {} (delayed by {})", get_name(), value, new_value, _delay);
    // Store the new value to be applied after the delay.
    stored_value = new_value;
    // Create a process that will apply the stored value after the delay.
    auto process = simcore::get_or_create_process(this, &signal_t::apply_stored, "delayed");
    // Schedule the process to be executed after the specified delay.
    simcore::scheduler.schedule_after(process, _delay);
}

template <typename T> inline void signal_t<T>::apply_stored() { this->set_now(stored_value); }

} // namespace simcore
