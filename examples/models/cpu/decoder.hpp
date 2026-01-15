/// @file decoder.hpp
/// @brief

#pragma once

#include <simcore/simcore.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class decoder_t : public simcore::module_t
{
public:
    simcore::input_t<bs_instruction_t> instruction; ///< Raw instruction input.
    simcore::input_t<bs_phase_t> phase;             ///< Phase of execution.
    simcore::output_t<bs_opcode_t> opcode;          ///< Output: decoded opcode.
    simcore::output_t<bs_register_t> rs;            ///< Output: source register A.
    simcore::output_t<bs_register_t> rt;            ///< Output: source register B.
    simcore::output_t<bool> flag;                   ///< Output: flag register.

    decoder_t(const std::string &_name)
        : simcore::module_t(_name)
        , instruction("instruction", this)
        , phase("phase", this)
        , opcode("opcode", this)
        , rs("rs", this)
        , rt("rt", this)
        , flag("flag", this)
    {
        ADD_SENSITIVITY(decoder_t, evaluate, instruction, phase);
        ADD_PRODUCER(decoder_t, evaluate, opcode, rs, rt, flag);
    }

private:
    void evaluate()
    {
        // [15:09] opcode
        // [08:04] rs
        // [03:00] rt
        // [00:00] flag

        const auto current_phase = static_cast<phase_t>(phase.get().to_ulong());
        if (current_phase != phase_t::DECODE) {
            return;
        }
        // Get the raw instruction.
        uint16_t raw = static_cast<uint16_t>(instruction.get().to_ulong());

        // Decode the instruction.
        uint8_t opcode_val, rs_val, rt_val, flag_val;
        decode_instruction(raw, opcode_val, rs_val, rt_val, flag_val);

        // Set the output signals.
        opcode.set(opcode_val);
        rs.set(rs_val);
        rt.set(rt_val);
        flag.set(flag_val);

        simcore::debug(
            get_name(),
            "0x{:04X} -> opcode: 0x{:04X} [{:16}], rs: 0x{:04X}, rt: 0x{:04X}, flag: 0x{:01X}", raw,
            opcode_val, opcode_to_string(opcode_val), rs_val, rt_val, flag_val);
    }
};