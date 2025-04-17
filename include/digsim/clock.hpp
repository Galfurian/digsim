/// @file clock.hpp
/// @brief Clock module for digital simulation.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/module.hpp"

namespace digsim
{

/// @brief Clock module that generates a periodic signal.
class clock_t : public module_t
{
public:
    /// @brief Output signal representing the clock.
    output_t<bool> out;

    /// @brief Constructor for the clock module.
    /// @param clk_name the name of the module.
    /// @param clk_period the period of the clock signal in discrete time units.
    /// @param clk_duty_cycle the duty cycle of the clock signal, as a fraction of the period.
    /// @param clk_start_time the time at which the clock starts generating signals.
    /// @param clk_posedge_first determines if the clock should start with a positive edge first.
    clock_t(
        const std::string &clk_name,
        discrete_time_t clk_period     = 2,
        double clk_duty_cycle          = 0.5,
        discrete_time_t clk_start_time = 0,
        bool clk_posedge_first         = false);

private:
    /// @brief Evaluates the clock signal at the current simulation time.
    void evaluate();

    /// @brief Period of the clock signal in discrete time units.
    discrete_time_t period;
    /// @brief The duty cycle of the clock signal, as a fraction of the period.
    double duty_cycle;
};

} // namespace digsim

#include "digsim/clock.tpp"
