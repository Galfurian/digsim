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

    // Create signal for loop
    digsim::signal_t<bool> sig("sig", false, 1);

    // Create inverters
    NotGate not_gate("not_gate");
    not_gate.in(sig);
    not_gate.out(sig);

    digsim::dependency_graph.export_dot("example9.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.run(5);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
