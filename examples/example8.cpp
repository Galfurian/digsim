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
    digsim::signal_t<bool> signal_a("a");

    // Inverters in loop: second one breaks loop with delay
    NotGate not1("not1", signal_a);
    NotGate not2("not2", not1.output, 1);

    signal_a.on_change(digsim::get_or_create_process(&not1, &NotGate::evaluate));

    digsim::info("Main", "=== Running loop-breaking delay test ===");

    digsim::scheduler.initialize();
    digsim::scheduler.run(20);

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}

