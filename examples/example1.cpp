/// @file example1.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/full_adder.hpp"
#include "models/not_gate.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Inputs
    digsim::signal_t<bool> a("a");
    digsim::signal_t<bool> b("b");
    digsim::signal_t<bool> cin("cin");

    digsim::signal_t<bool> sum("sum");
    digsim::signal_t<bool> count("count");

    digsim::signal_t<bool> not_sum("not_sum", false, 1);
    digsim::signal_t<bool> not_cout("not_cout", false, 1);

    digsim::signal_t<bool> sum_out("sum_out", false, 1);
    digsim::signal_t<bool> cout_out("cout_out", false, 1);

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
    digsim::probe_t<bool> p1("p1");
    p1.in(sum_out);

    digsim::probe_t<bool> p2("p2");
    p2.in(cout_out);

    // Export graph.
    digsim::dependency_graph.export_dot("example1.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    // Apply input pattern (e.g., a=1, b=1, cin=0)
    a.set(true);
    b.set(true);
    cin.set(false);

    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

