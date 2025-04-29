/// @file example12.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/pc.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Input signals.
    digsim::signal_t<bool> clk("clk", false, 0);
    digsim::signal_t<bool> reset("reset", false, 0);
    digsim::signal_t<bool> load("load", false, 0);
    digsim::signal_t<std::bitset<8>> next_addr("next_addr", 0b00000000, 0);

    // Output signal.
    digsim::signal_t<std::bitset<8>> addr("addr", 0b00000000, 1);

    // Create the Program Counter.
    pc_t<8> pc("pc0");
    pc.clk(clk);
    pc.reset(reset);
    pc.load(load);
    pc.next_addr(next_addr);
    pc.addr(addr);

    // Export the dependency graph.
    digsim::dependency_graph.export_dot("example_pc.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    // ============================================================
    digsim::info("Main", "=== Test: Reset ===");

    reset.set(true);
    clk.set(true);
    digsim::scheduler.run();

    reset.set(false);
    clk.set(false);
    digsim::scheduler.run();

    // ============================================================
    digsim::info("Main", "=== Test: Load Specific Address ===");

    next_addr.set(0x42);
    load.set(true);
    clk.set(true);
    digsim::scheduler.run();

    load.set(false);
    clk.set(false);
    digsim::scheduler.run();

    // ============================================================
    digsim::info("Main", "=== Test: Increment Normally ===");

    clk.set(true);
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();

    clk.set(true);
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();

    clk.set(true);
    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}
