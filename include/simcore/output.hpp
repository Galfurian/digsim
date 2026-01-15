/// @file output.hpp
/// @brief

#pragma once

#include "simcore/isignal.hpp"

namespace simcore
{

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
    module_t *sig_owner                           = nullptr;
    /// @brief The signal this input or output is bound to.
    isignal_t *bound_signal                       = nullptr;
    /// @brief List of sub-outputs that are bound to this output.
    std::unordered_set<output_t<T> *> sub_outputs = {};
};

template <typename T>
output_t<T>::output_t(const std::string &_name, module_t *_sig_owner)
    : isignal_t(_name)
    , sig_owner(_sig_owner)
    , bound_signal()
    , sub_outputs()
{
    // Nothing to do here.
}

template <typename T> void output_t<T>::set(T new_value)
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Output not bound: " + get_signal_location_string(this));
    }
    signal->set(new_value);
}

template <typename T> T output_t<T>::get() const
{
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Output not bound: " + get_signal_location_string(this));
    }
    return signal->get();
}

template <typename T> void output_t<T>::operator()(isignal_t &binding)
{
    if (auto *output = dynamic_cast<output_t<T> *>(&binding)) {
        simcore::trace(
            "output_t", "Binding output `{}` to output `{}`", get_signal_location_string(this),
            get_signal_location_string(output));
        // Binding a submodule output to this output (chaining).
        // Track the sub-output so we can propagate signal binding later.
        output->sub_outputs.insert(this);
    } else if (auto *signal = dynamic_cast<signal_t<T> *>(&binding)) {
        simcore::trace(
            "output_t", "Binding output `{}` to signal `{}`", get_signal_location_string(this), signal->get_name());
        // Set the bound signal.
        bound_signal = signal;
        // Recursively propagate to all sub-outputs.
        for (auto *sub_output : sub_outputs) {
            (*sub_output)(*signal);
        }
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
    auto signal = dynamic_cast<signal_t<T> *>(bound_signal);
    if (!signal) {
        throw std::runtime_error("Output not bound: " + get_signal_location_string(this));
    }
    return signal->get_delay();
}

template <typename T> bool output_t<T>::bound() const { return bound_signal != nullptr; }

template <typename T> const isignal_t *output_t<T>::get_bound_signal() const { return bound_signal; }

template <typename T> inline const char *output_t<T>::get_type_name() const { return typeid(T).name(); }

} // namespace simcore
