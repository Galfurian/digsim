/// @file common.hpp
/// @brief Common definitions and utilities for the digital simulation library.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/named_object.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace digsim
{

/// @brief Type of the simulation time.
using discrete_time_t = uint64_t;

/// @brief The types of the processes.
using process_t = std::function<void()>;

/// @brief A class that holds a reference to a named object.
struct object_ref_t {
    /// @brief A pointer to a named object.
    const named_object_t *ptr = nullptr;

    /// @brief Returns the name of the object.
    /// @return The name of the object, or "(anonymous)" if the pointer is null.
    std::string name() const;

    /// @brief Checks if the object reference is valid.
    /// @return True if the pointer is not null, false otherwise.
    bool valid() const;
};

/// @brief Stores detailed information about a scheduled process.
struct process_info_t {
    std::shared_ptr<process_t> process; ///< The process to be executed.
    void const *key;                    ///< A unique key for the process.
    object_ref_t owner;                 ///< The object instance that contains the method to be executed.
    std::string name;                   ///< The name of the process, typically in the format "obj.method".

    /// @brief Returns a string representation of the process information.
    /// @return A string containing the object's address, method name, and process name.
    std::string to_string() const;

    /// @brief Compares this process_info_t with another for ordering.
    /// @param other The other process_info_t to compare with.
    /// @return True if this process_info_t is less than the other, false otherwise.
    bool operator<(const process_info_t &other) const;

    /// @brief Compares this process_info_t with another for equality.
    /// @param other The other process_info_t to compare with.
    /// @return True if this process_info_t is equal to the other, false otherwise.
    bool operator==(const process_info_t &other) const;

    /// @brief Validates the process information.
    /// @return True if the process and key are not null, false otherwise.
    bool validate() const;
};

/// @brief Custom hash function for process_info_t.
struct process_info_hash {
    /// @brief Generates a hash for the process_info_t object.
    /// @param info The process_info_t object to hash.
    /// @return The hash value of the process_info_t object.
    std::size_t operator()(const process_info_t &info) const noexcept;
};

/// @brief Custom equality function for process_info_t.
struct process_info_equal {
    /// @brief Compares two process_info_t objects for equality.
    /// @param lhs The first process_info_t object.
    /// @param rhs The second process_info_t object.
    /// @return True if the two objects are equal, false otherwise.
    bool operator()(const process_info_t &lhs, const process_info_t &rhs) const noexcept;
};

/// @brief Outputs the name of an object reference to a stream.
/// @param os The output stream to write to.
/// @param ref The object reference to output.
/// @return The output stream after writing the object's name.
std::ostream &operator<<(std::ostream &os, const object_ref_t &ref);

/// @brief Generates a unique key for a method of an object.
/// @tparam Object the type of the object.
/// @param obj the object instance.
/// @param method the pointer to the method of the object.
/// @return a unique key that combines the object's address and the method's address.
template <typename Object> void *get_method_key(Object *obj, void (Object::*method)())
{
    // The union is used to safely cast the method pointer to a void pointer.
    union {
        void (Object::*method_ptr)();
        void *ptr;
    } caster;
    // Ensure the object pointer is not null.
    if (!obj) {
        throw std::runtime_error("Object pointer is null.");
        return nullptr;
    }
    // Ensure the method pointer is not null.
    if (!method) {
        throw std::runtime_error("Method pointer is null.");
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
/// @tparam Object the type of the object.
/// @param obj the object instance.
/// @param method the pointer to the method of the object.
/// @param name an optional name for the process, used for debugging and logging.
/// @return A process_info_t structure containing the process information.
template <typename Object>
process_info_t get_or_create_process(Object *obj, void (Object::*method)(), const std::string &name = "")
{
    static std::unordered_map<void const *, process_info_t> method_cache;
    void *key = digsim::get_method_key(obj, method);
    if (!key) {
        throw std::runtime_error("Failed to generate method key.");
    }
    auto it = method_cache.find(key);
    if (it != method_cache.end()) {
        return it->second;
    }
    auto proc = std::make_shared<process_t>([obj, method]() { (obj->*method)(); });
    process_info_t info{proc, key, object_ref_t(static_cast<const named_object_t *>(obj)), name};
    method_cache[key] = info;
    return info;
}

} // namespace digsim
