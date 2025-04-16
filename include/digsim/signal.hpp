/// @file signal.hpp
/// @brief Defines a signal class that can be used to represent a signal in a digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "common.hpp"
#include "named_objec.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace digsim
{

/// @brief The signal_t class represents a signal in a digital simulation.
/// @tparam T the type of the signal value.
template <typename T> class signal_t : public named_object_t
{
public:
    /// @brief Constructor for the signal_t class.
    /// @param _name the name of the signal.
    /// @param initial the initial value of the signal, defaulting to T{}.
    signal_t(const std::string &_name, T initial = T{});

    /// @brief Sets the value of the signal.
    /// @param new_value the new value to set the signal to.
    /// @param delay the delay before the value is set, defaulting to 0.
    void set(T new_value, discrete_time_t delay = 0);

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    /// @brief Registers a process to be notified when the signal changes.
    /// @param process a shared pointer to the process that should be notified.
    void on_change(std::shared_ptr<process_t> process);

    /// @brief Checks if the signal has changed since the last time it was checked.
    /// @return true if the signal has changed, false otherwise.
    bool has_changed() const;

private:
    /// @brief Sets the value of the signal immediately.
    /// @param new_value the new value to set the signal to.
    void set_now(T new_value);

    /// @brief Sets the value of the signal after a delay.
    /// @param new_value the new value to set the signal to.
    /// @param delay the delay before the value is set.
    void set_delayed(T new_value, discrete_time_t delay);

    /// @brief Applies the stored value to the signal.
    void apply_stored();

    /// @brief The current value of the signal.
    T value;
    /// @brief The last value of the signal, used to detect changes.
    T last_value;
    /// @brief The value to be stored for delayed application.
    T stored_value;
    /// @brief A set of processes that are registered to be notified when the signal changes.
    std::unordered_set<std::shared_ptr<process_t>> processes;
};

} // namespace digsim

#include "signal.tpp"
