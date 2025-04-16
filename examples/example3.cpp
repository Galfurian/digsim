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
    // Clock with 2ns period (1ns high, 1ns low)
    digsim::clock_t clk("clk", 2, 1);
    // D Flip-Flop with 1ns delay
    DFlipFlop dff("dff", clk.out, d, 1);
    // Probe output
    digsim::probe_t probe("probe", dff.q);

    probe.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "output = " + std::to_string(signal.get()));
    };

    d.set(1); // preload D

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.run(4);

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

