/// @file test_alu.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/alu.hpp"

static int test_result = 0;

struct alu_env_t {
    digsim::signal_t<bool> clk{"clk", 0, 0};
    digsim::signal_t<bool> reset{"reset", 0, 0};
    digsim::signal_t<bs_data_t> a{"a", 0, 0};
    digsim::signal_t<bs_data_t> b{"b", 0, 0};
    digsim::signal_t<bs_opcode_t> opcode{"opcode", 0, 0};
    digsim::signal_t<bs_phase_t> phase{"phase", static_cast<uint16_t>(phase_t::EXECUTE), 0};
    digsim::signal_t<bs_data_t> out{"out", 0, 0};
    digsim::signal_t<bs_data_t> rem{"rem", 0, 0};
    digsim::signal_t<bs_status_t> status{"status", 0, 0};
    alu_t alu{"alu"};

    alu_env_t()
    {
        alu.clk(clk);
        alu.reset(reset);
        alu.a(a);
        alu.b(b);
        alu.opcode(opcode);
        alu.phase(phase);
        alu.out(out);
        alu.remainder(rem);
        alu.status(status);
    }

    void toggle_clock()
    {
        clk.set(false);
        digsim::scheduler.run(); // Falling edge
        clk.set(true);
        digsim::scheduler.run(); // Rising edge
    }
};

void run_alu_test(
    alu_env_t &env,
    const std::string &label,
    bs_data_t a_val,
    bs_data_t b_val,
    bs_opcode_t opcode,
    bs_data_t expected_out,
    bs_data_t expected_rem      = 0,
    bs_status_t expected_status = 0)
{
    env.a.set(a_val);
    env.b.set(b_val);
    env.opcode.set(opcode);
    env.toggle_clock();

    if (env.out.get() != expected_out) {
        digsim::error(
            "ALU Test", "{}: out mismatch (got 0x{:X}, expected 0x{:X})", label, env.out.get().to_ulong(),
            expected_out.to_ulong());
        test_result = 1;
    }
    if (env.rem.get() != expected_rem) {
        digsim::error(
            "ALU Test", "{}: rem mismatch (got 0x{:X}, expected 0x{:X})", label, env.rem.get().to_ulong(),
            expected_rem.to_ulong());
        test_result = 1;
    }
    if (env.status.get() != expected_status) {
        digsim::error(
            "ALU Test", "{}: status mismatch (got 0x{:X}, expected 0x{:X})", label, env.status.get().to_ulong(),
            expected_status.to_ulong());
        test_result = 1;
    }
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);
    alu_env_t env;

    // Logic
    run_alu_test(env, "AND", 0xC, 0xA, ALU_AND, 0x8);
    run_alu_test(env, "OR", 0xC, 0xA, ALU_OR, 0xE);
    run_alu_test(env, "XOR", 0xC, 0xA, ALU_XOR, 0x6);
    run_alu_test(env, "NOT", 0xC, 0x0, ALU_NOT, ~bs_data_t(0xC));

    // Arithmetic
    run_alu_test(env, "ADD", 6, 3, ALU_ADD, 9);
    run_alu_test(env, "SUB", 6, 3, ALU_SUB, 3);
    run_alu_test(env, "SUB (underflow)", 3, 6, ALU_SUB, 0xFFFD, 0, alu_t::FLAG_OVERFLOW);
    run_alu_test(env, "MUL", 3, 3, ALU_MUL, 9);
    run_alu_test(env, "MUL (overflow)", 0x1000, 0x1000, ALU_MUL, 0, 0, alu_t::FLAG_OVERFLOW);
    run_alu_test(env, "ADD (overflow)", 0xFFFF, 0x1, ALU_ADD, 0x0, 0, alu_t::FLAG_CARRY);
    run_alu_test(env, "DIV", 9, 2, ALU_DIV, 4, 1);
    run_alu_test(env, "DIV by 0", 7, 0, ALU_DIV, 0, 0, alu_t::FLAG_DIV_ZERO);

    // Shifts
    run_alu_test(env, "SHL", 0x1, 0x2, SHIFT_LEFT, 0x4);
    run_alu_test(env, "SHR", 0x8, 0x3, SHIFT_RIGHT, 0x1);
    run_alu_test(env, "SHL (overflow bitcount)", 0x1, 32, SHIFT_LEFT, 0x0);
    run_alu_test(env, "SHR (overflow bitcount)", 0x8000, 32, SHIFT_RIGHT, 0x0);

    // Comparison
    run_alu_test(env, "EQ true", 0x5, 0x5, CMP_EQ, 1, 0, alu_t::FLAG_CMP_TRUE);
    run_alu_test(env, "EQ false", 0x3, 0x6, CMP_EQ, 0, 0, alu_t::FLAG_CMP_FALSE);
    run_alu_test(env, "LT true", 0x3, 0x6, CMP_LT, 1, 0, alu_t::FLAG_CMP_TRUE);
    run_alu_test(env, "LT false", 0xE, 0x2, CMP_LT, 0, 0, alu_t::FLAG_CMP_FALSE);
    run_alu_test(env, "GT true", 0xF, 0x1, CMP_GT, 1, 0, alu_t::FLAG_CMP_TRUE);
    run_alu_test(env, "GT false", 0x1, 0xF, CMP_GT, 0, 0, alu_t::FLAG_CMP_FALSE);
    run_alu_test(env, "NEQ true", 0xAAAA, 0x5555, CMP_NEQ, 1, 0, alu_t::FLAG_CMP_TRUE);
    run_alu_test(env, "NEQ false", 0xDEAD, 0xDEAD, CMP_NEQ, 0, 0, alu_t::FLAG_CMP_FALSE);

    return test_result;
}
