/// @file event.hpp
/// @brief The event_t structure and its associated functions.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "simcore/common.hpp"

namespace simcore
{

/// @brief An event in the simulation, which consists of a time and a callback process.
struct event_t {
    /// @brief The time at which the event occurs.
    discrete_time_t time;
    /// @brief Information about the process to be executed at this event.
    process_info_t process_info;

    /// @brief Comparisong operator for events, used to order them in the priority queue.
    /// @param other The other event to compare with.
    /// @return True if this event occurs before the other event, false otherwise.
    bool operator>(const event_t &other) const { return time > other.time; }
};

} // namespace simcore
