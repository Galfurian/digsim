/// @file scheduler.hpp
/// @brief The scheduler class is responsible for managing the simulation time and scheduling events.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/common.hpp"
#include "digsim/event.hpp"

#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace digsim
{

/// @brief The scheduler class is responsible for managing the simulation time and scheduling events.
class scheduler_t
{
public:
    /// @brief Get the singleton instance of the scheduler.
    /// @return A reference to the singleton instance of the scheduler.
    static scheduler_t &instance()
    {
        static scheduler_t s;
        return s;
    }

    /// @brief Get the current simulation time.
    /// @return The current simulation time as a discrete_time_t value.
    discrete_time_t time() const;

    /// @brief Schedule an event to be processed at a later time.
    /// @param event The event to schedule, which includes the time, callback process, and name.
    void schedule(const event_t &event);

    /// @brief Schedule a process to be executed immediately.
    /// @param proc_info Information about the process to be executed.
    void schedule_now(const process_info_t &proc_info);

    /// @brief Schedule a process to be executed after a specified delay.
    /// @param proc_info Information about the process to be executed.
    /// @param delay the delay after which the process should be executed.
    void schedule_after(const process_info_t &proc_info, discrete_time_t delay);

    /// @brief Registers a process to be initialized at the start of the simulation.
    /// @param proc_info Information about the process to be executed.
    void register_initializer(const process_info_t &proc_info);

    /// @brief Initializes the scheduler and all registered processes.
    void initialize();

    /// @brief Runs the simulation for a specified amount of time.
    /// @param simulation_time the total time to run the simulation, defaults to 0 which means run until all events are
    /// processed.
    void run(discrete_time_t simulation_time = 0);

    /// @brief Prints the current state of the event queue for debugging purposes.
    void print_event_queue() const;

private:
    /// @brief Private constructor for the singleton pattern.
    scheduler_t();

    /// @brief Check if the scheduler is initialized.
    bool initialized;
    /// @brief The current simulation time.
    discrete_time_t now;
    /// @brief The priority queue of events, ordered by their scheduled time.
    std::priority_queue<event_t, std::vector<event_t>, std::greater<>> event_queue;
    /// @brief The list of function to call during initialization.
    std::unordered_set<process_info_t, process_info_hash, process_info_equal> initializer_queue;
};

/// @brief A reference to the singleton instance of the scheduler, for convenience.
inline scheduler_t &scheduler = scheduler_t::instance();

} // namespace digsim
