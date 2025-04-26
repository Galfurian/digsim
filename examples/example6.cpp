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

    // Signals
    digsim::signal_t<bool> a("a");
    digsim::signal_t<bool> b("b");
    digsim::signal_t<bool> x("x");
    digsim::signal_t<bool> and_out("and_out");
    digsim::signal_t<bool> not_out("not_out");

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
    digsim::probe_t<bool> probe_and("probe_and");
    probe_and.in(and_out);

    digsim::probe_t<bool> probe_not("probe_not");
    probe_not.in(not_out);

    // Export the dependency graph
    digsim::dependency_graph.export_dot("example6.dot");

    digsim::info("Main", "=== Begin gate test ===");

    digsim::scheduler.initialize();

    // Test all combinations for AND gate
    a.set(false);
    b.set(false);
    digsim::scheduler.run();

    a.set(false);
    b.set(true);
    digsim::scheduler.run();

    a.set(true);
    b.set(false);
    digsim::scheduler.run();

    a.set(true);
    b.set(true);
    digsim::scheduler.run();

    // Test NOT gate
    x.set(false);
    digsim::scheduler.run();

    x.set(true);
    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}

