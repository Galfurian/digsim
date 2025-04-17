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
    digsim::logger.set_level(digsim::log_level_t::info);

    // Inputs
    digsim::signal_t<bool> a("a");
    digsim::signal_t<bool> b("b");
    digsim::signal_t<bool> cin("cin");

    // Full Adder
    FullAdder fa("fa");
    fa.a.bind(a);
    fa.b.bind(b);
    fa.cin.bind(cin);

    // Delay chain: sum → inv1 → inv2
    NotGate inv_sum_1("sum_inv1");
    inv_sum_1.in.bind(fa.sum);

    NotGate inv_sum_2("sum_inv2");
    inv_sum_2.in.bind(inv_sum_1.out);

    // Delay chain: cout → inv1 → inv2
    NotGate inv_cout_1("cout_inv1");
    inv_cout_1.in.bind(fa.cout);

    NotGate inv_cout_2("cout_inv2");
    inv_cout_2.in.bind(inv_cout_1.out);

    // Output probes
    digsim::probe_t<bool> p1("sum");
    p1.in.bind(inv_sum_2.out);

    digsim::probe_t<bool> p2("cout");
    p2.in.bind(inv_cout_2.out);

    p1.callback = [](const digsim::input_t<bool> &signal) {
        digsim::info("Main", "sum = " + std::to_string(signal.get()));
    };
    p2.callback = [](const digsim::input_t<bool> &signal) {
        digsim::info("Main", "cout = " + std::to_string(signal.get()));
    };

    // Export graph
    digsim::dependency_graph.export_dot("example1.dot");

    digsim::info("Main", "=== Initialize simulation ===");

    digsim::scheduler.initialize();
    digsim::scheduler.stabilize();

    digsim::info("Main", "=== Running simulation ===");

    // Apply input pattern (e.g., a=1, b=1, cin=0)
    a.set(true);
    b.set(true);
    cin.set(false);

    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

