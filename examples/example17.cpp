/// @file example17.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/cpu.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    constexpr size_t N = 8; // 8-bit data and address width

    // === 1. Clock and Reset Signals ===
    digsim::signal_t<bool> clk("clk", false);
    digsim::signal_t<bool> reset("reset", false);

    // === 2. ROM Program ===
    std::vector<uint64_t> rom_contents = {
        0x1001, // ADD r0 = r0 + r1
        0x2003, // SUB r2 = r2 - r3
        0x4560, // LOAD r5 -> r6
        0x3780  // STORE r7 -> mem[r8]
    };

    // === 3. Instantiate CPU ===
    cpu_t<N> cpu("cpu", rom_contents);
    cpu.clk(clk);
    cpu.reset(reset);

    digsim::dependency_graph.export_dot("cpu_test.dot");

    digsim::info("Main", "=== Initializing simulation ===");
    digsim::scheduler.initialize();

    digsim::info("Main", "=== Starting simulation ===");

    // === 4. Reset CPU ===
    reset.set(true);
    clk.set(true);
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();

    reset.set(false);

    // === 5. Manual register initialization (optional)
    // Ideally you would set specific values into reg file if exposed,
    // or patch it for now.
    // Here let's assume regs start at 0.

    // === 6. Simulate clock cycles ===
    for (int cycle = 0; cycle < 10; ++cycle) {
        digsim::info("Main", "=== Cycle {} ===", cycle);

        clk.set(true);
        digsim::scheduler.run();

        clk.set(false);
        digsim::scheduler.run();
    }

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}
