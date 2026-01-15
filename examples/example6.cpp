/// @file example6.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/gates/and_gate.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    simcore::info("Main", "=== Initializing simulation ===");

    // Signals
    simcore::signal_t<bool> a("a");
    simcore::signal_t<bool> b("b");
    simcore::signal_t<bool> x("x");
    simcore::signal_t<bool> and_out("and_out");
    simcore::signal_t<bool> not_out("not_out");

    // Gates
    AndGate gate_and("and_gate");
    gate_and.a(a);
    gate_and.b(b);
    gate_and.out(and_out);
    and_out.set_delay(1); // Set a delay of 1 time unit for the AND gate

    NotGate gate_not("not_gate");
    gate_not.in(x);
    gate_not.out(not_out);
    not_out.set_delay(2); // Set a delay of 2 time units for the NOT gate

    // Probes
    Probe<bool> probe_and("probe_and");
    probe_and.in(and_out);

    Probe<bool> probe_not("probe_not");
    probe_not.in(not_out);

    // Export the dependency graph
    simcore::dependency_graph.export_dot("example6.dot");

    simcore::info("Main", "=== Begin gate test ===");

    simcore::scheduler.initialize();

    // Test all combinations for AND gate
    a.set(false);
    b.set(false);
    simcore::scheduler.run();

    a.set(false);
    b.set(true);
    simcore::scheduler.run();

    a.set(true);
    b.set(false);
    simcore::scheduler.run();

    a.set(true);
    b.set(true);
    simcore::scheduler.run();

    // Test NOT gate
    x.set(false);
    simcore::scheduler.run();

    x.set(true);
    simcore::scheduler.run();

    simcore::info("Main", "=== Simulation finished ===");
    return 0;
}

