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
    digsim::input_t<bs_instruction_t> instruction;
    digsim::output_t<bs_register_t> rs;
    digsim::output_t<bs_register_t> rt;
    digsim::output_t<bs_register_t> rd;
    digsim::output_t<bs_opcode_t> opcode;

    decoder_t(const std::string &_name)
        : module_t(_name)
        , instruction("instruction", this)
        , rs("rs", this)
        , rt("rt", this)
        , rd("rd", this)
        , opcode("opcode", this)
    {
        ADD_SENSITIVITY(decoder_t, evaluate, instruction);
        ADD_PRODUCER(decoder_t, evaluate, rs, rt, rd, opcode);
    }

private:
    void evaluate()
    {
        uint16_t instr = static_cast<uint16_t>(instruction.get().to_ulong());

        opcode.set((instr >> 12) & 0xF); // [15:12]
        rs.set((instr >> 8) & 0xF);      // [11:8]
        rt.set((instr >> 4) & 0xF);      // [7:4]
        rd.set((instr >> 0) & 0xF);      // [3:0]

        digsim::debug(
            get_name(), "Decoded instruction 0x{:04X} -> opcode:{}, rs:{}, rt:{}, rd:{}", instr, opcode.get(), rs.get(),
            rt.get(), rd.get());
    }
};

