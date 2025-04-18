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

inline void scheduler_t::schedule_now(std::shared_ptr<process_t> process, const std::string &name)
{
    digsim::trace("Scheduler", "Scheduling process `" + name + "` to run now.");
    schedule(event_t{now, process, name});
}

inline void
scheduler_t::schedule_after(std::shared_ptr<process_t> process, discrete_time_t delay, const std::string &name)
{
    digsim::trace("Scheduler", "Scheduling process `" + name + "` after " + std::to_string(delay) + " time units.");
    schedule(event_t{now + delay, process, name});
}

inline void scheduler_t::register_initializer(std::shared_ptr<process_t> process) { initializer_queue.insert(process); }

inline void scheduler_t::initialize()
{
    // Run all initialization callbacks.
    if (!initializer_queue.empty()) {
        digsim::debug("Scheduler", "0.1. Running initializers");
        digsim::debug("Scheduler", "    Initializer queue size: " + std::to_string(initializer_queue.size()));
        digsim::debug("Scheduler", ">>>");
        // Run all initializers.
        for (const auto &initializer : initializer_queue) {
            (*initializer)();
        }
        digsim::debug("Scheduler", "<<<");
        digsim::debug("Scheduler", "");

        // Clear the initializer queue.
        initializer_queue.clear();
    }
}

inline void scheduler_t::stabilize()
{
    digsim::debug("Scheduler", "0.2. Stabilizing the circuit");
    while (!event_queue.empty()) {
        auto next_time = event_queue.top().time;
        if (next_time > now)
            break;
        now = next_time;
        std::set<std::shared_ptr<process_t>> batch;
        while (!event_queue.empty() && event_queue.top().time == now) {
            auto &ev = event_queue.top();
            batch.insert(ev.callback);
            event_queue.pop();
        }
        for (auto &cb : batch) {
            (*cb)();
        }
    }
    print_event_queue();
    digsim::debug("Scheduler", "");
    digsim::debug("Scheduler", "");
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

        digsim::debug("Scheduler", "1. Extracting all events...");
        digsim::debug("Scheduler", "    Current time     : " + std::to_string(current_time));
        digsim::debug("Scheduler", "    Event queue size : " + std::to_string(event_queue.size()));

        // Update the current time.
        now = current_time;

        // Clear the batch for this time.
        batch.clear();

        // Extract all callbacks scheduled for this time
        while (!event_queue.empty() && event_queue.top().time == current_time) {
            if (batch.insert(event_queue.top().callback).second) {
                digsim::debug("Scheduler", "        Queuing for execution: " + event_queue.top().name);
            }
            event_queue.pop();
        }
        digsim::debug("Scheduler", "");

        // Now run the batch
        if (!batch.empty()) {
            digsim::debug("Scheduler", "2. Running batch of callbacks...");
            digsim::debug("Scheduler", ">>>");
            for (auto &callback : batch) {
                (*callback)();
            }
            digsim::debug("Scheduler", "<<<");
            digsim::debug("Scheduler", "");
        }
        print_event_queue();
        digsim::debug("Scheduler", "");
        digsim::debug("Scheduler", "");
    }
}

inline void scheduler_t::print_event_queue() const
{
    std::priority_queue<event_t, std::vector<event_t>, std::greater<>> copy = event_queue;

    std::unordered_map<discrete_time_t, std::vector<std::string>> time_buckets;

    while (!copy.empty()) {
        const auto &ev = copy.top();
        time_buckets[ev.time].push_back(ev.name.empty() ? "<unnamed>" : ev.name);
        copy.pop();
    }

    if (!time_buckets.empty()) {
        digsim::debug("Scheduler", "3. Event queue snapshot:");
        for (const auto &[t, names] : time_buckets) {
            std::stringstream ss;
            ss << "    Queue: [ ";
            for (const auto &n : names) {
                ss << n << " ";
            }
            ss << "]";
            digsim::debug("Scheduler", ss.str());
        }
    } else {
        digsim::debug("Scheduler", "3. Event queue empty.");
    }
}

} // namespace digsim
