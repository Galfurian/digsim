/// @file cpu_defines.hpp
/// @brief

#pragma once

#include <cstdint>

constexpr size_t ADDRESS_WIDTH     = 16;                  ///< The width of the address bus.
constexpr size_t DATA_WIDTH        = 16;                  ///< The width of the data bus.
constexpr size_t OPCODE_WIDTH      = 4;                   ///< The width of the opcode.
constexpr size_t REGISTER_WIDTH    = 4;                   ///< The width of the register address.
constexpr size_t STATUS_WIDTH      = 4;                   ///< The width of the status register.
constexpr size_t INSTRUCTION_WIDTH = 16;                  ///< The width of the instruction.
constexpr size_t MEM_SIZE          = 1 << ADDRESS_WIDTH;  ///< Memory size derived from address width.
constexpr size_t NUM_REGS          = 1 << REGISTER_WIDTH; ///< Number of registers = 2^REGISTER_WIDTH

using bs_address_t     = std::bitset<ADDRESS_WIDTH>;     ///< Address type.
using bs_data_t        = std::bitset<DATA_WIDTH>;        ///< Data type.
using bs_opcode_t      = std::bitset<OPCODE_WIDTH>;      ///< Opcode type.
using bs_register_t    = std::bitset<REGISTER_WIDTH>;    ///< Register type.
using bs_status_t      = std::bitset<STATUS_WIDTH>;      ///< Status type.
using bs_instruction_t = std::bitset<INSTRUCTION_WIDTH>; ///< Instruction type.

/// @brief CPU instruction opcodes.
enum class opcode_t : uint8_t {
    ALU_AND         = 0x0, ///< AND
    ALU_OR          = 0x1, ///< OR
    ALU_XOR         = 0x2, ///< XOR
    ALU_NOT         = 0x3, ///< NOT
    ALU_ADD         = 0x4, ///< ADD
    ALU_SUB         = 0x5, ///< SUB
    ALU_MUL         = 0x6, ///< MUL
    ALU_DIV         = 0x7, ///< DIV
    ALU_SHIFT_LEFT  = 0x8, ///< SHIFT LEFT
    ALU_SHIFT_RIGHT = 0x9, ///< SHIFT RIGHT
    ALU_EQUAL       = 0xA, ///< EQUAL
    ALU_LT          = 0xB, ///< LESS THAN
    STORE           = 0xC, ///< Store to memory
    LOAD            = 0xD, ///< Load from memory
    NOP             = 0xE, ///< No operation
    ILLEGAL         = 0xF  ///< Reserved / illegal instruction
};

inline const char *opcode_to_string(opcode_t op)
{
    switch (op) {
    case opcode_t::ALU_AND:
        return "ALU_AND";
    case opcode_t::ALU_OR:
        return "ALU_OR";
    case opcode_t::ALU_XOR:
        return "ALU_XOR";
    case opcode_t::ALU_NOT:
        return "ALU_NOT";
    case opcode_t::ALU_ADD:
        return "ALU_ADD";
    case opcode_t::ALU_SUB:
        return "ALU_SUB";
    case opcode_t::ALU_MUL:
        return "ALU_MUL";
    case opcode_t::ALU_DIV:
        return "ALU_DIV";
    case opcode_t::ALU_SHIFT_LEFT:
        return "ALU_SHIFT_LEFT";
    case opcode_t::ALU_SHIFT_RIGHT:
        return "ALU_SHIFT_RIGHT";
    case opcode_t::ALU_EQUAL:
        return "ALU_EQUAL";
    case opcode_t::ALU_LT:
        return "ALU_LT";
    case opcode_t::STORE:
        return "STORE";
    case opcode_t::LOAD:
        return "LOAD";
    case opcode_t::NOP:
        return "NOP";
    case opcode_t::ILLEGAL:
        return "ILLEGAL";
    default:
        return "UNKNOWN";
    }
}
