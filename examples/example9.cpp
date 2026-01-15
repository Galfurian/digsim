/// @file example8.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/clock.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Create signal for loop
    digsim::signal_t<bool> sig("sig", false, 1);

    // Create inverters
    NotGate not_gate("not_gate");
    not_gate.in(sig);
    not_gate.out(sig);

    digsim::dependency_graph.export_dot("example9.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.run(5);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
