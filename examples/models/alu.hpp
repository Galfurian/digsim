/// @file alu.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple ALU (Arithmetic Logic Unit) implementation.

#pragma once

#include <digsim/digsim.hpp>

#include <bitset>
#include <iomanip>
#include <sstream>

template <size_t N> class alu_t : public digsim::module_t
{
public:
    /// @brief First operand.
    digsim::input_t<std::bitset<N>> a;
    /// @brief Second operand.
    digsim::input_t<std::bitset<N>> b;
    /// @brief Operation code.
    digsim::input_t<std::bitset<4>> op;
    /// @brief Clock signal.
    digsim::input_t<bool> clk;
    /// @brief Output result.
    digsim::output_t<std::bitset<N>> out;
    /// @brief Remainder of division.
    digsim::output_t<std::bitset<N>> remainder;
    /// @brief Status flags.
    digsim::output_t<std::bitset<4>> status;

    /// @brief Constructor.
    /// @param _name Name of the module.
    alu_t(const std::string &_name)
        : module_t(_name)
        , a("a")
        , b("b")
        , op("op")
        , clk("clk")
        , out("out")
        , remainder("remainder")
        , status("status")
    {
        ADD_SENSITIVITY(alu_t, evaluate, clk);
        ADD_CONSUMER(alu_t, evaluate, a, b, op);
        ADD_PRODUCER(alu_t, evaluate, out, remainder, status);
    }

private:
    /// @brief Status flags.
    enum status_flag_t {
        FLAG_CARRY    = 0, ///< Carry flag.
        FLAG_BORROW   = 1, ///< Borrow flag.
        FLAG_DIV_ZERO = 2, ///< Zero division flag.
        FLAG_OVERFLOW = 3  ///< Overflow flag.
    };

    /// @brief Evaluate the ALU operation.
    void evaluate()
    {
        // Only evaluate on rising edge.
        if (!clk.posedge()) {
            return;
        }

        const auto a_val  = a.get();
        const auto b_val  = b.get();
        const auto op_val = op.get();

        std::bitset<N> result = 0;
        std::bitset<N> rem    = 0;
        std::bitset<4> flags  = 0;

        const unsigned long a_u = a_val.to_ulong();
        const unsigned long b_u = b_val.to_ulong();

        switch (op_val.to_ulong()) {
        case 0:
            result = a_val & b_val;
            break; // AND
        case 1:
            result = a_val | b_val;
            break; // OR
        case 2:
            result = a_val ^ b_val;
            break; // XOR
        case 3:
            result = ~a_val;
            break; // NOT
        case 4: {  // ADD
            unsigned long sum = a_u + b_u;
            result            = sum;
            if (sum >= (1UL << N)) {
                flags = 1 << FLAG_CARRY;
            }
            break;
        }
        case 5: { // SUB
            long diff = static_cast<long>(a_u) - static_cast<long>(b_u);
            result    = diff;
            if (diff < 0) {
                flags = 1 << FLAG_OVERFLOW;
            }
            break;
        }
        case 6:
            result = a_u * b_u;
            break; // MUL
        case 7:    // DIV
            if (b_u == 0) {
                result = 0;
                flags  = 1 << FLAG_DIV_ZERO;
            } else {
                result = a_u / b_u;
            }
            break;
        case 8: // MOD
            if (b_u == 0) {
                rem   = 0;
                flags = 1 << FLAG_DIV_ZERO;
            } else {
                rem = a_u % b_u;
            }
            break;
        case 9:
            result = a_u << b_u;
            break; // SHL
        case 10:
            result = a_u >> b_u;
            break; // SHR
        case 11:
            result = (a_u == b_u);
            break; // EQUAL
        case 12:
            result = (a_u < b_u);
            break; // LESS THAN
        default:
            result = 0;
            break;
        }

        out.set(result);
        remainder.set(rem);
        status.set(flags);

        std::stringstream ss;
        ss << "a:" << a_val;
        ss << ", b:" << b_val;
        ss << ", op:" << op_val;
        ss << "-> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (+" << out.get_delay() << "t)";
        }
        ss << ", remainder:" << rem;
        if (remainder.get_delay() > 0) {
            ss << " (+" << remainder.get_delay() << "t)";
        }
        ss << ", status:" << flags;
        if (status.get_delay() > 0) {
            ss << " (+" << status.get_delay() << "t)";
        }
        digsim::info(get_name(), ss.str());
    }
};
