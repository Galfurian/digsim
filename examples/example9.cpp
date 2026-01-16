/// @file example8.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::set_log_level(simcore::log_level_t::debug);

    // Create signal for loop
    simcore::signal_t<bool> sig("sig", false, 1);

    // Create inverters
    NotGate not_gate("not_gate");
    not_gate.in(sig);
    not_gate.out(sig);

    simcore::dependency_graph.export_dot("example9.dot");

    simcore::info("Main", "=== Initializing simulation ===");

    simcore::scheduler.initialize();

    simcore::info("Main", "=== Running simulation ===");

    simcore::scheduler.run(5);

    simcore::info("Main", "=== Simulation finished ===");
    return 0;
}
