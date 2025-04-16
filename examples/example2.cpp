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

    digsim::info("Main", "=== Initializing simulation ===");

    // Inputs.
    digsim::signal_t<bool> a("a");
    digsim::signal_t<bool> b("b");
    digsim::signal_t<bool> sel("sel");

    // Models.
    Mux2to1 mux("mux", a, b, sel);
    NotGate inv1("inv1", mux.out, 1);
    NotGate inv2("inv2", inv1.output, 1);
    digsim::probe_t probe("probe", inv2.output);

    probe.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "output = " + std::to_string(signal.get()));
    };

    digsim::info("Main", "=== Running simulation ===");

    a.set(0);
    b.set(1);
    sel.set(0); // select 'a' = 0

    digsim::scheduler.run();

    sel.set(1); // select 'b' = 1

    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");
}

