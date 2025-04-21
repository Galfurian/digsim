/// @file clock.cpp
/// @brief Implementation of a clock module for digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "digsim/clock.hpp"

namespace digsim
{

clock_t::clock_t(
    const std::string &clk_name,
    discrete_time_t clk_period,
    double clk_duty_cycle,
    discrete_time_t clk_start_time,
    bool clk_posedge_first)
    : module_t(clk_name)
    , out("out")
    , period(clk_period)
    , duty_cycle(clk_duty_cycle)
{
    // Get the initial delay for the clock signal.
    auto delay = clk_start_time;
    if (clk_posedge_first) {
        delay += static_cast<discrete_time_t>(static_cast<double>(period) * duty_cycle);
    } else {
        delay += static_cast<discrete_time_t>(static_cast<double>(period) * (1 - duty_cycle));
    }
    // Create a process info for the clock evaluation method.
    auto proc_info = digsim::get_or_create_process(this, &clock_t::evaluate, "start");
    // Schedule the first evaluation of the clock signal.
    scheduler.schedule_after(proc_info, delay);
    // Register the output signal in the dependency graph.
    ADD_PRODUCES(clock_t, evaluate, out);
}

void clock_t::evaluate()
{
    // Get the next state of the clock signal.
    bool next = !out.get();
    // Set the output signal to the next state.
    out.set(next);
    // Compute the new delay based on the current state and duty cycle.
    discrete_time_t delay;
    if (next) {
        delay = static_cast<discrete_time_t>(static_cast<double>(period) * duty_cycle);
    } else {
        delay = static_cast<discrete_time_t>(static_cast<double>(period) * (1 - duty_cycle));
    }
    // Create the process info for the next evaluation.
    auto proc_info = digsim::get_or_create_process(this, &clock_t::evaluate, "evaluate");
    // Schedule the next evaluation of the clock signal.
    scheduler.schedule_after(proc_info, delay);
}

} // namespace digsim
