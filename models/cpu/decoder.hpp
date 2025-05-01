/// @file decoder.hpp
/// @brief

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class decoder_t : public digsim::module_t
{
public:
    digsim::input_t<bs_instruction_t> instruction; ///< Raw instruction input.
    digsim::input_t<bs_phase_t> phase;             ///< Phase of execution.
    digsim::output_t<bs_opcode_t> opcode;          ///< Output: decoded opcode.
    digsim::output_t<bs_register_t> rs;            ///< Output: source register A.
    digsim::output_t<bs_register_t> rt;            ///< Output: source register B.
    digsim::output_t<bs_register_t> rd;            ///< Output: destination register.

    decoder_t(const std::string &_name)
        : digsim::module_t(_name)
        , instruction("instruction", this)
        , phase("phase", this)
        , opcode("opcode", this)
        , rs("rs", this)
        , rt("rt", this)
        , rd("rd", this)
    {
        ADD_SENSITIVITY(decoder_t, evaluate, instruction, phase);
        ADD_PRODUCER(decoder_t, evaluate, opcode, rs, rt, rd);
    }

private:
    void evaluate()
    {
        // [15:13] opcode
        // [12:9]  func
        // [8:5]   rs
        // [4:1]   rt or imm
        // [0]     unused (optional flag)

        const auto current_phase = static_cast<phase_t>(phase.get().to_ulong());
        if (current_phase != phase_t::DECODE)
            return;

        uint16_t raw = static_cast<uint16_t>(instruction.get().to_ulong());
        uint8_t opcode_val, rs_val, rt_val, rd_val;

        // Decode the instruction.
        decode_instruction(raw, opcode_val, rs_val, rt_val, rd_val);

        // For simplicity, treat all as general-purpose R-type for now
        opcode.set(opcode_val);
        rs.set(rs_val);
        rt.set(rt_val);
        rd.set(rd_val);

        digsim::debug(
            get_name(),
            "Decoded instruction 0x{:04X} -> opcode: 0x{:04X} [{:16}], rs: 0x{:04X}, rt: 0x{:04X}, rd: 0x{:04X}", raw,
            opcode_val, opcode_to_string(opcode_val), rs_val, rt_val, rd_val);
    }
};