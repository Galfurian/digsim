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
    digsim::logger.set_level(digsim::log_level_t::trace);

    digsim::info("Main", "=== Initializing simulation ===");

    // Signals
    digsim::signal_t<bool> clk_out("clk_out");
    digsim::signal_t<bool> d("d");
    digsim::signal_t<bool> q("q");
    digsim::signal_t<bool> q_not("q_not");

    // Clock with 2ns period (1ns high, 1ns low)
    digsim::clock_t clk("clk", 2, 1);
    clk.out(clk_out);

    // D Flip-Flop with 1ns delay
    DFlipFlop dff("dff");
    dff.clk(clk_out);
    dff.d(d);
    dff.q(q);
    dff.q_not(q_not);
    q.set_delay(1); // Set propagation delay for Q output

    // Probe output with default callback
    digsim::probe_t<bool> probe("probe");
    probe.in(q);

    d.set(1); // preload D

    digsim::dependency_graph.export_dot("example3.dot");

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.initialize();
    digsim::scheduler.run(4);

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

