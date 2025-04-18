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
    digsim::signal_t<bool> not1_out("not1_out");
    digsim::signal_t<bool> not2_out("not2_out");

    // Create inverters
    NotGate not1("not1");
    not1.in(not2_out);
    not1.out(not1_out);

    NotGate not2("not2");
    not2.in(not1_out);
    not2.out(not2_out);
    not2_out.set_delay(1);

    digsim::dependency_graph.export_dot("example8.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.run(20);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
