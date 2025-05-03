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
    digsim::input_t<bool> clk;             ///< Clock signal.
    digsim::input_t<bool> reset;           ///< Reset signal.
    digsim::input_t<bs_data_t> a;          ///< First operand.
    digsim::input_t<bs_data_t> b;          ///< Second operand.
    digsim::input_t<bs_opcode_t> opcode;   ///< Operation code.
    digsim::input_t<bs_phase_t> phase;     ///< Current pipeline phase.
    digsim::output_t<bs_data_t> out;       ///< Output result.
    digsim::output_t<bs_data_t> remainder; ///< Remainder of division.
    digsim::output_t<bs_status_t> status;  ///< Status flags.

    alu_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , a("a", this)
        , b("b", this)
        , opcode("opcode", this)
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
        FLAG_CMP_FALSE = (1 << 0), ///< The comparison result is false.
        FLAG_CMP_TRUE  = (1 << 1), ///< The comparison result is true.
        FLAG_CARRY     = (1 << 2), ///< The operation resulted in a carry.
        FLAG_BORROW    = (1 << 3), ///< The operation resulted in a borrow.
        FLAG_DIV_ZERO  = (1 << 4), ///< The division by zero occurred.
        FLAG_OVERFLOW  = (1 << 5), ///< The operation resulted in an overflow.
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
        const auto op_val = opcode.get();
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
            // Compute the sum using a wider type.
            auto sum = a_u + b_u;
            // Store the sum into result.
            result   = sum;
            // Check if carry-out occurred (i.e., bit DATA_WIDTH was set).
            if (sum >> DATA_WIDTH) {
                flags = FLAG_CARRY;
            }
            break;
        }
        case opcode_t::ALU_SUB: {
            // Perform the subtraction in a wider type
            auto diff = static_cast<int32_t>(a_u) - static_cast<int32_t>(b_u);
            // Assign the truncated result to the 16-bit result
            result    = static_cast<uint32_t>(diff);
            // Detect borrow (i.e., if unsigned a < b)
            if (a_u < b_u) {
                flags = FLAG_BORROW;
            }
            break;
        }
        case opcode_t::ALU_MUL: {
            // Perform the multiplication in a wider type.
            auto prod = a_u * b_u;
            // If the higher bits of the product are set, it means overflow occurred.
            if (prod >> DATA_WIDTH) {
                result = 0;
                flags  = FLAG_OVERFLOW;
            } else {
                result = prod;
            }
            break;
        }
        case opcode_t::ALU_DIV: {
            if (b_u == 0) {
                result = 0;
                rem    = 0;
                flags  = FLAG_DIV_ZERO;
            } else {
                result = a_u / b_u;
                rem    = a_u % b_u;
            }
            break;
        }

        // Shift
        case opcode_t::SHIFT_LEFT:
            result = (b_u >= DATA_WIDTH) ? 0 : (a_u << b_u);
            break;
        case opcode_t::SHIFT_RIGHT:
            result = (b_u >= DATA_WIDTH) ? 0 : (a_u >> b_u);
            break;

        // Comparison.
        case opcode_t::CMP_EQ:
            flags  = (a_u == b_u) ? FLAG_CMP_TRUE : FLAG_CMP_FALSE;
            result = (flags == FLAG_CMP_TRUE);
            break;
        case opcode_t::CMP_NEQ:
            flags  = (a_u != b_u) ? FLAG_CMP_TRUE : FLAG_CMP_FALSE;
            result = (flags == FLAG_CMP_TRUE);
            break;
        case opcode_t::CMP_LT:
            flags  = (a_u < b_u) ? FLAG_CMP_TRUE : FLAG_CMP_FALSE;
            result = (flags == FLAG_CMP_TRUE);
            break;
        case opcode_t::CMP_GT:
            flags  = (a_u > b_u) ? FLAG_CMP_TRUE : FLAG_CMP_FALSE;
            result = (flags == FLAG_CMP_TRUE);
            break;

        case opcode_t::BR_BRT:
            if (a_u != 0) {
                result = b_u;
                flags  = FLAG_CMP_TRUE;
            } else {
                result = 0;
                flags  = FLAG_CMP_FALSE;
            }
            break;

        case opcode_t::BR_JMP:
            result = b_u;
            flags  = FLAG_CMP_TRUE;
            break;

        // MEM passthrough ops (if you still want to support them here)
        case opcode_t::MEM_LOAD:
        case opcode_t::MEM_STORE:
        case opcode_t::MEM_LOADI:
            result = a_val;
            break;

        case opcode_t::MEM_MOVE:
            result = a_val;
            break;

        case opcode_t::SYS_HALT:
            flags = 0xFFFF;
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
            get_name(), "a: {}, b: {}, opcode: {} ({:15}) -> out: {}, remainder: {}, status: {}", a.get(), b.get(),
            opcode.get(), opcode_to_string(static_cast<opcode_t>(op_u)), out.get(), out.get(), status.get());
    }
};