/// @file example3.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/d_flip_flop.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    simcore::info("Main", "=== Initializing simulation ===");

    // Signals
    simcore::signal_t<bool> clk_out("clk_out");
    simcore::signal_t<bool> d("d");
    simcore::signal_t<bool> en("en");
    simcore::signal_t<bool> rst("rst");
    simcore::signal_t<bool> q("q");
    simcore::signal_t<bool> q_not("q_not");

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

    simcore::dependency_graph.export_dot("example3.dot");

    simcore::info("Main", "=== Running simulation ===");

    simcore::scheduler.initialize();
    simcore::scheduler.run(4);

    simcore::info("Main", "=== Simulation finished ===");

    return 0;
}

