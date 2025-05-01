/// @file test_cu.cpp
/// @author Enrico Fraccaroli
/// @brief Control unit test using flattened opcode_t values.

#include "cpu/control_unit.hpp"
#include <tuple>
#include <vector>

/// @brief Apply input stimulus for the control unit test.
void apply_inputs(
    opcode_t opcode_val,
    digsim::signal_t<bs_opcode_t> &opcode,
    digsim::signal_t<bs_phase_t> &phase,
    phase_t phase_val)
{
    opcode.set(static_cast<uint8_t>(opcode_val)); // now using flattened opcode
    phase.set(static_cast<uint8_t>(phase_val));
}

/// @brief Execute simulation.
void execute_cycle() { digsim::scheduler.run(); }

/// @brief Verify expected control unit outputs.
int verify_outputs(
    opcode_t opcode_val,
    bool expected_reg_write,
    bool expected_mem_write,
    bool expected_mem_to_reg,
    digsim::signal_t<bs_opcode_t> &alu_op,
    digsim::signal_t<bool> &reg_write,
    digsim::signal_t<bool> &mem_write,
    digsim::signal_t<bool> &mem_to_reg)
{
    int fail = 0;

    if (alu_op.get().to_ulong() != static_cast<uint8_t>(opcode_val)) {
        digsim::error(
            "Test", "ALU opcode mismatch: expected 0x{:02X}, got 0x{:02X}", static_cast<uint8_t>(opcode_val),
            alu_op.get().to_ulong());
        fail = 1;
    }
    if (reg_write.get() != expected_reg_write) {
        digsim::error("Test", "reg_write mismatch: expected {}, got {}", expected_reg_write, reg_write.get());
        fail = 1;
    }
    if (mem_write.get() != expected_mem_write) {
        digsim::error("Test", "mem_write mismatch: expected {}, got {}", expected_mem_write, mem_write.get());
        fail = 1;
    }
    if (mem_to_reg.get() != expected_mem_to_reg) {
        digsim::error("Test", "mem_to_reg mismatch: expected {}, got {}", expected_mem_to_reg, mem_to_reg.get());
        fail = 1;
    }

    return fail;
}

/// @brief Run a control unit test with given inputs and expectations.
int run_test(
    opcode_t opcode_val,
    bool expected_reg_write,
    bool expected_mem_write,
    bool expected_mem_to_reg,
    digsim::signal_t<bs_opcode_t> &opcode,
    digsim::signal_t<bs_opcode_t> &alu_op,
    digsim::signal_t<bs_phase_t> &phase,
    digsim::signal_t<bool> &reg_write,
    digsim::signal_t<bool> &mem_write,
    digsim::signal_t<bool> &mem_to_reg)
{
    apply_inputs(opcode_val, opcode, phase, phase_t::FETCH);
    execute_cycle();
    apply_inputs(opcode_val, opcode, phase, phase_t::EXECUTE);
    execute_cycle();
    apply_inputs(opcode_val, opcode, phase, phase_t::WRITEBACK);
    execute_cycle();

    return verify_outputs(
        opcode_val, expected_reg_write, expected_mem_write, expected_mem_to_reg, alu_op, reg_write, mem_write,
        mem_to_reg);
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bs_opcode_t> opcode("opcode", 0, 0);
    digsim::signal_t<bs_opcode_t> alu_op("alu_op", 0, 0);
    digsim::signal_t<bs_phase_t> phase("phase", 0, 0);
    digsim::signal_t<bool> reg_write("reg_write", false, 0);
    digsim::signal_t<bool> mem_write("mem_write", false, 0);
    digsim::signal_t<bool> mem_to_reg("mem_to_reg", false, 0);

    // Instantiate the control unit
    control_unit_t cu0("cu0");
    cu0.opcode(opcode);
    cu0.phase(phase);
    cu0.alu_op(alu_op);
    cu0.reg_write(reg_write);
    cu0.mem_write(mem_write);
    cu0.mem_to_reg(mem_to_reg);

    digsim::scheduler.initialize();

    // List of test cases: (opcode_t, reg_write, mem_write, mem_to_reg)
    std::vector<std::tuple<opcode_t, bool, bool, bool>> test_cases = {
        {opcode_t::ALU_ADD, true, false, false},   {opcode_t::ALU_SUB, true, false, false},
        {opcode_t::ALU_MUL, true, false, false},   {opcode_t::SHIFT_LEFT, true, false, false},
        {opcode_t::CMP_EQ, true, false, false},    {opcode_t::MEM_LOAD, true, false, true},
        {opcode_t::MEM_STORE, false, true, false}, {opcode_t::SYS_NOP, false, false, false},
    };

    for (const auto &[op, rw, mw, mtr] : test_cases) {
        if (run_test(op, rw, mw, mtr, opcode, alu_op, phase, reg_write, mem_write, mem_to_reg)) {
            return 1;
        }
    }

    // Test: unknown or illegal opcode
    uint8_t unknown_opcode = 0x7F; // intentionally invalid
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    opcode.set(unknown_opcode);
    digsim::scheduler.run();

    if (reg_write.get() || mem_write.get() || mem_to_reg.get()) {
        digsim::error("Test", "Unknown opcode default FAILED (0x{:02X})", unknown_opcode);
        return 1;
    }

    return 0;
}
