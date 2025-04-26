/// @file scheduler.cpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "digsim/scheduler.hpp"

#include "digsim/dependency_graph.hpp"
#include "digsim/logger.hpp"

namespace digsim
{
scheduler_t::scheduler_t()
    : initialized(false)
    , now(0)
    , event_queue()
    , initializer_queue()
{
    // Nothing to do here.
}

discrete_time_t scheduler_t::time() const { return now; }

void scheduler_t::schedule(const event_t &event) { event_queue.push(event); }

void scheduler_t::schedule_now(const process_info_t &proc_info)
{
    schedule(event_t{now, proc_info});
    digsim::trace("scheduler_t", "[#queue = {:-2}] Now: {} (now)", event_queue.size(), proc_info.to_string());
}

void scheduler_t::schedule_after(const process_info_t &proc_info, discrete_time_t delay)
{
    schedule(event_t{now + delay, proc_info});
    digsim::trace(
        "scheduler_t", "[#queue = {:-2}] Schedule: {} (+{}t)", event_queue.size(), proc_info.to_string(), delay);
}

void scheduler_t::register_initializer(const process_info_t &proc_info) { initializer_queue.insert(proc_info); }

void scheduler_t::initialize()
{
    if (initialized) {
        digsim::trace(
            "scheduler_t", "[#queue = {:-2}] Scheduler already initialized. Skipping initialization",
            event_queue.size());
        return;
    }
    digsim::trace("scheduler_t", "[#queue = {:-2}] -- Check for bad cycles", event_queue.size());
    // First, compute the cycles in the dependency graph.
    digsim::dependency_graph.compute_cycles();
    // Check the cycles.
    for (const auto &cycle : digsim::dependency_graph.get_cycles()) {
        if (digsim::dependency_graph.is_bad_cycle(cycle)) {
            digsim::error("scheduler_t", "Bad cycle detected:");
            digsim::dependency_graph.print_cycle_report(cycle);
            digsim::error("scheduler_t", "Exporting DOT graph as `bad_cycle_graph.dot`.");
            digsim::dependency_graph.export_dot("bad_cycle_graph.dot");
            digsim::error("scheduler_t", "Exiting.");
            std::exit(EXIT_FAILURE);
        }
    }
    // Run all initialization callbacks.
    if (!initializer_queue.empty()) {
        digsim::trace("scheduler_t", "[#queue = {:-2}] -- Begin initialization cylce", event_queue.size());
        // Run all initializers.
        for (const auto &initializer : initializer_queue) {
            (*initializer.process)();
        }
        // Clear the initializer queue.
        initializer_queue.clear();
    }
    initialized = true;
}

void scheduler_t::run(discrete_time_t simulation_time)
{
    if (!initialized) {
        digsim::trace(
            "scheduler_t", "[#queue = {:-2}] Scheduler not initialized. Calling initialize()", event_queue.size());
        initialize();
    }
    // This will hold the batched processes to be executed.
    std::set<std::shared_ptr<process_t>> batch;
    discrete_time_t simulation_end = now + simulation_time;
    while (!event_queue.empty()) {
        discrete_time_t current_time = event_queue.top().time;
        // Next event is beyond the allowed time.
        if ((simulation_time > 0) && (current_time > simulation_end)) {
            break;
        }
        digsim::trace("scheduler_t", "[#queue = {:-2}] -- Begin cylce", event_queue.size());
        // Update the current time.
        now = current_time;
        // Clear the batch for this time.
        batch.clear();
        // Extract all callbacks scheduled for this time
        while (!event_queue.empty() && event_queue.top().time == current_time) {
            if (batch.insert(event_queue.top().process_info.process).second) {
                digsim::trace(
                    "scheduler_t", "[#queue = {:-2}]     Pop: {}", event_queue.size(),
                    event_queue.top().process_info.to_string());
            }
            event_queue.pop();
        }
        // Now run the batch.
        if (!batch.empty()) {
            digsim::trace("scheduler_t", "[#queue = {:-2}] -- Run batch", event_queue.size());
            for (auto &callback : batch) {
                (*callback)();
            }
        }
        print_event_queue();
    }
}

void scheduler_t::print_event_queue() const
{
    std::priority_queue<event_t, std::vector<event_t>, std::greater<>> copy = event_queue;
    std::unordered_map<discrete_time_t, std::vector<std::string>> time_buckets;
    while (!copy.empty()) {
        const auto &ev = copy.top();
        time_buckets[ev.time].push_back(ev.process_info.to_string());
        copy.pop();
    }
    if (!time_buckets.empty()) {
        digsim::trace("scheduler_t", "[#queue = {:-2}] -- Event queue", event_queue.size());
        for (const auto &[t, names] : time_buckets) {
            std::stringstream ss;
            ss << "    Queue [" << std::right << std::setw(3) << t << "] : [ ";
            for (const auto &n : names) {
                ss << n << " ";
            }
            ss << "]";
            digsim::trace("scheduler_t", "[#queue = {:-2}] {}", event_queue.size(), ss.str());
        }
    } else {
        digsim::trace("scheduler_t", "[#queue = {:-2}] -- Event queue is empty", event_queue.size());
    }
}

} // namespace digsim
