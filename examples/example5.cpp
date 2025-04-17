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

    // Control signals
    digsim::signal_t<bool> en("en");
    digsim::signal_t<bool> rst("rst");

    // Clock: 2ns period (1ns high, 1ns low)
    digsim::clock_t clk("clk", 2, 1);

    // Counter
    Counter4BitRipple counter("counter", clk.out, en, rst);

    // Probes
    digsim::probe_t p0("q0", counter.q0);
    digsim::probe_t p1("q1", counter.q1);
    digsim::probe_t p2("q2", counter.q2);
    digsim::probe_t p3("q3", counter.q3);

    p0.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "q0 = " + std::to_string(signal.get()));
    };
    p1.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "q1 = " + std::to_string(signal.get()));
    };
    p2.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "q2 = " + std::to_string(signal.get()));
    };
    p3.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "q3 = " + std::to_string(signal.get()));
    };

    digsim::scheduler.run(20); // simulate 20ns

    digsim::dependency_graph.export_dot("example5.dot");

    digsim::info("Main", "=== Running simulation ===");
    // Initialize state
    en.set(1);  // enable counting
    rst.set(0); // no reset

    digsim::scheduler.run(20); // simulate 20ns

    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
