/// @file rom.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Read-Only Memory (ROM) module.

#pragma once

#include <simcore/simcore.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class rom_t : public simcore::module_t
{
public:
    simcore::input_t<bs_address_t> addr;             ///< Address input.
    simcore::output_t<bs_instruction_t> instruction; ///< Output instruction.

    rom_t(const std::string &_name, const std::vector<uint16_t> &_contents)
        : module_t(_name)
        , addr("addr", this)
        , instruction("instruction", this)
        , contents(_contents)
    {
        ADD_SENSITIVITY(rom_t, evaluate, addr);
        ADD_PRODUCER(rom_t, evaluate, instruction);
    }

    /// @brief Read instruction from ROM at given address
    uint16_t debug_read(std::size_t read_addr) const
    {
        if (read_addr >= contents.size()) {
            simcore::error(get_name(), "debug_read: out of bounds access to address {}", read_addr);
            return 0;
        }
        return contents[read_addr];
    }

private:
    std::vector<uint16_t> contents; ///< Contents of ROM (addressable by 16-bit instructions)

    void evaluate()
    {
        auto address = addr.get().to_ulong();

        uint16_t instr = 0;
        if (address < contents.size()) {
            instr = static_cast<uint16_t>(contents[address]);
        } else {
            instr = 0; // Default to 0 (NOP) if out of bounds
        }

        instruction.set(instr);

        // Decode instruction for debugging
        auto opcode = (instr >> 9U) & 0x7F;
        auto rs     = (instr >> 5U) & 0xF;
        auto rt     = (instr >> 1U) & 0xF;
        auto flag   = instr & 0x1;

        simcore::debug(
            get_name(),
            "address: 0x{:04X}, instruction: 0x{:04X} => opcode: 0x{:04X}, rs: 0x{:04X}, rt: 0x{:04X}, flag: 0x{:01X}",
            address, instr, opcode, rs, rt, flag);
    }
};
