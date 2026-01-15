/// @file input.hpp
/// @brief

#pragma once

#include "simcore/isignal.hpp"

namespace simcore
{

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
    module_t *sig_owner                         = nullptr;
    /// @brief The signal this input or output is bound to.
    isignal_t *bound_signal                     = nullptr;
    /// @brief List of sub-inputs that are bound to this input.
    std::unordered_set<input_t<T> *> sub_inputs = {};
    /// @brief A set of processes that are registered to be notified when the signal changes.
    std::unordered_set<process_info_t, process_info_hash, process_info_equal> processes;
};

template <typename T>
input_t<T>::input_t(const std::string &_name, module_t *_sig_owner)
    : isignal_t(_name)
    , sig_owner(_sig_owner)
    , bound_signal()
    , sub_inputs()
    , processes()
{
    // Nothing to do here.
}

template <typename T> T input_t<T>::get() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Input not bound: " + get_signal_location_string(this));
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
        simcore::trace("input_t", "Process already subscribed for input `{}`", get_name());
        return;
    }
    simcore::trace("input_t", "Subscribing process `{}` for input `{}`", proc_info.to_string(), get_name());
    processes.insert(proc_info);
}

template <typename T> void input_t<T>::operator()(isignal_t &binding)
{
    if (auto *input = dynamic_cast<input_t<T> *>(&binding)) {
        simcore::trace(
            "input_t", "Binding input  `{}` to input `{}`", get_signal_location_string(this),
            get_signal_location_string(input));
        // Add this child to our list of sub-inputs.
        input->sub_inputs.insert(this);
    } else if (auto *signal = dynamic_cast<signal_t<T> *>(&binding)) {
        simcore::trace(
            "input_t", "Binding input  `{}` to signal `{}`", get_signal_location_string(this), signal->get_name());
        // Set the bound signal.
        bound_signal = signal;
        // Share subscriptions.
        signal->processes.insert(processes.begin(), processes.end());
        // Propagate signal binding to all children.
        for (auto *sub_input : sub_inputs) {
            (*sub_input)(*signal);
        }
    } else {
        throw std::runtime_error("Invalid binding for input `" + get_name() + "`");
    }
}

template <typename T> template <typename U> std::enable_if_t<std::is_same_v<U, bool>, bool> input_t<T>::posedge() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Input not bound: " + get_signal_location_string(this));
    }
    return signal->value && !signal->last_value;
}

template <typename T> template <typename U> std::enable_if_t<std::is_same_v<U, bool>, bool> input_t<T>::negedge() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Input not bound: " + get_signal_location_string(this));
    }
    return !signal->value && signal->last_value;
}

template <typename T> discrete_time_t input_t<T>::get_delay() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Input not bound: " + get_signal_location_string(this));
    }
    return signal->get_delay();
}

template <typename T> bool input_t<T>::bound() const { return bound_signal != nullptr; }

template <typename T> const isignal_t *input_t<T>::get_bound_signal() const { return bound_signal; }

template <typename T> inline const char *input_t<T>::get_type_name() const { return typeid(T).name(); }

template <typename T> inline std::ostream &operator<<(std::ostream &os, const signal_t<T> &sig)
{
    os << sig.get();
    return os;
}

} // namespace simcore
