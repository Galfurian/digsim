/// @file alu.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple ALU (Arithmetic Logic Unit) implementation.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

/// @brief ALU (Arithmetic Logic Unit) supporting 4-phase CPU pipeline.
class alu_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;         ///< Clock signal.
    digsim::input_t<bool> reset;       ///< Reset signal.
    digsim::input_t<bs_data_t> a;      ///< First operand.
    digsim::input_t<bs_data_t> b;      ///< Second operand.
    digsim::input_t<bs_opcode_t> op;   ///< Operation code.
    digsim::input_t<bs_phase_t> phase; ///< Current pipeline phase.

    digsim::output_t<bs_data_t> out;       ///< Output result.
    digsim::output_t<bs_data_t> remainder; ///< Remainder of division.
    digsim::output_t<bs_status_t> status;  ///< Status flags.

    alu_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , a("a", this)
        , b("b", this)
        , op("op", this)
        , phase("phase", this)
        , out("out", this)
        , remainder("remainder", this)
        , status("status", this)
    {
        ADD_SENSITIVITY(alu_t, evaluate, clk, reset);
        ADD_PRODUCER(alu_t, evaluate, out, remainder, status);
    }

    /// @brief Status flags.
    enum status_flag_t {
        FLAG_CARRY    = (1 << 0), ///< Carry flag.
        FLAG_BORROW   = (1 << 1), ///< Borrow flag.
        FLAG_DIV_ZERO = (1 << 2), ///< Zero division flag.
        FLAG_OVERFLOW = (1 << 3), ///< Overflow flag.
    };

private:
    /// @brief Evaluate the ALU operation, only during EXECUTE phase.
    void evaluate()
    {
        // Only evaluate on rising edge.
        if (!clk.posedge()) {
            return;
        }

        // Reset the ALU.
        if (reset.get()) {
            out.set(0);
            remainder.set(0);
            status.set(0);
            return;
        }

        // Only perform ALU operation in EXECUTE phase.
        if (static_cast<phase_t>(phase.get().to_ulong()) != phase_t::EXECUTE) {
            return;
        }

        const auto a_val  = a.get();
        const auto b_val  = b.get();
        const auto op_val = op.get();
        const auto a_u    = a_val.to_ulong();
        const auto b_u    = b_val.to_ulong();
        const auto op_u   = op_val.to_ulong();

        bs_data_t result  = 0;
        bs_data_t rem     = 0;
        bs_status_t flags = 0;

        switch (static_cast<opcode_t>(op_u)) {
        // Logic
        case opcode_t::ALU_AND:
            result = a_val & b_val;
            break;
        case opcode_t::ALU_OR:
            result = a_val | b_val;
            break;
        case opcode_t::ALU_XOR:
            result = a_val ^ b_val;
            break;
        case opcode_t::ALU_NOT:
            result = ~a_val;
            break;

        // Arithmetic
        case opcode_t::ALU_ADD: {
            auto sum = a_u + b_u;
            result   = sum;
            if (sum >= (1UL << ADDRESS_WIDTH))
                flags = FLAG_CARRY;
            break;
        }
        case opcode_t::ALU_SUB: {
            auto diff = static_cast<long>(a_u) - static_cast<long>(b_u);
            result    = static_cast<unsigned long>(diff);
            if (diff < 0)
                flags = FLAG_OVERFLOW;
            break;
        }
        case opcode_t::ALU_MUL: {
            auto prod = a_u * b_u;
            if (prod >= (1UL << ADDRESS_WIDTH)) {
                result = 0;
                flags  = FLAG_OVERFLOW;
            } else {
                result = prod;
            }
            break;
        }
        case opcode_t::ALU_DIV:
            if (b_u == 0) {
                result = 0;
                rem    = 0;
                flags  = FLAG_DIV_ZERO;
            } else {
                result = a_u / b_u;
                rem    = a_u % b_u;
            }
            break;

        // Shift
        case opcode_t::SHIFT_LEFT:
            result = (b_u >= ADDRESS_WIDTH) ? 0 : (a_u << b_u);
            break;
        case opcode_t::SHIFT_RIGHT:
            result = (b_u >= ADDRESS_WIDTH) ? 0 : (a_u >> b_u);
            break;

        // Comparison
        case opcode_t::CMP_EQ:
            result = (a_u == b_u);
            break;
        case opcode_t::CMP_LT:
            result = (a_u < b_u);
            break;
        case opcode_t::CMP_GT:
            result = (a_u > b_u);
            break;
        case opcode_t::CMP_NEQ:
            result = (a_u != b_u);
            break;

        // MEM passthrough ops (if you still want to support them here)
        case opcode_t::MEM_LOAD:
        case opcode_t::MEM_STORE:
        case opcode_t::MEM_LOADI:
            result = a_val;
            break;

        // Default
        default:
            result = 0;
            break;
        }

        out.set(result);
        remainder.set(rem);
        status.set(flags);

        digsim::debug(
            get_name(), "a: {}, b: {}, op: {} ({:15}) -> out: {}, remainder: {}, status: {}", a.get(), b.get(),
            op.get(), opcode_to_string(static_cast<opcode_t>(op_u)), out.get(), out.get(), status.get());
    }
};