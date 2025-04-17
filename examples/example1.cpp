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
    FullAdder fa("fa", a, b, cin);
    // Delay chain: sum → inv1 → inv2
    NotGate inv_sum_1("sum_inv1", fa.sum, 1);
    NotGate inv_sum_2("sum_inv2", inv_sum_1.output, 1);
    // Delay chain: cout → inv1 → inv2
    NotGate inv_cout_1("cout_inv1", fa.cout, 1);
    NotGate inv_cout_2("cout_inv2", inv_cout_1.output, 1);
    // Output probes.
    digsim::probe_t p1("sum", inv_sum_2.output);
    digsim::probe_t p2("cout", inv_cout_2.output);

    p1.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "sum = " + std::to_string(signal.get()));
    };
    p2.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "cout = " + std::to_string(signal.get()));
    };

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

