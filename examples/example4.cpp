/// @file example4.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/d_flip_flop.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    // Signals
    simcore::signal_t<bool> clk_out("clk_out");
    simcore::signal_t<bool> d("d");
    simcore::signal_t<bool> en("en");
    simcore::signal_t<bool> rst("rst");
    simcore::signal_t<bool> q("q");
    simcore::signal_t<bool> qn("q_not");

    // Clock with 2ns period (1ns high, 1ns low)
    Clock clk("clk");
    clk.out(clk_out);

    // D Flip-Flop with 1ns delay
    DFlipFlop dff("dff");
    dff.clk(clk_out);
    dff.d(d);
    dff.enable(en);
    dff.reset(rst);
    dff.q(q);
    dff.q_not(qn);

    simcore::dependency_graph.export_dot("example4.dot");

    simcore::info("Main", "=== Running simulation ===");

    // Initial state
    d.set(1);   // preload D with 1
    en.set(0);  // disabled initially
    rst.set(0); // reset inactive

    simcore::scheduler.initialize();

    // Run to t=2 (disabled, rising edge ignored)
    simcore::scheduler.run(2);

    // Enable = 1, now it will latch d=1 at next rising edge (t=2)
    en.set(1);
    simcore::scheduler.run(4); // run to t=6

    // Change d=0 (still enabled), DFF should latch 0 at t=6
    d.set(0);
    simcore::scheduler.run(2); // run to t=8

    // Apply reset = 1 (should force q=0 at t=8)
    rst.set(1);
    simcore::scheduler.run(2); // run to t=10

    // Release reset, set d=1 again, expect q=1 at t=10
    rst.set(0);
    d.set(1);
    simcore::scheduler.run(2); // run to t=12

    simcore::info("Main", "=== Simulation finished ===");

    return 0;
}

