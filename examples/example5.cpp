/// @file example5.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/probe.hpp"

#include <simcore/simcore.hpp>

int main()
{
    simcore::set_log_level(simcore::log_level_t::debug);

    simcore::info("Main", "=== Initializing simulation ===");

    simcore::signal_t<bool> clk_out("clk_out");

    // Clock: 2ns period (1ns high, 1ns low)
    Clock clk("clk");
    clk.out(clk_out);

    Probe<bool> probe("probe");
    probe.in(clk_out);

    simcore::dependency_graph.export_dot("example5.dot");

    simcore::info("Main", "=== Running simulation ===");

    simcore::scheduler.initialize();

    simcore::scheduler.run(10);

    simcore::info("Main", "=== Simulation finished ===");
    return 0;
}
