/// @file control_unit.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple control unit for a CPU.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

/// @brief A simple control unit for a 4-phase CPU pipeline.
class control_unit_t : public digsim::module_t
{
public:
    digsim::input_t<bs_opcode_t> opcode; ///< Decoded operation code.
    digsim::input_t<bs_phase_t> phase;   ///< Current CPU phase.

    digsim::output_t<bs_opcode_t> alu_op; ///< ALU operation code.
    digsim::output_t<bool> reg_write;     ///< Register file write enable.
    digsim::output_t<bool> mem_write;     ///< Memory write enable.
    digsim::output_t<bool> mem_to_reg;    ///< Write-back select: memory or ALU.
    digsim::output_t<bool> rt_as_dest;    ///< Register file write destination: rt or rd.

    control_unit_t(const std::string &_name)
        : module_t(_name)
        , opcode("opcode", this)
        , phase("phase", this)
        , alu_op("alu_op", this)
        , reg_write("reg_write", this)
        , mem_write("mem_write", this)
        , mem_to_reg("mem_to_reg", this)
        , rt_as_dest("rt_as_dest", this)
    {
        ADD_SENSITIVITY(control_unit_t, evaluate, phase); // Only reacts on phase change
        ADD_PRODUCER(control_unit_t, evaluate, alu_op, reg_write, mem_write, mem_to_reg, rt_as_dest);
    }

private:
    void evaluate()
    {
        const auto in_op         = static_cast<opcode_t>(opcode.get().to_ulong());
        const auto current_phase = static_cast<phase_t>(phase.get().to_ulong());

        // Always set ALU operation.
        alu_op.set(opcode.get());

        switch (current_phase) {
        case phase_t::FETCH:
            // PC should increment after FETCH
            reg_write.set(false);
            mem_write.set(false);
            mem_to_reg.set(false);
            rt_as_dest.set(false);
            break;

        case phase_t::DECODE:
            reg_write.set(false);
            mem_write.set(false);
            mem_to_reg.set(false);
            rt_as_dest.set(false);
            break;

        case phase_t::EXECUTE:
            reg_write.set(false);
            mem_write.set(false);
            mem_to_reg.set(false);
            rt_as_dest.set(false);
            break;

        case phase_t::WRITEBACK:
            switch (in_op) {
            // ALU results go to register
            case opcode_t::ALU_ADD:
            case opcode_t::ALU_SUB:
            case opcode_t::ALU_AND:
            case opcode_t::ALU_OR:
            case opcode_t::ALU_XOR:
            case opcode_t::ALU_NOT:
            case opcode_t::ALU_MUL:
            case opcode_t::ALU_DIV:
            case opcode_t::SHIFT_LEFT:
            case opcode_t::SHIFT_RIGHT:
            case opcode_t::CMP_EQ:
            case opcode_t::CMP_LT:
            case opcode_t::CMP_GT:
            case opcode_t::CMP_NEQ:
                reg_write.set(true);
                mem_write.set(false);
                mem_to_reg.set(false); // ALU → reg
                rt_as_dest.set(false);
                break;

            // LOAD: write from memory to reg
            case opcode_t::MEM_LOAD:
            case opcode_t::MEM_LOADI:
                reg_write.set(true);
                mem_write.set(false);
                mem_to_reg.set(true);
                rt_as_dest.set(true);
                break;

            // STORE: write reg to memory
            case opcode_t::MEM_STORE:
                reg_write.set(false);
                mem_write.set(true);
                mem_to_reg.set(false);
                rt_as_dest.set(false);
                break;

            // Others: no write-back
            default:
                reg_write.set(false);
                mem_write.set(false);
                mem_to_reg.set(false);
                rt_as_dest.set(false);
                break;
            }
            break;
        }

        digsim::debug(
            get_name(),
            "{:9}: opcode 0x{:04X} ({:12}) -> reg_write: {}, mem_write: {}, mem_to_reg: {}, rt_as_dest: {}, alu_op: "
            "0x{:04X}",
            phase_to_string(current_phase), opcode.get().to_ulong(), opcode_to_string(in_op), reg_write.get(),
            mem_write.get(), mem_to_reg.get(), rt_as_dest.get(), alu_op.get().to_ulong());
    }
};
