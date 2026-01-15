/// @file isignal.hpp
/// @brief

#pragma once

#include "simcore/common.hpp"

namespace simcore
{

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

template <typename T> class signal_t;
template <typename T> class input_t;
template <typename T> class output_t;

} // namespace simcore
