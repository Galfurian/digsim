/// @file example3.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/clock.hpp"
#include "models/d_flip_flop.hpp"
#include "models/probe.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    digsim::info("Main", "=== Initializing simulation ===");

    // Signals
    digsim::signal_t<bool> clk_out("clk_out");
    digsim::signal_t<bool> d("d");
    digsim::signal_t<bool> en("en");
    digsim::signal_t<bool> rst("rst");
    digsim::signal_t<bool> q("q");
    digsim::signal_t<bool> q_not("q_not");

    // Clock with 2ns period (1ns high, 1ns low)
    Clock clk("clk", 2, 1);
    clk.out(clk_out);

    // D Flip-Flop with 1ns delay
    DFlipFlop dff("dff");
    dff.clk(clk_out);
    dff.d(d);
    dff.enable(en);
    dff.reset(rst);
    dff.q(q);
    dff.q_not(q_not);
    q.set_delay(1); // Set propagation delay for Q output

    // Probe output with default callback
    Probe<bool> probe("probe");
    probe.in(q);

    d.set(1); // preload D

    digsim::dependency_graph.export_dot("example3.dot");

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.initialize();
    digsim::scheduler.run(4);

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

