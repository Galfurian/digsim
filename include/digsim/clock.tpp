/// @file clock.hpp
/// @brief Implementation of a clock module for digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/clock.hpp"

namespace digsim
{

clock_t::clock_t(
    const std::string &name,
    discrete_time_t clk_period,
    double clk_duty_cycle,
    discrete_time_t clk_start_time,
    bool clk_posedge_first)
    : module_t(name)
    , out(name + "_clk", clk_posedge_first)
    , period(clk_period)
    , duty_cycle(clk_duty_cycle)
{
    auto delay = clk_start_time + static_cast<discrete_time_t>(
                                      clk_posedge_first ? static_cast<double>(period) * duty_cycle
                                                        : static_cast<double>(period) * (1 - duty_cycle));
    scheduler.schedule_after(digsim::get_or_create_process(this, &clock_t::evaluate), delay, get_name() + "_start");
}

void clock_t::evaluate()
{
    bool next = !out.get();
    out.set(next);
    auto delay = static_cast<discrete_time_t>(
        next ? static_cast<double>(period) * duty_cycle : static_cast<double>(period) * (1 - duty_cycle));
    scheduler.schedule_after(digsim::get_or_create_process(this, &clock_t::evaluate), delay, get_name() + "_evaluate");
}

} // namespace digsim
