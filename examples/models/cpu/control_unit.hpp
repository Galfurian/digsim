/// @file control_unit.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple control unit for a CPU.

#pragma once

#include <digsim/digsim.hpp>

#include <bitset>
#include <iomanip>
#include <sstream>

template <size_t N> class control_unit_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> instruction;
    digsim::output_t<uint8_t> alu_op;
    digsim::output_t<bool> reg_write;
    digsim::output_t<bool> mem_write;
    digsim::output_t<bool> mem_to_reg;

    control_unit_t(const std::string &_name)
        : module_t(_name)
        , instruction("instruction")
        , alu_op("alu_op")
        , reg_write("reg_write")
        , mem_write("mem_write")
        , mem_to_reg("mem_to_reg")
    {
        ADD_SENSITIVITY(control_unit_t, evaluate, instruction);
        ADD_PRODUCER(control_unit_t, evaluate, alu_op, reg_write, mem_write, mem_to_reg);
    }

private:
    void evaluate()
    {
        uint8_t opcode = static_cast<uint8_t>(instruction.get().to_ulong() >> 12U);

        // ALU and LOAD.
        reg_write.set(opcode == 0 || opcode == 1 || opcode == 2 || opcode == 4);
        // STORE.
        mem_write.set(opcode == 3);
        // LOAD.
        mem_to_reg.set(opcode == 4);
        // Pass directly for now.
        alu_op.set(opcode);
    }
};
