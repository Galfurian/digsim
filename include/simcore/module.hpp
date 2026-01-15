/// @file module.hpp
/// @brief Definition of the module_t class.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "simcore/common.hpp"
#include "simcore/signal.hpp"

namespace simcore
{

/// @brief Base class for all modules in the digital simulator.
class module_t : public named_object_t
{
public:
    /// @brief Constructor for the module_t class.
    /// @param _name the name of the module.
    /// @param _parent_module the parent module of this module.
    module_t(const std::string &_name, module_t *_parent_module = nullptr);

    /// @brief Sets the parent module of this module.
    /// @param _parent_module the parent module to set.
    void set_parent(module_t *_parent_module) { parent_module = _parent_module; }

    /// @brief Returns the parent module of this module.
    /// @return a pointer to the parent module.
    module_t *get_parent() const { return parent_module; }

    /// @brief Adds a signal to the process sensitivity list.
    /// @tparam Module the module type that contains the method.
    /// @param method the method to be called when the signal changes.
    /// @param _name the name of the process.
    /// @param signal the signal that is going to trigger the process.
    template <typename Module>
    void add_sensitivity(void (Module::*method)(), const std::string _name, isignal_t &signal)
    {
        // Get the process information for the method.
        auto proc_info = simcore::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);
        // Adds the signal to the process sensitivity list.
        add_sensitivity(proc_info, signal);
        // Registers the process as a consumer of the signal.
        add_consumer(proc_info, signal);
    }

    /// @brief Adds a set of signals to the process sensitivity list.
    /// @tparam Module the module type that contains the method.
    /// @tparam Signals the rest of the signals.
    /// @param method the method to be called when the signal changes.
    /// @param _name the name of the process.
    /// @param first the first signal.
    /// @param rest the rest of the signals.
    template <typename Module, typename... Signals>
    void add_sensitivity(void (Module::*method)(), const std::string _name, isignal_t &first, Signals &...rest)
    {
        add_sensitivity(method, _name, first);
        (add_sensitivity(method, _name, rest), ...);
    }

    /// @brief Registers the process as a consumer of the signal.
    /// @tparam Module the module type that contains the method.
    /// @param method the method that consumes the signal.
    /// @param _name the name of the process.
    /// @param signal the signal that is going to be consumed.
    /// @note This method is used to register the process in the dependency graph.
    template <typename Module> void add_consumer(void (Module::*method)(), const std::string _name, isignal_t &signal)
    {
        // Get the process information for the method.
        auto proc_info = simcore::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);
        // Registers the signal as a consumer.
        add_consumer(proc_info, signal);
    }

    /// @brief Registers the process as a consumer of the signal.
    /// @tparam Module the module type that contains the method.
    /// @tparam Signals the rest of the signals.
    /// @param method the method belonging to the module that is going to consume the input.
    /// @param _name the name of the process.
    /// @param first the signal that is going to be consumed.
    /// @param rest the rest of the signals.
    template <typename Module, typename... Signals>
    void add_consumer(void (Module::*method)(), const std::string _name, isignal_t &first, Signals &...rest)
    {
        add_consumer(method, _name, first);
        (add_consumer(method, _name, rest), ...);
    }

    /// @brief Registers the process as a producer of the signal.
    /// @tparam Module the module type that contains the method.
    /// @param method the method belonging to the module that is going to produce the output.
    /// @param _name the name of the process.
    /// @param signal the signal that is going to be produced.
    template <typename Module> void add_producer(void (Module::*method)(), const std::string _name, isignal_t &signal)
    {
        // Get the process information for the method.
        auto proc_info = simcore::get_or_create_process<Module>(static_cast<Module *>(this), method, _name);
        // Registers the signal as a producer.
        add_producer(proc_info, signal);
    }

    /// @brief Registers the process as a producer of the signal.
    /// @tparam Module the module type that contains the method.
    /// @tparam Signals the rest of the signals.
    /// @param method the method belonging to the module that is going to produce the output.
    /// @param _name the name of the process.
    /// @param first the signal that is going to be produced.
    /// @param ...rest the rest of the signals.
    template <typename Module, typename... Signals>
    void add_producer(void (Module::*method)(), const std::string _name, isignal_t &first, Signals &...rest)
    {
        add_producer(method, _name, first);
        (add_producer(method, _name, rest), ...);
    }

protected:
    /// @brief Adds the signal to the process sensitivity list.
    /// @tparam T the type of the signal.
    /// @param proc_info the process information containing the process to be executed.
    /// @param signal the signal that is going to trigger the process.
    void add_sensitivity(const process_info_t &proc_info, isignal_t &signal);

    /// @brief Just Adds the process as a consumer of the signal, but do not register it in the scheduler.
    /// @param proc_info the process that consumes the signal.
    /// @param signal the signal that is going to be consumed.
    /// @note This method is used to register the process in the dependency graph.
    void add_consumer(const process_info_t &proc_info, isignal_t &signal);

    /// @brief Adds the process as a producer of the signal.
    /// @param proc_info the process that produces the signal.
    /// @param signal the signal that is going to be produced.
    /// @note This method is used to register the process in the dependency graph.
    void add_producer(const process_info_t &proc_info, isignal_t &signal);

private:
    /// @brief Pointer to the parent module.
    module_t *parent_module = nullptr;
};

} // namespace simcore

/// @brief Helper macro to add a sensitivity to a process.
#define ADD_SENSITIVITY(object, method, ...) add_sensitivity(&object::method, #method, __VA_ARGS__)

/// @brief Helper macro to add a consumer to a process.
#define ADD_CONSUMER(object, method, ...) add_consumer(&object::method, #method, __VA_ARGS__)

/// @brief Helper macro to add a producer to a process.
#define ADD_PRODUCER(object, method, ...) add_producer(&object::method, #method, __VA_ARGS__)
