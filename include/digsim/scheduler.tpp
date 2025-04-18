/// @file scheduler.tpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "logger.hpp"
#include "scheduler.hpp"

#pragma once

namespace digsim
{
scheduler_t::scheduler_t()
    : now(0)
    , event_queue()
    , initializer_queue()
{
    // Nothing to do here.
}

inline discrete_time_t scheduler_t::time() const { return now; }

inline void scheduler_t::schedule(const event_t &event) { event_queue.push(event); }

inline void scheduler_t::schedule_now(const process_info_t &proc_info)
{
    digsim::trace("scheduler_t", "Scheduling process `{}` to run immediately.", proc_info.to_string());
    schedule(event_t{now, proc_info});
}

inline void scheduler_t::schedule_after(const process_info_t &proc_info, discrete_time_t delay)
{
    digsim::trace("scheduler_t", "Scheduling process `{}` to run after {} time units.", proc_info.to_string(), delay);
    schedule(event_t{now + delay, proc_info});
}

inline void scheduler_t::register_initializer(const process_info_t &proc_info) { initializer_queue.insert(proc_info); }

inline void scheduler_t::initialize()
{
    // Run all initialization callbacks.
    if (!initializer_queue.empty()) {
        digsim::debug("scheduler_t", "0.1. Running initializers");
        digsim::debug("scheduler_t", "    Initializer queue size: " + std::to_string(initializer_queue.size()));
        digsim::debug("scheduler_t", ">>>");
        // Run all initializers.
        for (const auto &initializer : initializer_queue) {
            (*initializer.process)();
        }
        digsim::debug("scheduler_t", "<<<");
        digsim::debug("scheduler_t", "");

        // Clear the initializer queue.
        initializer_queue.clear();
    }
}

inline void scheduler_t::stabilize()
{
    digsim::debug("scheduler_t", "0.2. Stabilizing the circuit");
    while (!event_queue.empty()) {
        auto next_time = event_queue.top().time;
        if (next_time > now)
            break;
        now = next_time;
        std::set<std::shared_ptr<process_t>> batch;
        while (!event_queue.empty() && event_queue.top().time == now) {
            auto &ev = event_queue.top();
            batch.insert(ev.process_info.process);
            event_queue.pop();
        }
        for (auto &cb : batch) {
            (*cb)();
        }
    }
    print_event_queue();
    digsim::debug("scheduler_t", "");
    digsim::debug("scheduler_t", "");
}

inline void scheduler_t::run(discrete_time_t simulation_time)
{
    // This will hold the batched processes to be executed.
    std::set<std::shared_ptr<process_t>> batch;

    discrete_time_t simulation_end = now + simulation_time;

    while (!event_queue.empty()) {
        discrete_time_t current_time = event_queue.top().time;

        // Next event is beyond the allowed time.
        if ((simulation_time > 0) && (current_time > simulation_end)) {
            break;
        }

        digsim::debug("scheduler_t", "1. Extracting all events...");
        digsim::debug("scheduler_t", "    Current time     : {}", std::to_string(current_time));
        digsim::debug("scheduler_t", "    Event queue size : {}", std::to_string(event_queue.size()));

        // Update the current time.
        now = current_time;

        // Clear the batch for this time.
        batch.clear();

        // Extract all callbacks scheduled for this time
        while (!event_queue.empty() && event_queue.top().time == current_time) {
            if (batch.insert(event_queue.top().process_info.process).second) {
                digsim::debug(
                    "scheduler_t", "        Queuing for execution: {}", event_queue.top().process_info.to_string());
            }
            event_queue.pop();
        }
        digsim::debug("scheduler_t", "");

        // Now run the batch
        if (!batch.empty()) {
            digsim::debug("scheduler_t", "2. Running batch of callbacks...");
            digsim::debug("scheduler_t", ">>>");
            for (auto &callback : batch) {
                (*callback)();
            }
            digsim::debug("scheduler_t", "<<<");
            digsim::debug("scheduler_t", "");
        }
        print_event_queue();
        digsim::debug("scheduler_t", "");
        digsim::debug("scheduler_t", "");
    }
}

inline void scheduler_t::print_event_queue() const
{
    std::priority_queue<event_t, std::vector<event_t>, std::greater<>> copy = event_queue;

    std::unordered_map<discrete_time_t, std::vector<std::string>> time_buckets;

    while (!copy.empty()) {
        const auto &ev = copy.top();
        time_buckets[ev.time].push_back(ev.process_info.to_string());
        copy.pop();
    }

    if (!time_buckets.empty()) {
        digsim::debug("scheduler_t", "3. Event queue snapshot:");
        for (const auto &[t, names] : time_buckets) {
            std::stringstream ss;
            ss << "    Queue [" << std::right << std::setw(3) << t << "] : [ ";
            for (const auto &n : names) {
                ss << n << " ";
            }
            ss << "]";
            digsim::debug("scheduler_t", ss.str());
        }
    } else {
        digsim::debug("scheduler_t", "3. Event queue empty.");
    }
}

} // namespace digsim
