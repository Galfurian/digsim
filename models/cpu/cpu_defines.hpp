/// @file cpu_defines.hpp
/// @brief

#pragma once

#include <bitset>
#include <cstdint>

constexpr std::size_t ADDRESS_WIDTH     = 16;                   ///< The width of the address bus.
constexpr std::size_t DATA_WIDTH        = 16;                   ///< The width of the data bus.
constexpr std::size_t OPCODE_WIDTH      = 4;                    ///< The width of the opcode.
constexpr std::size_t REGISTER_WIDTH    = 4;                    ///< The width of the register address.
constexpr std::size_t STATUS_WIDTH      = 4;                    ///< The width of the status register.
constexpr std::size_t INSTRUCTION_WIDTH = 16;                   ///< The width of the instruction.
constexpr std::size_t PHASE_FSM_WIDTH   = 2;                    ///< The width of the phase FSM.
constexpr std::size_t RAM_SIZE          = 1 << ADDRESS_WIDTH;   ///< Memory size derived from address width.
constexpr std::size_t NUM_REGS          = 1 << REGISTER_WIDTH;  ///< Number of registers = 2^REGISTER_WIDTH
constexpr std::size_t NUM_PHASES        = 1 << PHASE_FSM_WIDTH; ///< Number of phases = 2^PHASE_FSM_WIDTH

using bs_address_t     = std::bitset<ADDRESS_WIDTH>;     ///< Address type.
using bs_data_t        = std::bitset<DATA_WIDTH>;        ///< Data type.
using bs_opcode_t      = std::bitset<OPCODE_WIDTH>;      ///< Opcode type.
using bs_register_t    = std::bitset<REGISTER_WIDTH>;    ///< Register type.
using bs_status_t      = std::bitset<STATUS_WIDTH>;      ///< Status type.
using bs_instruction_t = std::bitset<INSTRUCTION_WIDTH>; ///< Instruction type.
using bs_phase_t       = std::bitset<PHASE_FSM_WIDTH>;   ///< Phase type.

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

/// @brief The phase of the CPU FSM.
enum class phase_t : uint8_t {
    FETCH     = 0x0, ///< Fetch instruction.
    DECODE    = 0x1, ///< Decode instruction.
    EXECUTE   = 0x2, ///< Execute instruction.
    WRITEBACK = 0x3  ///< Write back result.
};

/// @brief Encode an instruction from opcode and registers.
/// @param op The opcode.
/// @param rs Source register A (bits [11:8]).
/// @param rt Source register B (bits [7:4]).
/// @param rd Destination register (bits [3:0]).
/// @return 16-bit encoded instruction.
uint16_t encode_instruction(opcode_t op, uint8_t rs, uint8_t rt, uint8_t rd)
{
    return static_cast<uint16_t>(static_cast<uint16_t>(op) << 12U) | static_cast<uint16_t>((rs & 0xFU) << 8U) |
           static_cast<uint16_t>((rt & 0xFU) << 4U) | static_cast<uint16_t>(rd & 0xFU);
}

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

const char *phase_to_string(phase_t phase)
{
    switch (phase) {
    case phase_t::FETCH:
        return "FETCH";
    case phase_t::DECODE:
        return "DECODE";
    case phase_t::EXECUTE:
        return "EXECUTE";
    case phase_t::WRITEBACK:
        return "WRITEBACK";
    default:
        return "UNKNOWN";
    }
}
