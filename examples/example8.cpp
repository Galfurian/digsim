/// @file example8.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    // Create signal for loop
    simcore::signal_t<bool> not1_out("not1_out", false, 1);
    simcore::signal_t<bool> not2_out("not2_out", false, 1);

    // Create inverters
    NotGate not1("not1");
    not1.in(not2_out);
    not1.out(not1_out);

    NotGate not2("not2");
    not2.in(not1_out);
    not2.out(not2_out);

    simcore::dependency_graph.export_dot("example8.dot");

    simcore::info("Main", "=== Initializing simulation ===");

    simcore::scheduler.initialize();

    simcore::info("Main", "=== Running simulation ===");

    not2_out.set(true);
    simcore::scheduler.run(5);

    simcore::info("Main", "=== Simulation finished ===");
    return 0;
}
