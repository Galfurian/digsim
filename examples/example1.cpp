/// @file example1.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/full_adder.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    // Inputs
    simcore::signal_t<bool> a("a");
    simcore::signal_t<bool> b("b");
    simcore::signal_t<bool> cin("cin");

    simcore::signal_t<bool> sum("sum");
    simcore::signal_t<bool> count("count");

    simcore::signal_t<bool> not_sum("not_sum", false, 1);
    simcore::signal_t<bool> not_cout("not_cout", false, 1);

    simcore::signal_t<bool> sum_out("sum_out", false, 1);
    simcore::signal_t<bool> cout_out("cout_out", false, 1);

    // Full Adder
    FullAdder fa("fa");
    fa.a(a);
    fa.b(b);
    fa.cin(cin);
    fa.sum(sum);
    fa.cout(count);

    // The first NotGate inverts the sum output.
    NotGate not_sum_1("sum_not1");
    not_sum_1.in(sum);
    not_sum_1.out(not_sum);

    // The second NotGate inverts the output of the first NotGate.
    NotGate not_sum_2("sum_not2");
    not_sum_2.in(not_sum);
    not_sum_2.out(sum_out);

    // Delay chain: cout -> not1 -> not2
    NotGate not_cout_1("cout_not1");
    not_cout_1.in(count);
    not_cout_1.out(not_cout);

    NotGate not_cout_2("cout_not2");
    not_cout_2.in(not_cout);
    not_cout_2.out(cout_out);

    // Output probes
    Probe<bool> p1("p1");
    p1.in(sum_out);

    Probe<bool> p2("p2");
    p2.in(cout_out);

    // Export graph.
    simcore::dependency_graph.export_dot("example1.dot");

    simcore::info("Main", "=== Initializing simulation ===");

    simcore::scheduler.initialize();

    simcore::info("Main", "=== Running simulation ===");

    // Apply input pattern (e.g., a=1, b=1, cin=0)
    a.set(true);
    b.set(true);
    cin.set(false);

    simcore::scheduler.run();

    simcore::info("Main", "=== Simulation finished ===");

    return 0;
}

