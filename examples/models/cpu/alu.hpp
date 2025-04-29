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

    /// @brief The list of operations supported by the ALU.
    enum class op_t {
        /// @brief ALU operations.
        AND,         ///< AND
        OR,          ///< OR
        XOR,         ///< XOR
        NOT,         ///< NOT
        ADD,         ///< ADD
        SUB,         ///< SUB
        MUL,         ///< MUL
        DIV,         ///< DIV
        MOD,         ///< MOD
        SHIFT_LEFT,  ///< SHIFT LEFT
        SHIFT_RIGHT, ///< SHIFT RIGHT
        EQUAL,       ///< EQUAL
        LT,          ///< LESS THAN
        GT,          ///< GREATER THAN
    };

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

        const auto a_val  = a.get();
        const auto b_val  = b.get();
        const auto op_val = op.get();

        std::bitset<N> result = 0;
        std::bitset<N> rem    = 0;
        std::bitset<4> flags  = 0;

        const unsigned long a_u = a_val.to_ulong();
        const unsigned long b_u = b_val.to_ulong();

        switch (static_cast<op_t>(op_val.to_ulong())) {
        case op_t::AND:
            result = a_val & b_val;
            break;
        case op_t::OR:
            result = a_val | b_val;
            break;
        case op_t::XOR:
            result = a_val ^ b_val;
            break;
        case op_t::NOT:
            result = ~a_val;
            break;
        case op_t::ADD: {
            unsigned long sum = a_u + b_u;
            result            = sum;
            if (sum >= (1UL << N)) {
                flags = FLAG_CARRY;
            }
            break;
        }
        case op_t::SUB: {
            long diff = static_cast<long>(a_u) - static_cast<long>(b_u);
            result    = static_cast<unsigned long>(diff);
            if (diff < 0) {
                flags = FLAG_OVERFLOW;
            }
            break;
        }
        case op_t::MUL: {
            unsigned long prod = a_u * b_u;
            if (prod >= (1UL << N)) {
                flags  = FLAG_OVERFLOW;
                result = 0;
            } else {
                result = prod;
            }
            break;
        }
        case op_t::DIV:
            if (b_u == 0) {
                result = 0;
                rem    = 0;
                flags  = FLAG_DIV_ZERO;
            } else {
                result = a_u / b_u;
                rem    = a_u % b_u;
            }
            break;
        case op_t::MOD:
            if (b_u == 0) {
                rem   = 0;
                flags = FLAG_DIV_ZERO;
            } else {
                rem = a_u % b_u;
            }
            break;
        case op_t::SHIFT_LEFT:
            result = (b_u >= N) ? 0 : (a_u << b_u);
            break;
        case op_t::SHIFT_RIGHT:
            result = (b_u >= N) ? 0 : (a_u >> b_u);
            break;
        case op_t::EQUAL:
            result = (a_u == b_u);
            break;
        case op_t::LT:
            result = (a_u < b_u);
            break;
        case op_t::GT:
            result = (a_u > b_u);
            break;
        default:
            result = 0;
            break;
        }

        out.set(result);
        remainder.set(rem);
        status.set(flags);

        // Debugging output.
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
        digsim::debug(get_name(), ss.str());
    }
};
