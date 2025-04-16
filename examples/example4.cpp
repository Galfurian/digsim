/// @file digital_circuit.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief
/// @version 0.1
/// @date 2024-05-13
///
/// @copyright Copyright (c) 2024
///

#include "digsim/digsim.hpp"

#include "models.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    // Initial state
    digsim::info("Main", "=== Initializing simulation ===");

    // Signals
    digsim::signal_t<bool> d("d");
    digsim::signal_t<bool> en("en");
    digsim::signal_t<bool> rst("rst");

    // Clock with 2ns period (1ns high, 1ns low)
    digsim::clock_t clk("clk", 2, 1);
    // D Flip-Flop with 1ns delay
    DFlipFlop dff("dff", clk.out, d, 1);
    // Probe output
    digsim::probe_t probe("q", dff.q);

    probe.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "output = " + std::to_string(signal.get()));
    };

    dff.connect_enable(en);
    dff.connect_reset(rst);

    // Initial state
    d.set(1);   // preload D with 1
    en.set(0);  // disabled initially
    rst.set(0); // reset inactive

    digsim::info("Main", "=== Running simulation ===");

    // Run to t=2 (disabled, rising edge ignored)
    digsim::scheduler.run(2);

    // Enable = 1, now it will latch d=1 at next rising edge (t=2)
    en.set(1);
    digsim::scheduler.run(4); // run to t=6

    // Change d=0 (still enabled), DFF should latch 0 at t=6
    d.set(0);
    digsim::scheduler.run(2); // run to t=8

    // Apply reset = 1 (should force q=0 at t=8)
    rst.set(1);
    digsim::scheduler.run(2); // run to t=10

    // Release reset, set d=1 again, expect q=1 at t=10
    rst.set(0);
    d.set(1);
    digsim::scheduler.run(2); // run to t=12

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

