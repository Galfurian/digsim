/// @file test_cpu.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/cpu.hpp"

/// @brief Toggle clock with rising edge.
/// @param clk The clock signal to toggle.
void toggle_clock(digsim::signal_t<bool> &clk)
{
    clk.set(false);
    digsim::scheduler.run(); // falling edge
    clk.set(true);
    digsim::scheduler.run(); // rising edge
}

#if 0

/// @brief Runs the instruction for a given number of clock phases.
/// @param clk The clock signal to toggle.
/// @param phases The number of clock phases to run.
void run_instruction(digsim::signal_t<bool> &clk)
{
    for (std::size_t i = 0; i < NUM_PHASES; ++i) {
        digsim::info("Test", "--------------------------------------");
        digsim::info("Test", "Running clock phase {}", i);
        digsim::info("Test", "");
        toggle_clock(clk);
        digsim::info("Test", "");
    }
}

inline uint16_t encode_instruction(opcode_t op, uint8_t rs, uint8_t rt_or_imm)
{
    uint8_t opcode = static_cast<uint8_t>(op) >> 4;
    uint8_t func   = static_cast<uint8_t>(op) & 0xF;
    return encode_instruction(opcode, func, rs, rt_or_imm);
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // --------------------------------------------------
    // ROM Program
    std::vector<uint16_t> program = {
        encode_instruction(opcode_t::ALU_ADD, 1, 2),   // r1 + r2 → r1
        encode_instruction(opcode_t::MEM_STORE, 2, 3), // MEM[r2 + imm] = r3
        encode_instruction(opcode_t::MEM_LOAD, 3, 4),  // r4 = MEM[r3 + imm]
        encode_instruction(opcode_t::SYS_NOP, 0, 0),   // NOP
    };

    // Clock and reset signals
    digsim::signal_t<bool> clk("clk");
    digsim::signal_t<bool> reset("reset");

    // Instantiate CPU
    cpu_t cpu("cpu", program);
    cpu.clk(clk);
    cpu.reset(reset);

    digsim::info("Test", "=========================================");
    digsim::info("Test", "Initializing CPU...");
    digsim::info("Test", "");
    digsim::scheduler.initialize();
    toggle_clock(clk);
    digsim::info("Test", "");
    digsim::info("Test", "CPU initialized.");

    // --------------------------------------------------
    // Reset
    digsim::info("Test", "=========================================");
    digsim::info("Test", "Resetting CPU...");
    digsim::info("Test", "");
    reset.set(true);
    clk.set(true);
    digsim::scheduler.run();

    reset.set(false);
    clk.set(false);
    digsim::scheduler.run();

    digsim::info("Test", "");
    digsim::info("Test", "Reset complete.");

    // --------------------------------------------------
    // Preload registers: r1 = 5, r2 = 7, r3 = 0x10
    digsim::info("Test", "=========================================");
    digsim::info("Test", "Preloading registers...");
    cpu.reg.debug_write(1, 0x05);
    cpu.reg.debug_write(2, 0x07);
    cpu.reg.debug_write(3, 0x10);
    digsim::info("Test", "Registers preloaded.");

    // --------------------------------------------------
    // ADD r0 = r1 + r2
    digsim::info("Test", "=========================================");
    digsim::info("Test", "Running ADD...");

    run_instruction(clk);

    uint16_t r0 = cpu.reg.debug_read(0);
    digsim::info("Test", "ADD result: r0 = 0x{:02X}", r0);
    if (r0 != 0x0C) {
        digsim::error("Test", "ADD FAILED: Expected r0 = 0x0C, got 0x{:02X}", r0);
        return 1;
    }

    // --------------------------------------------------
    // STORE r2 → MEM[r3]
    digsim::info("Test", "=========================================");
    digsim::info("Test", "Running STORE...");

    run_instruction(clk);

    uint16_t mem_10 = cpu.ram.debug_read(0x10);

    if (mem_10 != 0x07) {
        digsim::error("Test", "STORE FAILED: Expected mem[0x10] = 0x07, got 0x{:02X}", mem_10);
        return 1;
    }

    // --------------------------------------------------
    // LOAD r4 = MEM[r3]
    digsim::info("Test", "=========================================");
    digsim::info("Test", "Running LOAD...");
    run_instruction(clk);
    uint16_t r4 = cpu.reg.debug_read(4);
    if (r4 != 0x07) {
        digsim::error("Test", "LOAD FAILED: Expected r4 = 0x07, got 0x{:02X}", r4);
        return 1;
    }

    // --------------------------------------------------
    // NOP
    digsim::info("Test", "=========================================");
    digsim::info("Test", "Running NOP...");
    run_instruction(clk);
    // No observable state change expected

    digsim::info("Test", "All CPU instruction tests passed.");
    return 0;
}
#else
int main() { return 0; }
#endif