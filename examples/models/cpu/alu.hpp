/// @file alu.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple ALU (Arithmetic Logic Unit) implementation.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class alu_t : public digsim::module_t
{
public:
    // Input signals.
    digsim::input_t<bool> clk;       ///< Clock signal.
    digsim::input_t<bool> reset;     ///< Reset signal.
    digsim::input_t<bs_data_t> a;    ///< First operand.
    digsim::input_t<bs_data_t> b;    ///< Second operand.
    digsim::input_t<bs_opcode_t> op; ///< Operation code.

    // Output signals.
    digsim::output_t<bs_data_t> out;       ///< Output result.
    digsim::output_t<bs_data_t> remainder; ///< Remainder of division.
    digsim::output_t<bs_status_t> status;  ///< Status flags.

    /// @brief Constructor.
    /// @param _name Name of the module.
    alu_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , a("a", this)
        , b("b", this)
        , op("op", this)
        , out("out", this)
        , remainder("remainder", this)
        , status("status", this)
    {
        ADD_SENSITIVITY(alu_t, evaluate, clk, reset);
        ADD_CONSUMER(alu_t, evaluate, a, b, op);
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
    /// @brief Evaluate the ALU operation.
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

        // Get the input values.
        const auto a_val  = a.get();
        const auto b_val  = b.get();
        const auto op_val = op.get();
        const auto a_u    = a_val.to_ulong();
        const auto b_u    = b_val.to_ulong();
        const auto op_u   = op_val.to_ulong();

        // Prepare the output values.
        bs_data_t result  = 0;
        bs_data_t rem     = 0;
        bs_status_t flags = 0;

        switch (static_cast<opcode_t>(op_u)) {
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
        case opcode_t::ALU_ADD: {
            unsigned long sum = a_u + b_u;
            result            = sum;
            if (sum >= (1UL << ADDRESS_WIDTH)) {
                flags = FLAG_CARRY;
            }
            break;
        }
        case opcode_t::ALU_SUB: {
            long diff = static_cast<long>(a_u) - static_cast<long>(b_u);
            result    = static_cast<unsigned long>(diff);
            if (diff < 0) {
                flags = FLAG_OVERFLOW;
            }
            break;
        }
        case opcode_t::ALU_MUL: {
            unsigned long prod = a_u * b_u;
            if (prod >= (1UL << ADDRESS_WIDTH)) {
                flags  = FLAG_OVERFLOW;
                result = 0;
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
        case opcode_t::ALU_SHIFT_LEFT:
            result = (b_u >= ADDRESS_WIDTH) ? 0 : (a_u << b_u);
            break;
        case opcode_t::ALU_SHIFT_RIGHT:
            result = (b_u >= ADDRESS_WIDTH) ? 0 : (a_u >> b_u);
            break;
        case opcode_t::ALU_EQUAL:
            result = (a_u == b_u);
            break;
        case opcode_t::ALU_LT:
            result = (a_u < b_u);
            break;
        default:
            result = 0;
            break;
        }

        out.set(result);
        remainder.set(rem);
        status.set(flags);

        // Debugging output.
        digsim::debug(
            get_name(),
            "a: 0x{:04X}, b: 0x{:04X}, op: 0x{:04X} -> out: 0x{:04X}, remainder: 0x{:04X}, status: 0x{:04X}", a_u, b_u,
            op_u, result.to_ulong(), rem.to_ulong(), flags.to_ulong());
    }
};
