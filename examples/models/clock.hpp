/// @file clock.hpp
/// @brief Clock module for digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/module.hpp"
#include "digsim/output.hpp"

/// @brief Clock module that generates a periodic signal.
class Clock : public digsim::module_t
{
public:
    /// @brief Output signal representing the clock.
    digsim::output_t<bool> out;

    /// @brief Constructor for the clock module.
    /// @param clk_name the name of the module.
    /// @param clk_period the period of the clock signal in discrete time units.
    /// @param clk_duty_cycle the duty cycle of the clock signal, as a fraction of the period.
    /// @param clk_start_time the time at which the clock starts generating signals.
    /// @param clk_posedge_first determines if the clock should start with a positive edge first.
    Clock(
        const std::string &clk_name,
        digsim::discrete_time_t clk_period     = 2,
        double clk_duty_cycle                  = 0.5,
        digsim::discrete_time_t clk_start_time = 0,
        bool clk_posedge_first                 = false)
        : digsim::module_t(clk_name)
        , out("out", this)
        , period(clk_period)
        , duty_cycle(clk_duty_cycle)
    {
        // Get the initial delay for the clock signal.
        auto delay = clk_start_time;
        if (clk_posedge_first) {
            delay += static_cast<digsim::discrete_time_t>(static_cast<double>(period) * duty_cycle);
        } else {
            delay += static_cast<digsim::discrete_time_t>(static_cast<double>(period) * (1 - duty_cycle));
        }
        // Create a process info for the clock evaluation method.
        auto proc_info = digsim::get_or_create_process(this, &Clock::evaluate, "start");
        // Schedule the first evaluation of the clock signal.
        digsim::scheduler.schedule_after(proc_info, delay);
        // Register the output signal in the dependency graph.
        ADD_PRODUCER(Clock, evaluate, out);
    }

private:
    /// @brief Evaluates the clock signal at the current simulation time.
    void evaluate()
    {
        // Get the next state of the clock signal.
        bool next = !out.get();
        // Set the output signal to the next state.
        out.set(next);
        // Compute the new delay based on the current state and duty cycle.
        digsim::discrete_time_t delay;
        if (next) {
            delay = static_cast<digsim::discrete_time_t>(static_cast<double>(period) * duty_cycle);
        } else {
            delay = static_cast<digsim::discrete_time_t>(static_cast<double>(period) * (1 - duty_cycle));
        }
        // Create the process info for the next evaluation.
        auto proc_info = digsim::get_or_create_process(this, &Clock::evaluate, "evaluate");
        // Schedule the next evaluation of the clock signal.
        digsim::scheduler.schedule_after(proc_info, delay);
    }

    /// @brief Period of the clock signal in discrete time units.
    digsim::discrete_time_t period;
    /// @brief The duty cycle of the clock signal, as a fraction of the period.
    double duty_cycle;
};