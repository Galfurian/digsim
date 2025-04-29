/// @file control_unit.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple control unit for a CPU.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

/// @brief A simple control unit for a CPU.
/// @tparam N The size of the instruction word.
class control_unit_t : public digsim::module_t
{
public:
    digsim::input_t<bs_opcode_t> opcode;  ///< Decoded operation code.
    digsim::output_t<bs_opcode_t> alu_op; ///< ALU operation code.
    digsim::output_t<bool> reg_write;     ///< Register file write enable.
    digsim::output_t<bool> mem_write;     ///< Memory write enable.
    digsim::output_t<bool> mem_to_reg;    ///< Write-back select: memory or ALU.

    /// @brief Constructor for the control unit.
    /// @param _name The name of the module.
    control_unit_t(const std::string &_name)
        : module_t(_name)
        , opcode("opcode", this)
        , alu_op("alu_op", this)
        , reg_write("reg_write", this)
        , mem_write("mem_write", this)
        , mem_to_reg("mem_to_reg", this)
    {
        ADD_SENSITIVITY(control_unit_t, evaluate, opcode);
        ADD_PRODUCER(control_unit_t, evaluate, alu_op, reg_write, mem_write, mem_to_reg);
    }

private:
    /// @brief Evaluate the control unit.
    void evaluate()
    {
        opcode_t in_op = static_cast<opcode_t>(opcode.get().to_ulong());

        switch (in_op) {
        case opcode_t::ALU_AND:
        case opcode_t::ALU_OR:
        case opcode_t::ALU_XOR:
        case opcode_t::ALU_NOT:
        case opcode_t::ALU_ADD:
        case opcode_t::ALU_SUB:
        case opcode_t::ALU_MUL:
        case opcode_t::ALU_DIV:
        case opcode_t::ALU_SHIFT_LEFT:
        case opcode_t::ALU_SHIFT_RIGHT:
        case opcode_t::ALU_EQUAL:
        case opcode_t::ALU_LT:
            reg_write.set(true);
            mem_write.set(false);
            mem_to_reg.set(false);
            break;
        case opcode_t::STORE:
            reg_write.set(false);
            mem_write.set(true);
            mem_to_reg.set(false);
            break;
        case opcode_t::LOAD:
            reg_write.set(true);
            mem_write.set(false);
            mem_to_reg.set(true);
            break;
        case opcode_t::NOP:
            reg_write.set(false);
            mem_write.set(false);
            mem_to_reg.set(false);
            break;
        case opcode_t::ILLEGAL:
            reg_write.set(false);
            mem_write.set(false);
            mem_to_reg.set(false);
            break;
        default:
            reg_write.set(false);
            mem_write.set(false);
            mem_to_reg.set(false);
            break;
        }
        // Set the ALU operation code.
        alu_op.set(opcode.get());

        digsim::debug(
            get_name(), "opcode {} ({:12}) -> reg_write: {}, mem_write: {}, mem_to_reg: {}, alu_op: 0x{:02X}",
            opcode.get(), opcode_to_string(in_op), reg_write.get(), mem_write.get(), mem_to_reg.get(),
            alu_op.get().to_ulong());
    }
};

