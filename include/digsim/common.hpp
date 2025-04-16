/// @file common.hpp
/// @brief Common definitions and utilities for the digital simulation library.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <iostream>

namespace digsim
{

/// @brief Type of the simulation time.
using discrete_time_t = uint64_t;

/// @brief The types of the processes.
using process_t = std::function<void()>;

/// @brief Generates a unique key for a method of an object.
/// @tparam T the type of the object.
/// @param obj the object instance.
/// @param method the pointer to the method of the object.
/// @return a unique key that combines the object's address and the method's address.
template <typename T> static inline void *get_method_key(T *obj, void (T::*method)())
{
    // The union is used to safely cast the method pointer to a void pointer.
    union {
        void (T::*method_ptr)();
        void *ptr;
    } caster;
    // Ensure the object pointer is not null.
    if (!obj) {
        std::cerr << "Error: Object pointer is null." << std::endl;
        return nullptr;
    }
    // Ensure the method pointer is not null.
    if (!method) {
        std::cerr << "Error: Method pointer is null." << std::endl;
        return nullptr;
    }
    // Set the method pointer in the union.
    caster.method_ptr  = method;
    // Combine the object's address and the method's address into a unique key.
    uintptr_t combined = reinterpret_cast<uintptr_t>(obj) ^ reinterpret_cast<uintptr_t>(caster.ptr);
    // Return the combined address as a void pointer.
    return reinterpret_cast<void *>(combined);
}

/// @brief Retrieves or creates a process for a given method of an object.
/// @tparam Module the type of the object.
/// @param obj the object instance.
/// @param method the pointer to the method of the object.
/// @return a shared pointer to the process associated with the method.
template <typename Module>
static inline std::shared_ptr<process_t> get_or_create_process(Module *obj, void (Module::*method)())
{
    // Stores the processes in a static map to avoid recreating them.
    static std::unordered_map<void const *, std::shared_ptr<process_t>> method_cache;
    // Generate a unique key for the method.
    void *key = digsim::get_method_key(obj, method);
    if (!key) {
        std::cerr << "Error: Failed to generate method key." << std::endl;
        return nullptr;
    }
    // Check if the process already exists in the cache.
    auto it = method_cache.find(key);
    if (it != method_cache.end()) {
        return it->second;
    }
    // If not found, create a new process and store it in the cache.
    auto process      = std::make_shared<process_t>([obj, method]() { (obj->*method)(); });
    // Store the new process in the cache.
    method_cache[key] = process;
    // Return the newly created process.
    return process;
}

} // namespace digsim
