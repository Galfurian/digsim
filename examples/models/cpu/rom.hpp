/// @file rom.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Read-Only Memory (ROM) module.

#pragma once

#include <digsim/digsim.hpp>

#include <bitset>
#include <iomanip>
#include <sstream>

template <size_t N> class rom_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> addr;
    digsim::output_t<std::bitset<N>> instruction;

    rom_t(const std::string &_name, const std::vector<uint64_t> &_contents)
        : module_t(_name)
        , addr("addr")
        , instruction("instruction")
        , contents(_contents)
    {
        ADD_SENSITIVITY(rom_t, evaluate, addr);
        ADD_PRODUCER(rom_t, evaluate, instruction);
    }

private:
    std::vector<uint64_t> contents;

    void evaluate()
    {
        // Get the address.
        uint64_t address = addr.get().to_ulong();

        // Read the instruction from ROM if address is valid.
        if (address < contents.size()) {
            instruction.set(contents[address]);
        } else {
            instruction.set(0);
        }

        // Debugging output.
        std::stringstream ss;
        ss << "address: " << addr.get() << " -> instruction: " << instruction.get();
        digsim::debug("ROM", ss.str());
    }
};
