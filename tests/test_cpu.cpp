
/// @file test_cpu.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Enhanced CPU testbench with modular test helpers and extended coverage.

#include "cpu/cpu.hpp"

/// @brief Holds the full environment to test a CPU program.
struct cpu_env_t {
    digsim::signal_t<bool> clk{"clk"};
    digsim::signal_t<bool> reset{"reset"};
    cpu_t cpu;

    int test_result = 0;

    cpu_env_t(const std::vector<uint16_t> &program)
        : cpu("cpu", program)
    {
        cpu.clk(clk);
        cpu.reset(reset);
        digsim::scheduler.initialize();
        toggle_clock();
        reset_pc();
    }

    void toggle_clock()
    {
        clk.set(false);
        digsim::scheduler.run();
        clk.set(true);
        digsim::scheduler.run();
    }

    void run_instruction()
    {
        digsim::info("Test", "");
        digsim::info("Test", "Snapshot before instruction:");
        print_registers();
        for (std::size_t i = 0; i < NUM_PHASES; ++i) {
            toggle_clock();
        }
    }

    void reset_pc()
    {
        reset.set(true);
        clk.set(true);
        digsim::scheduler.run();
        reset.set(false);
        clk.set(false);
        digsim::scheduler.run();
    }

    void set_register(uint8_t reg, uint16_t value) { cpu.reg.debug_write(reg, value); }

    void set_registers(const std::array<uint16_t, NUM_REGS> &values)
    {
        for (uint8_t i = 0; i < NUM_REGS; ++i) {
            cpu.reg.debug_write(i, values[i]);
        }
    }

    void set_memory(uint16_t addr, uint16_t value) { cpu.ram.debug_write(addr, value); }

    uint16_t read_register(uint8_t reg) { return cpu.reg.debug_read(reg); }

    uint16_t read_memory(uint16_t addr) { return cpu.ram.debug_read(addr); }

    void print_registers()
    {
        std::stringstream ss;
        for (uint8_t i = 0; i < NUM_REGS; ++i)
            ss << std::hex << std::setw(4) << std::setfill('0') << cpu.reg.debug_read(i) << " ";
        digsim::info("Test", "REGS : {}", ss.str());
    }

    void check_reg(uint8_t reg, uint16_t expected, const std::string &msg)
    {
        uint16_t value = read_register(reg);
        if (value == expected) {
            digsim::info("Test", "OK [{:24}]: r{} = 0x{:04X}", msg, reg, value);
        } else {
            digsim::error("Test", "NO [{:24}]: Expected r{} = 0x{:04X}, got 0x{:04X}", msg, reg, expected, value);
            digsim::info("Test", "Snapshot after failed test:");
            print_registers();
            test_result = 1;
        }
    }

    void check_mem(uint16_t addr, uint16_t expected, const std::string &msg)
    {
        uint16_t value = read_memory(addr);
        if (value == expected) {
            digsim::info("Test", "OK [{:24}]: mem[0x{:04X}] = 0x{:04X}", msg, addr, value);
        } else {
            digsim::error(
                "Test", "NO [{:24}]: Expected mem[0x{:04X}] = 0x{:04X}, got 0x{:04X}", msg, addr, expected, value);
            digsim::info("Test", "Snapshot after failed test:");
            print_registers();
            test_result = 1;
        }
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    std::vector<uint16_t> program = {
        encode_instruction(opcode_t::ALU_ADD, 1, 2),     // r1 = r1 + r2 (5 + 7 = 12)
        encode_instruction(opcode_t::ALU_SUB, 1, 2),     // r1 = r1 - r2 (12 - 7 = 5)
        encode_instruction(opcode_t::ALU_MUL, 1, 2),     // r1 = r1 * r2 (5 * 7 = 35)
        encode_instruction(opcode_t::ALU_DIV, 1, 2),     // r1 = r1 / r2 (35 / 7 = 5)
        encode_instruction(opcode_t::ALU_AND, 1, 2),     // r1 = r1 & r2 (5 & 7 = 5)
        encode_instruction(opcode_t::ALU_OR, 1, 2),      // r1 = r1 | r2 (5 | 7 = 7)
        encode_instruction(opcode_t::ALU_XOR, 1, 2),     // r1 = r1 ^ r2 (5 ^ 7 = 2)
        encode_instruction(opcode_t::ALU_NOT, 1, 0),     // r1 = ~r1 (~2 = 0xFFFD)
        encode_instruction(opcode_t::SHIFT_RIGHT, 1, 2), // r1 = r1 >> r2
        encode_instruction(opcode_t::SHIFT_LEFT, 1, 2),  // r1 = r1 << r2
        encode_instruction(opcode_t::CMP_EQ, 1, 2),      // r1 == r2 → 0 (false)
        encode_instruction(opcode_t::CMP_LT, 1, 2),      // r1 <  r2 → 1 (true)
        encode_instruction(opcode_t::CMP_GT, 1, 2),      // r1 >  r2 → 0 (false)
        encode_instruction(opcode_t::CMP_NEQ, 1, 2),     // r1 != r2 → 1 (true)
        encode_instruction(opcode_t::MEM_STORE, 3, 2),   // MEM[r2] = r3
        encode_instruction(opcode_t::MEM_LOAD, 2, 4),    // r4 = MEM[r2]
        encode_instruction(opcode_t::SYS_NOP, 0, 0),     // NOP
    };

    cpu_env_t env(program);

    env.set_registers({
        0x0000,                      // r0
        0x0005,                      // r1
        0x0007,                      // r2
        0x1234,                      // r3
        0x0000,                      // r4
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0 // r5-r15
    });

    // ADD: 5 + 7 = 12
    env.run_instruction();
    env.check_reg(1, 0x000C, "ADD r1 = r1 + r2");

    // SUB: 12 - 7 = 5
    env.run_instruction();
    env.check_reg(1, 0x0005, "SUB r1 = r1 - r2");

    // MUL: 5 * 7 = 35
    env.run_instruction();
    env.check_reg(1, 0x0023, "MUL r1 = r1 * r2");

    // DIV: 35 / 7 = 5
    env.run_instruction();
    env.check_reg(1, 0x0005, "DIV r1 = r1 / r2");

    // AND: 5 & 7 = 5
    env.run_instruction();
    env.check_reg(1, 0x0005, "AND r1 = r1 & r2");

    // OR: 5 | 7 = 7
    env.run_instruction();
    env.check_reg(1, 0x0007, "OR r1 = r1 | r2");

    // XOR: 7 ^ 7 = 0
    env.run_instruction();
    env.check_reg(1, 0x0000, "XOR r1 = r1 ^ r2");

    // NOT: ~2 = 0xFFFF
    env.run_instruction();
    env.check_reg(1, 0xFFFF, "NOT r1 = ~r1");

    // SHR: r1 >> r2 = 0x01FF
    env.run_instruction();
    env.check_reg(1, 0x01FF, "SHR r1 = r1 >> r2");

    // SHL: r1 << r2 = 0xFF80
    env.run_instruction();
    env.check_reg(1, 0xFF80, "SHL r1 = r1 << r2");

    // CMP_EQ: 0xFF80 == 0x0007 → false
    env.set_register(1, 0xFF80);
    env.run_instruction();
    env.check_reg(1, 0x0000, "CMP_EQ r1 == r2");

    // CMP_LT: 0xFF80 < 0x0007 → false
    env.set_register(1, 0xFF80);
    env.run_instruction();
    env.check_reg(1, 0x0000, "CMP_LT r1 < r2");

    // CMP_GT: 0xFF80 > 0x0007 → true
    env.set_register(1, 0xFF80);
    env.run_instruction();
    env.check_reg(1, 0x0001, "CMP_GT r1 > r2");

    // CMP_NEQ: 0xFF80 != 0x0007 → true
    env.set_register(1, 0xFF80);
    env.run_instruction();
    env.check_reg(1, 0x0001, "CMP_NEQ r1 != r2");

    // STORE: MEM[r2 = 7] = r3 = 0x1234
    env.run_instruction();
    env.check_mem(0x0007, 0x1234, "STORE MEM[r2] = r3");

    // LOAD: r4 = MEM[r2 = 7] → r4 = 0x1234
    env.run_instruction();
    env.check_reg(4, 0x1234, "LOAD r4 = mem[r2]");

    // Final NOP (no effect)
    env.run_instruction();

    if (env.test_result == 0)
        digsim::info("Test", "All CPU tests passed.");
    return env.test_result;
}