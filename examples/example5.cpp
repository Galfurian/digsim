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

    digsim::signal_t<bool> clk_out("clk_out");

    // Clock: 2ns period (1ns high, 1ns low)
    digsim::clock_t clk("clk");
    clk.out(clk_out);

    digsim::probe_t<bool> probe("probe");
    probe.in(clk_out);

    digsim::dependency_graph.export_dot("example5.dot");

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.initialize();

    digsim::scheduler.run(10);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
