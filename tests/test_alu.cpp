/// @file test_alu.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/alu.hpp"

void toggle_clock(digsim::signal_t<bool> &clk)
{
    clk.set(false);
    digsim::scheduler.run(); // Falling edge
    clk.set(true);
    digsim::scheduler.run(); // Rising edge
}

int run_alu_test(
    const std::string &label,
    digsim::signal_t<bs_data_t> &a,
    digsim::signal_t<bs_data_t> &b,
    digsim::signal_t<bs_opcode_t> &op,
    digsim::signal_t<bs_data_t> &out,
    digsim::signal_t<bs_data_t> &remainder,
    digsim::signal_t<bs_status_t> &status,
    digsim::signal_t<bool> &clk,
    bs_data_t a_val,
    bs_data_t b_val,
    opcode_t op_val,
    bs_data_t expected_out,
    bs_data_t expected_rem      = 0,
    bs_status_t expected_status = 0)
{
    a.set(a_val);
    b.set(b_val);
    op.set(static_cast<unsigned long>(op_val));
    toggle_clock(clk);

    if (out.get() != expected_out) {
        digsim::error(
            "ALU Test", "{}: out mismatch (got 0x{:X}, expected 0x{:X})", label, out.get().to_ulong(),
            expected_out.to_ulong());
        return 1;
    }
    if (remainder.get() != expected_rem) {
        digsim::error(
            "ALU Test", "{}: remainder mismatch (got 0x{:X}, expected 0x{:X})", label, remainder.get().to_ulong(),
            expected_rem.to_ulong());
        return 1;
    }
    if (status.get() != expected_status) {
        digsim::error(
            "ALU Test", "{}: status mismatch (got 0x{:X}, expected 0x{:X})", label, status.get().to_ulong(),
            expected_status.to_ulong());
        return 1;
    }
    return 0;
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    digsim::signal_t<bool> clk("clk", 0, 0);
    digsim::signal_t<bool> reset("reset", 0, 0);
    digsim::signal_t<bs_data_t> a("a", 0, 0);
    digsim::signal_t<bs_data_t> b("b", 0, 0);
    digsim::signal_t<bs_opcode_t> op("op", 0, 0);
    digsim::signal_t<bs_phase_t> phase("phase", static_cast<uint16_t>(phase_t::EXECUTE), 0);
    digsim::signal_t<bs_data_t> out("out", 0, 0);
    digsim::signal_t<bs_data_t> remainder("remainder", 0, 0);
    digsim::signal_t<bs_status_t> status("status", 0, 0);

    // Create the ALU.
    alu_t alu0("alu0");
    alu0.clk(clk);
    alu0.reset(reset);
    alu0.a(a);
    alu0.b(b);
    alu0.op(op);
    alu0.phase(phase);
    alu0.out(out);
    alu0.remainder(remainder);
    alu0.status(status);

    // Logic operations
    run_alu_test("AND", a, b, op, out, remainder, status, clk, 0b1100, 0b1010, opcode_t::ALU_AND, 0b1000);
    run_alu_test("OR", a, b, op, out, remainder, status, clk, 0b1100, 0b1010, opcode_t::ALU_OR, 0b1110);
    run_alu_test("XOR", a, b, op, out, remainder, status, clk, 0b1100, 0b1010, opcode_t::ALU_XOR, 0b0110);
    run_alu_test("NOT", a, b, op, out, remainder, status, clk, 0b1100, 0, opcode_t::ALU_NOT, ~bs_data_t(0b1100));

    // Arithmetic
    run_alu_test("ADD", a, b, op, out, remainder, status, clk, 6, 3, opcode_t::ALU_ADD, 9);
    run_alu_test("SUB", a, b, op, out, remainder, status, clk, 6, 3, opcode_t::ALU_SUB, 3);
    run_alu_test(
        "SUB (underflow)", a, b, op, out, remainder, status, clk, 3, 6, opcode_t::ALU_SUB, 0xFFFD, 0,
        alu_t::FLAG_OVERFLOW);
    run_alu_test("MUL", a, b, op, out, remainder, status, clk, 3, 3, opcode_t::ALU_MUL, 9);
    run_alu_test("DIV", a, b, op, out, remainder, status, clk, 7, 3, opcode_t::ALU_DIV, 2, 1);
    run_alu_test(
        "DIV by 0", a, b, op, out, remainder, status, clk, 7, 0, opcode_t::ALU_DIV, 0, 0, alu_t::FLAG_DIV_ZERO);

    // Shift operations
    run_alu_test("SHL", a, b, op, out, remainder, status, clk, 0b0001, 2, opcode_t::ALU_SHIFT_LEFT, 0b0100);
    run_alu_test("SHR", a, b, op, out, remainder, status, clk, 0b1000, 3, opcode_t::ALU_SHIFT_RIGHT, 0b0001);

    // Comparison
    run_alu_test("EQ true", a, b, op, out, remainder, status, clk, 0b0101, 0b0101, opcode_t::ALU_EQUAL, 1);
    run_alu_test("EQ false", a, b, op, out, remainder, status, clk, 0b0011, 0b0110, opcode_t::ALU_EQUAL, 0);
    run_alu_test("LT true", a, b, op, out, remainder, status, clk, 0b0011, 0b0110, opcode_t::ALU_LT, 1);
    run_alu_test("LT false", a, b, op, out, remainder, status, clk, 0b1110, 0b0010, opcode_t::ALU_LT, 0);

    return 0;
}
