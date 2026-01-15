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
    digsim::signal_t<bool> not1_out("not1_out", false, 1);
    digsim::signal_t<bool> not2_out("not2_out", false, 1);

    // Create inverters
    NotGate not1("not1");
    not1.in(not2_out);
    not1.out(not1_out);

    NotGate not2("not2");
    not2.in(not1_out);
    not2.out(not2_out);

    digsim::dependency_graph.export_dot("example8.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    not2_out.set(true);
    digsim::scheduler.run(5);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
