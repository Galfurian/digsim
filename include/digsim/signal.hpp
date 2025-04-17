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

/// @brief Abstract base interface for all signals (type-erased).
class isignal_t : public named_object_t
{
public:
    /// @brief Constructor for the isignal_t class.
    /// @param _name the name of the signal.
    isignal_t(const std::string &_name)
        : named_object_t(_name)
    {
        // Nothing to do here.
    }

    virtual ~isignal_t() = default;

    /// @brief Returns the default delay of this signal.
    virtual discrete_time_t get_delay() const = 0;

    /// @brief Returns the type name of the signal (e.g., "bool", "int").
    virtual const char *get_type_name() const = 0;
};

template <typename T> class input_t;
template <typename T> class output_t;

/// @brief The signal_t class represents a signal in a digital simulation.
/// @tparam T the type of the signal value.
template <typename T> class signal_t : public isignal_t
{
public:
    /// @brief Constructor for the signal_t class.
    /// @param _name the name of the signal.
    /// @param _initial the initial value of the signal, defaulting to T{}.
    /// @param _default_delay the default propagation delay for this signal.
    signal_t(const std::string &_name, T _initial = T{}, discrete_time_t _default_delay = 0);

    /// @brief Registers a process to be notified when the signal changes.
    /// @param process a shared pointer to the process that should be notified.
    void on_change(std::shared_ptr<process_t> process);

    /// @brief Sets the value of the signal.
    /// @param new_value the new value to set the signal to.
    /// @param delay the delay before the value is set, defaulting to 0.
    void set(T new_value, std::optional<discrete_time_t> delay = 0);

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    /// @brief Checks if the signal has changed since the last time it was checked.
    /// @return true if the signal has changed, false otherwise.
    bool has_changed() const;

    /// @brief Gets the default delay for this signal.
    /// @return the default delay for this signal.
    discrete_time_t get_delay() const override;

    /// @brief Gets the type name of the signal (e.g., "bool", "int").
    /// @return the type name of the signal.
    const char *get_type_name() const override;

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
    /// @brief The default delay for this signal.
    discrete_time_t default_delay;
    /// @brief A set of processes that are registered to be notified when the signal changes.
    std::unordered_set<std::shared_ptr<process_t>> processes;

    friend class input_t<T>;
};

template <typename T> class output_t : public isignal_t
{
public:
    output_t(const std::string &_name);

    /// @brief Checks if this input is bound to a signal.
    /// @return true if this input is bound to a signal, false otherwise.
    /// @return false if this input is not bound to any signal.
    inline bool bound() const { return bound_signal != nullptr; }

    /// @brief Binds this output to a signal.
    /// @param signal the signal to bind this output to.
    void bind(signal_t<T> &signal);

    /// @brief Sets the value of the signal.
    /// @param new_value the new value to set the signal to.
    /// @param delay the delay before the value is set, defaulting to 0.
    void set(T new_value, std::optional<discrete_time_t> delay = 0);

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    /// @brief Gets the default delay for this signal.
    /// @return the default delay for this signal.
    discrete_time_t get_delay() const override;

    /// @brief Gets the type name of the signal (e.g., "bool", "int").
    /// @return the type name of the signal.
    const char *get_type_name() const override;

private:
    signal_t<T> *bound_signal;
    inline static const std::string unnamed = "<unbound output>";

    friend class input_t<T>;
};

template <typename T> class input_t : public isignal_t
{
public:
    input_t(const std::string &_name);

    /// @brief Checks if this input is bound to a signal.
    /// @return true if this input is bound to a signal, false otherwise.
    /// @return false if this input is not bound to any signal.
    inline bool bound() const { return bound_signal != nullptr; }

    /// @brief Binds this input to a signal.
    /// @param signal the signal to bind this input to.
    void bind(signal_t<T> &signal);

    /// @brief Binds this input to a signal.
    /// @param signal the signal to bind this input to.
    void bind(output_t<T> &output);

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    /// @brief Registers a process to be notified when the signal changes.
    /// @param process a shared pointer to the process that should be notified.
    void on_change(std::shared_ptr<process_t> process);

    /// @brief Gets the default delay for this signal.
    /// @return the default delay for this signal.
    discrete_time_t get_delay() const override;

    /// @brief Gets the type name of the signal (e.g., "bool", "int").
    /// @return the type name of the signal.
    const char *get_type_name() const override;

private:
    signal_t<T> *bound_signal;
};

} // namespace digsim

#include "signal.tpp"
