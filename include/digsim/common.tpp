/// @file common.tpp
/// @brief Common definitions and utilities for the digital simulation library.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "common.hpp"

#include "module.hpp"

#include <iostream>
#include <sstream>

namespace digsim
{

inline std::string object_ref_t::name() const { return ptr ? ptr->get_name() : "(anonymous)"; }

inline bool object_ref_t::valid() const { return ptr != nullptr; }

/// @brief Outputs the name of an object reference to a stream.
/// @param os The output stream to write to.
/// @param ref The object reference to output.
/// @return The output stream after writing the object's name.
inline std::ostream &operator<<(std::ostream &os, const object_ref_t &ref) { return os << ref.name(); }

std::string process_info_t::to_string() const
{
    std::stringstream ss;
    if (auto module = dynamic_cast<const module_t *>(owner.ptr)) {
        ss << "<module: " << module->get_name() << "." << name << ">";
    } else if (auto signal = dynamic_cast<const isignal_t *>(owner.ptr)) {
        ss << "<signal: " << signal->get_name() << "." << name << ">";
    } else {
        ss << "<unknown: " << owner << "." << name << ">";
    }
    return ss.str();
}

template <typename Object> void *get_method_key(Object *obj, void (Object::*method)())
{
    // The union is used to safely cast the method pointer to a void pointer.
    union {
        void (Object::*method_ptr)();
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

template <typename Object>
process_info_t get_or_create_process(Object *obj, void (Object::*method)(), const std::string &name)
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
