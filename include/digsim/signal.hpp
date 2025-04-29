/// @file signal.hpp
/// @brief Defines a signal class that can be used to represent a signal in a digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/common.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace digsim
{

template <typename T> class signal_t;
template <typename T> class input_t;
template <typename T> class output_t;
class module_t;

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

    /// @brief Returns the module that owns this signal.
    /// @return a pointer to the module that owns this signal.
    virtual module_t *get_owner() const = 0;

    /// @brief Checks if this input or output is bound to a signal.
    /// @return true if this input or output is bound to a signal, false otherwise.
    virtual bool bound() const = 0;

    /// @brief Binds this output to a signal.
    /// @param signal the signal to bind this output to.
    virtual void operator()(isignal_t &signal) = 0;

    /// @brief Add the process to the list of processes that should be notified when the signal changes.
    /// @param proc_info the process information containing the process to be executed when the signal changes.
    virtual void subscribe(const process_info_t &proc_info) = 0;

    /// @brief Gets the default delay for this signal.
    /// @return the default delay for this signal.
    virtual discrete_time_t get_delay() const = 0;

    /// @brief Gets the signal this input or output is bound to.
    /// @return the signal this input or output is bound to.
    virtual const isignal_t *get_bound_signal() const = 0;

    /// @brief Returns the type name of the signal (e.g., "bool", "int").
    /// @return the type name of the signal.
    virtual const char *get_type_name() const = 0;
};

/// @brief Returns a string representation of the binding chain.
/// @param signal the signal to get the binding chain for.
/// @return a string representation of the binding chain.
std::string get_signal_location_string(const isignal_t *signal);

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

/// @brief The output_t class represents an output signal in a digital simulation.
template <typename T> class output_t : public isignal_t
{
public:
    /// @brief Constructor for the output_t class.
    /// @param _name the name of the output signal.
    /// @param _sig_owner the module that owns this output signal, defaulting to nullptr.
    output_t(const std::string &_name, module_t *_sig_owner = nullptr);

    /// @brief Returns the module that owns this signal.
    /// @return a pointer to the module that owns this signal.
    module_t *get_owner() const override { return sig_owner; }

    /// @brief Sets the value of the signal.
    /// @param new_value the new value to set the signal to.
    void set(T new_value);

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    void operator()(isignal_t &_signal) override;

    void subscribe(const process_info_t &proc_info) override;

    discrete_time_t get_delay() const override;

    bool bound() const override;

    const isignal_t *get_bound_signal() const override;

    const char *get_type_name() const override;

private:
    /// @brief The module that owns this signal.
    module_t *sig_owner     = nullptr;
    /// @brief The signal this input or output is bound to.
    isignal_t *bound_signal = nullptr;
};

/// @brief The input_t class represents an input signal in a digital simulation.
template <typename T> class input_t : public isignal_t
{
public:
    /// @brief Constructor for the input_t class.
    /// @param _name the name of the input signal.
    /// @param _sig_owner the module that owns this input signal, defaulting to nullptr.
    input_t(const std::string &_name, module_t *_sig_owner = nullptr);

    /// @brief Returns the module that owns this signal.
    /// @return a pointer to the module that owns this signal.
    module_t *get_owner() const override { return sig_owner; }

    /// @brief Gets the current value of the signal.
    /// @return the current value of the signal.
    T get() const;

    void operator()(isignal_t &_signal) override;

    void subscribe(const process_info_t &proc_info) override;

    /// @brief Returns true on a rising edge (value transition).
    /// - For bool: returns true when signal goes from false to true.
    /// - For numeric types: returns true when value > last_value.
    /// @note This is the opposite of negedge().
    /// @return true if the signal has a positive edge, false otherwise.
    template <typename U = T> std::enable_if_t<std::is_same_v<U, bool>, bool> posedge() const;

    /// @brief Returns true on a falling edge (value transition).
    /// - For bool: returns true when signal goes from true to false.
    /// - For numeric types: returns true when value < last_value.
    /// @note This is the opposite of posedge().
    /// @return true if the signal has a negative edge, false otherwise.
    template <typename U = T> std::enable_if_t<std::is_same_v<U, bool>, bool> negedge() const;

    discrete_time_t get_delay() const override;

    bool bound() const override;

    const isignal_t *get_bound_signal() const override;

    const char *get_type_name() const override;

private:
    /// @brief The module that owns this signal.
    module_t *sig_owner     = nullptr;
    /// @brief The signal this input or output is bound to.
    isignal_t *bound_signal = nullptr;
    /// @brief A set of processes that are registered to be notified when the signal changes.
    std::unordered_set<process_info_t, process_info_hash, process_info_equal> processes;
};

/// @brief Resolves the signal to its final bound signal.
/// @param signal_if the signal to start the resolution from.
/// @return the resolved signal.
template <typename T> signal_t<T> *resolve_signal(const isignal_t *signal_if);

} // namespace digsim

#include "signal.tpp"
