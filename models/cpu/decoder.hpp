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
        const auto current_phase = static_cast<phase_t>(phase.get().to_ulong());
        if (current_phase != phase_t::DECODE)
            return;

        uint16_t instr = static_cast<uint16_t>(instruction.get().to_ulong());

        opcode_t op    = static_cast<opcode_t>((instr >> 12) & 0xF);
        uint16_t rsrc1 = (instr >> 8) & 0xF;
        uint16_t rsrc2 = (instr >> 4) & 0xF;
        uint16_t rdest = instr & 0xF;

        opcode.set(static_cast<uint8_t>(op));

        switch (op) {
        case opcode_t::STORE:
            // rs = source data, rd = memory address
            rs.set(rdest); // r3: memory address
            rt.set(rsrc1); // r2: data to store
            rd.set(0);     // not used
            break;
        case opcode_t::LOAD:
            // rs = memory address, rd = destination
            rs.set(rdest); // r3: address
            rt.set(0);     // unused
            rd.set(rsrc1); // r4: destination register
            break;
        default:
            // Default R-type style
            rs.set(rsrc1);
            rt.set(rsrc2);
            rd.set(rdest);
            break;
        }

        digsim::debug(
            get_name(), "Decoded instruction 0x{:04X} -> opcode:{:04b}, rs:{:04b}, rt:{:04b}, rd:{:04b}", instr,
            static_cast<uint16_t>(op), rsrc1, rsrc2, rdest);
    }
};