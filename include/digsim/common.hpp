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

    inline std::string name() const;

    inline bool valid() const;
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

    bool operator<(const process_info_t &other) const { return key < other.key; }

    bool operator==(const process_info_t &other) const { return key == other.key; }

    bool validate() const { return (process != nullptr) && (key != nullptr); }
};

// Custom hasher
struct process_info_hash {
    std::size_t operator()(const process_info_t &info) const noexcept { return std::hash<const void *>{}(info.key); }
};

// Custom equality comparator
struct process_info_equal {
    bool operator()(const process_info_t &lhs, const process_info_t &rhs) const noexcept { return lhs.key == rhs.key; }
};

/// @brief Generates a unique key for a method of an object.
/// @tparam Object the type of the object.
/// @param obj the object instance.
/// @param method the pointer to the method of the object.
/// @return a unique key that combines the object's address and the method's address.
template <typename Object> void *get_method_key(Object *obj, void (Object::*method)());

/// @brief Retrieves or creates a process for a given method of an object.
/// @tparam Object the type of the object.
/// @param obj the object instance.
/// @param method the pointer to the method of the object.
/// @param name an optional name for the process, used for debugging and logging.
/// @return A process_info_t structure containing the process information.
template <typename Object>
process_info_t get_or_create_process(Object *obj, void (Object::*method)(), const std::string &name = "");

} // namespace digsim

#include "common.tpp"
