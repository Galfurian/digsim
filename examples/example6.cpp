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

    digsim::signal_t<bool> a("a");
    digsim::signal_t<bool> b("b");
    digsim::signal_t<bool> x("x");

    AndGate gate_and("and_gate", a, b);

    NotGate gate_not("not_gate", x);


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
    x.set(false);
    digsim::scheduler.run();
    x.set(true);
    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
