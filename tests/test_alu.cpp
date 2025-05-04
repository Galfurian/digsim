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

    void run_test(
        const std::string &label,
        bs_data_t in_a,
        bs_data_t in_b,
        bs_opcode_t in_opcode,
        bs_data_t expected_out,
        bs_data_t expected_rem      = 0,
        bs_status_t expected_status = 0)
    {
        // Set the stimuli.
        a.set(in_a);
        b.set(in_b);
        opcode.set(in_opcode);
        // Toggle the clock to trigger the ALU evaluation.
        toggle_clock();
        // Read the output values.
        auto got_out    = out.get().to_ulong();
        auto got_rem    = rem.get().to_ulong();
        auto got_status = status.get().to_ulong();
        // Get the expected output values.
        auto exp_out    = expected_out.to_ulong();
        auto exp_rem    = expected_rem.to_ulong();
        auto exp_status = expected_status.to_ulong();
        // Check the output values.
        if (got_out != exp_out) {
            digsim::error("ALU Test", "{}: out mismatch    (got 0x{:04X}, expected 0x{:04X})", label, got_out, exp_out);
            test_result = 1;
        }
        if (got_rem != exp_rem) {
            digsim::error("ALU Test", "{}: rem mismatch    (got 0x{:04X}, expected 0x{:04X})", label, got_rem, exp_rem);
            test_result = 1;
        }
        if (got_status != exp_status) {
            digsim::error("ALU Test", "{}: status mismatch (got 0x{:04X}, expected 0x{:04X})", label, got_status, exp_status);
            test_result = 1;
        }
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);
    alu_env_t env;

    // Logic
    env.run_test("AND", 0xC, 0xA, ALU_AND, 0x8);
    env.run_test("OR", 0xC, 0xA, ALU_OR, 0xE);
    env.run_test("XOR", 0xC, 0xA, ALU_XOR, 0x6);
    env.run_test("NOT", 0xC, 0x0, ALU_NOT, ~bs_data_t(0xC));

    // Arithmetic
    env.run_test("ADD", 6, 3, ALU_ADD, 9);
    env.run_test("SUB", 6, 3, ALU_SUB, 3);
    env.run_test("SUB (underflow)", 3, 6, ALU_SUB, 0xFFFD, 0, alu_t::FLAG_BORROW);
    env.run_test("MUL", 3, 3, ALU_MUL, 9);
    env.run_test("MUL (overflow)", 0x1000, 0x1000, ALU_MUL, 0, 0, alu_t::FLAG_OVERFLOW);
    env.run_test("ADD (overflow)", 0xFFFF, 0x1, ALU_ADD, 0x0, 0, alu_t::FLAG_CARRY);
    env.run_test("DIV", 9, 2, ALU_DIV, 4, 1);
    env.run_test("DIV by 0", 7, 0, ALU_DIV, 0, 0, alu_t::FLAG_DIV_ZERO);

    // Shifts
    env.run_test("SHL", 0x1, 0x2, SHIFT_LEFT, 0x4);
    env.run_test("SHR", 0x8, 0x3, SHIFT_RIGHT, 0x1);
    env.run_test("SHL (overflow bitcount)", 0x1, 32, SHIFT_LEFT, 0x0);
    env.run_test("SHR (overflow bitcount)", 0x8000, 32, SHIFT_RIGHT, 0x0);

    // Comparison
    env.run_test("EQ true", 0x5, 0x5, CMP_EQ, 1, 0, alu_t::FLAG_CMP_TRUE);
    env.run_test("EQ false", 0x3, 0x6, CMP_EQ, 0, 0, alu_t::FLAG_CMP_FALSE);
    env.run_test("LT true", 0x3, 0x6, CMP_LT, 1, 0, alu_t::FLAG_CMP_TRUE);
    env.run_test("LT false", 0xE, 0x2, CMP_LT, 0, 0, alu_t::FLAG_CMP_FALSE);
    env.run_test("GT true", 0xF, 0x1, CMP_GT, 1, 0, alu_t::FLAG_CMP_TRUE);
    env.run_test("GT false", 0x1, 0xF, CMP_GT, 0, 0, alu_t::FLAG_CMP_FALSE);
    env.run_test("NEQ true", 0xAAAA, 0x5555, CMP_NEQ, 1, 0, alu_t::FLAG_CMP_TRUE);
    env.run_test("NEQ false", 0xDEAD, 0xDEAD, CMP_NEQ, 0, 0, alu_t::FLAG_CMP_FALSE);

    return test_result;
}
