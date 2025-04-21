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

    // Create signal for loop
    digsim::signal_t<bool> not1_out("not1_out", false, 0);
    digsim::signal_t<bool> not2_out("not2_out", false, 0);

    // Create inverters
    NotGate not1("not1");
    not1.in(not2_out);
    not1.out(not1_out);

    NotGate not2("not2");
    not2.in(not1_out);
    not2.out(not2_out);

    digsim::dependency_graph.export_dot("example8.dot");
    digsim::dependency_graph.compute_cycles();
    for (const auto &cycle : digsim::dependency_graph.get_cycles()) {
        if (digsim::dependency_graph.is_bad_cycle(cycle)) {
            digsim::info("Main", "=== Bad cycle detected ===");
            digsim::dependency_graph.print_cycle_report(cycle);
            digsim::info("Main", "==========================");
        }
    }

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    not2_out.set(true);
    digsim::scheduler.run(5);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
