/// @file common.cpp
/// @brief Common definitions and utilities for the digital simulation library.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "simcore/common.hpp"

#include "simcore/module.hpp"

#include <iostream>
#include <sstream>

namespace simcore
{

std::string object_ref_t::name() const { return ptr ? ptr->get_name() : "(anonymous)"; }

bool object_ref_t::valid() const { return ptr != nullptr; }

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

bool process_info_t::operator<(const process_info_t &other) const { return key < other.key; }

bool process_info_t::operator==(const process_info_t &other) const { return key == other.key; }

bool process_info_t::validate() const { return (process != nullptr) && (key != 0); }

std::size_t process_info_hash::operator()(const process_info_t &info) const noexcept
{
    return std::hash<std::uintptr_t>{}(info.key);
}

bool process_info_equal::operator()(const process_info_t &lhs, const process_info_t &rhs) const noexcept
{
    return lhs.key == rhs.key;
}

std::ostream &operator<<(std::ostream &os, const object_ref_t &ref) { return os << ref.name(); }

std::string format_time(discrete_time_t nanoseconds)
{
    if (nanoseconds >= 1000000000ULL) { // >= 1 second
        double seconds = static_cast<double>(nanoseconds) / 1000000000.0;
        std::stringstream ss;
        if (nanoseconds % 1000000000ULL == 0) {
            ss << static_cast<discrete_time_t>(seconds) << " s";
        } else {
            ss << std::fixed << std::setprecision(1) << seconds << " s";
        }
        return ss.str();
    } else if (nanoseconds >= 1000000ULL) { // >= 1 millisecond
        double milliseconds = static_cast<double>(nanoseconds) / 1000000.0;
        std::stringstream ss;
        if (nanoseconds % 1000000ULL == 0) {
            ss << static_cast<discrete_time_t>(milliseconds) << " ms";
        } else {
            ss << std::fixed << std::setprecision(1) << milliseconds << " ms";
        }
        return ss.str();
    } else if (nanoseconds >= 1000ULL) { // >= 1 microsecond
        double microseconds = static_cast<double>(nanoseconds) / 1000.0;
        std::stringstream ss;
        if (nanoseconds % 1000ULL == 0) {
            ss << static_cast<discrete_time_t>(microseconds) << " µs";
        } else {
            ss << std::fixed << std::setprecision(1) << microseconds << " µs";
        }
        return ss.str();
    } else { // nanoseconds
        return std::to_string(nanoseconds) + " ns";
    }
}

} // namespace simcore
