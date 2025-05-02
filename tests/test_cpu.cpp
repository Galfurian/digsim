
/// @file test_cpu.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Enhanced CPU testbench with modular test helpers and extended coverage.

#include "cpu/cpu.hpp"

void print_registers(cpu_t &cpu)
{
    std::stringstream ss;
    for (uint8_t i = 0; i < NUM_REGS; ++i) {
        ss << std::hex << std::setw(4) << std::setfill('0') << cpu.reg.debug_read(i) << " ";
    }
    digsim::info("Test", "REGS : {}", ss.str());
}

uint16_t read_register(cpu_t &cpu, uint8_t reg) { return cpu.reg.debug_read(reg); }

uint16_t read_memory(cpu_t &cpu, uint16_t addr) { return cpu.ram.debug_read(addr); }

void set_register(cpu_t &cpu, uint8_t reg, uint16_t value) { cpu.reg.debug_write(reg, value); }

void set_registers(cpu_t &cpu, const std::array<uint16_t, NUM_REGS> &values)
{
    for (uint8_t i = 0; i < NUM_REGS; ++i) {
        cpu.reg.debug_write(i, values[i]);
    }
}

void set_memory(cpu_t &cpu, uint16_t addr, uint16_t value) { cpu.ram.debug_write(addr, value); }

void toggle_clock(digsim::signal_t<bool> &clk)
{
    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);
    digsim::scheduler.run();
}

void run_instruction(digsim::signal_t<bool> &clk)
{
    digsim::info("Test", "");
    for (std::size_t i = 0; i < NUM_PHASES; ++i) {
        toggle_clock(clk);
    }
}

bool check_reg(cpu_t &cpu, uint8_t reg, uint16_t expected, const std::string &msg)
{
    uint16_t value = read_register(cpu, reg);
    if (value != expected) {
        digsim::error("Test", "FAILED [{:24}]: Expected r{} = 0x{:04X}, got 0x{:04X}", msg, reg, expected, value);
        return false;
    }
    digsim::info("Test", "OK     [{:24}]: r{} = 0x{:04X}", msg, reg, value);
    return true;
}

bool check_mem(cpu_t &cpu, uint16_t addr, uint16_t expected, const std::string &msg)
{
    uint16_t value = read_memory(cpu, addr);
    if (value != expected) {
        digsim::error(
            "Test", "FAILED [{:24}]: Expected mem[0x{:04X}] = 0x{:04X}, got 0x{:04X}", msg, addr, expected, value);
        return false;
    }
    digsim::info("Test", "OK     [{:24}]: mem[0x{:04X}] = 0x{:04X}", msg, addr, value);
    return true;
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    std::vector<uint16_t> program = {
        encode_instruction(opcode_t::ALU_ADD, 1, 2),   // r1 += r2 → 5 + 7 = 12
        encode_instruction(opcode_t::MEM_STORE, 3, 2), // MEM[r2] = r3
        encode_instruction(opcode_t::MEM_LOAD, 3, 4),  // r4 = MEM[r2]
        encode_instruction(opcode_t::ALU_SUB, 4, 1),   // r4 -= r1 → 7 - 12 = -5
        encode_instruction(opcode_t::ALU_MUL, 1, 1),   // r1 *= r1 → 12 * 12 = 144
        encode_instruction(opcode_t::ALU_DIV, 1, 2),   // r1 /= r2 → 144 / 7 = 20
        encode_instruction(opcode_t::SYS_NOP, 0, 0),   // NOP
    };

    digsim::signal_t<bool> clk("clk");
    digsim::signal_t<bool> reset("reset");

    cpu_t cpu("cpu", program);
    cpu.clk(clk);
    cpu.reset(reset);

    digsim::scheduler.initialize();
    toggle_clock(clk);

    reset.set(true);
    clk.set(true);
    digsim::scheduler.run();
    reset.set(false);
    clk.set(false);
    digsim::scheduler.run();

    // Initial values
    set_register(cpu, 0, 0x00); // r0 = 0
    set_register(cpu, 1, 0x05); // r1 = 0
    set_register(cpu, 2, 0x07); // r2 = 7
    set_register(cpu, 3, 0x10); // r3 = 0x10

    bool pass = true;

    // ADD
    run_instruction(clk);
    pass &= check_reg(cpu, 1, 0x0C, "ADD r1 = r1 + r2");

    // STORE
    run_instruction(clk);
    pass &= check_mem(cpu, 0x10, 0x07, "STORE MEM[r2] = r3");

    // LOAD
    run_instruction(clk);
    pass &= check_reg(cpu, 0x04, 0x07, "LOAD r4 = MEM[r2]");

    // SUB
    run_instruction(clk);
    pass &= check_reg(cpu, 4, 0xFFFB, "SUB r4 = r4 - r1");

    // MUL
    run_instruction(clk);
    pass &= check_reg(cpu, 1, 0x0090, "MUL r1 = r1 * r1");

    // DIV
    run_instruction(clk);
    pass &= check_reg(cpu, 1, 0x0014, "DIV r1 = r1 / r2");

    // NOP
    run_instruction(clk);

    if (pass)
        digsim::info("Test", "All tests passed.");
    return pass ? 0 : 1;
}
