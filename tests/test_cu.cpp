/// @file test_cu.cpp
/// @author Enrico Fraccaroli
/// @brief Control unit test using flattened opcode_t values.

#include "cpu/control_unit.hpp"
#include <tuple>
#include <vector>

int test_result = 0;

struct cu_env_t {
    digsim::signal_t<bs_opcode_t> opcode{"opcode", 0, 0};
    digsim::signal_t<bs_phase_t> phase{"phase", static_cast<uint16_t>(phase_t::EXECUTE), 0};
    digsim::signal_t<bool> reg_write{"reg_write", 0, 0};
    digsim::signal_t<bool> mem_write{"mem_write", 0, 0};
    digsim::signal_t<bool> mem_to_reg{"mem_to_reg", 0, 0};
    digsim::signal_t<bool> rt_as_dest{"rt_as_dest", 0, 0};
    digsim::signal_t<bool> jump_enable{"jump_enable", 0, 0};
    digsim::signal_t<bool> branch_enable{"branch_enable", 0, 0};

    control_unit_t cu{"cu"};

    cu_env_t()
    {
        cu.opcode(opcode);
        cu.phase(phase);
        cu.reg_write(reg_write);
        cu.mem_write(mem_write);
        cu.mem_to_reg(mem_to_reg);
        cu.rt_as_dest(rt_as_dest);
        cu.jump_enable(jump_enable);
        cu.branch_enable(branch_enable);
    }

    void apply_inputs(opcode_t opcode_val, phase_t phase_val)
    {
        opcode.set(static_cast<uint8_t>(opcode_val)); // now using flattened opcode
        phase.set(static_cast<uint8_t>(phase_val));
    }

    void execute_cycle() { digsim::scheduler.run(); }

    void verify_outputs(
        bool expected_reg_write,
        bool expected_mem_write,
        bool expected_mem_to_reg,
        bool expected_rt_as_dest,
        bool expected_jump_enable,
        bool expected_branch_enable)
    {
        if (reg_write.get() != expected_reg_write) {
            digsim::error("Test", "reg_write mismatch: expected {}, got {}", expected_reg_write, reg_write.get());
            test_result = 1;
        }
        if (mem_write.get() != expected_mem_write) {
            digsim::error("Test", "mem_write mismatch: expected {}, got {}", expected_mem_write, mem_write.get());
            test_result = 1;
        }
        if (mem_to_reg.get() != expected_mem_to_reg) {
            digsim::error("Test", "mem_to_reg mismatch: expected {}, got {}", expected_mem_to_reg, mem_to_reg.get());
            test_result = 1;
        }
        if (rt_as_dest.get() != expected_rt_as_dest) {
            digsim::error("Test", "rt_as_dest mismatch: expected {}, got {}", expected_rt_as_dest, rt_as_dest.get());
            test_result = 1;
        }
        if (jump_enable.get() != expected_jump_enable) {
            digsim::error("Test", "jump_enable mismatch: expected {}, got {}", expected_jump_enable, jump_enable.get());
            test_result = 1;
        }
        if (branch_enable.get() != expected_branch_enable) {
            digsim::error(
                "Test", "branch_enable mismatch: expected {}, got {}", expected_branch_enable, branch_enable.get());
            test_result = 1;
        }
    }

    void run_test(
        opcode_t opcode_val,
        bool expected_reg_write,
        bool expected_mem_write,
        bool expected_mem_to_reg,
        bool expected_rt_as_dest,
        bool expected_jump_enable,
        bool expected_branch_enable)
    {
        apply_inputs(opcode_val, phase_t::FETCH);
        execute_cycle();
        apply_inputs(opcode_val, phase_t::EXECUTE);
        execute_cycle();
        apply_inputs(opcode_val, phase_t::WRITEBACK);
        execute_cycle();
        verify_outputs(
            expected_reg_write, expected_mem_write, expected_mem_to_reg, expected_rt_as_dest, expected_jump_enable,
            expected_branch_enable);
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    cu_env_t env;

    digsim::scheduler.initialize();

    // List of test cases: (opcode_t, reg_write, mem_write, mem_to_reg, rt_as_dest, jump_enable, branch_enable)
    std::vector<std::tuple<opcode_t, bool, bool, bool, bool, bool, bool>> test_cases = {
        {opcode_t::ALU_ADD, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::ALU_SUB, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::ALU_AND, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::ALU_OR, 1, 0, 0, 0, 0, 0},       //
        {opcode_t::ALU_XOR, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::ALU_NOT, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::ALU_MUL, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::ALU_DIV, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::SHIFT_LEFT, 1, 0, 0, 0, 0, 0},   //
        {opcode_t::SHIFT_RIGHT, 1, 0, 0, 0, 0, 0},  //
        {opcode_t::SHIFT_ARITH, 1, 0, 0, 0, 0, 0},  //
        {opcode_t::SHIFT_ROTATE, 1, 0, 0, 0, 0, 0}, //
        {opcode_t::CMP_EQ, 1, 0, 0, 0, 0, 0},       //
        {opcode_t::CMP_LT, 1, 0, 0, 0, 0, 0},       //
        {opcode_t::CMP_GT, 1, 0, 0, 0, 0, 0},       //
        {opcode_t::CMP_NEQ, 1, 0, 0, 0, 0, 0},      //
        {opcode_t::MEM_LOAD, 1, 0, 1, 1, 0, 0},     //
        {opcode_t::MEM_STORE, 0, 1, 0, 0, 0, 0},    //
        {opcode_t::MEM_LOADI, 1, 0, 1, 1, 0, 0},    //
        {opcode_t::MEM_MOVE, 1, 0, 0, 1, 0, 0},     //
        {opcode_t::BR_JMP, 0, 0, 0, 0, 1, 0},       //
        {opcode_t::BR_BRT, 0, 0, 0, 0, 0, 1},       //
        {opcode_t::SYS_NOP, 0, 0, 0, 0, 0, 0},      //
        {opcode_t::SYS_HALT, 0, 0, 0, 0, 0, 0},     //
        {opcode_t::SYS_BREAK, 0, 0, 0, 0, 0, 0},    //
        {opcode_t::SYS_CALL, 0, 0, 0, 0, 0, 0},     //
        {opcode_t::RESERVED1, 0, 0, 0, 0, 0, 0},    //
        {opcode_t::RESERVED2, 0, 0, 0, 0, 0, 0},    //
    };

    for (const auto &[op, rw, mw, mtr, rtd, je, be] : test_cases) {
        env.run_test(op, rw, mw, mtr, rtd, je, be);
    }

    return 0;
}
