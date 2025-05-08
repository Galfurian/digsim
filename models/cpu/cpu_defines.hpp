/// @file cpu_defines.hpp
/// @brief

#pragma once

#include <bitset>
#include <cstdint>

constexpr std::size_t ADDRESS_WIDTH     = 16;                   ///< The width of the address bus.
constexpr std::size_t DATA_WIDTH        = 16;                   ///< The width of the data bus.
constexpr std::size_t OPCODE_WIDTH      = 7;                    ///< The width of the opcode.
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
/// @brief Full 7-bit opcode combining primary class (3b) and function (4b).
enum opcode_t : uint8_t {
    // ALU (0x00–0x07)
    ALU_ADD      = 0x00,
    ALU_SUB      = 0x01,
    ALU_AND      = 0x02,
    ALU_OR       = 0x03,
    ALU_XOR      = 0x04,
    ALU_NOT      = 0x05,
    ALU_MUL      = 0x06,
    ALU_DIV      = 0x07,
    // SHIFT (0x10–0x13)
    SHIFT_LEFT   = 0x10,
    SHIFT_RIGHT  = 0x11,
    SHIFT_ARITH  = 0x12,
    SHIFT_ROTATE = 0x13,
    // CMP (0x20–0x23)
    CMP_EQ       = 0x20,
    CMP_LT       = 0x21,
    CMP_GT       = 0x22,
    CMP_NEQ      = 0x23,
    // MEMORY (0x30–0x33)
    MEM_LOAD     = 0x30,
    MEM_STORE    = 0x31,
    MEM_LOADI    = 0x32,
    MEM_MOVE     = 0x33,
    // BRANCH (0x40–0x41)
    BR_JMP       = 0x40,
    BR_BRT       = 0x41,
    // SYSTEM (0x50–0x53)
    SYS_NOP      = 0x50,
    SYS_HALT     = 0x51,
    SYS_BREAK    = 0x52,
    SYS_CALL     = 0x53,
    // Reserved (0x60, 0x70)
    RESERVED1    = 0x60,
    RESERVED2    = 0x70
};

/// @brief The phase of the CPU FSM.
enum phase_t : uint8_t {
    FETCH     = 0x0, ///< Fetch instruction.
    DECODE    = 0x1, ///< Decode instruction.
    EXECUTE   = 0x2, ///< Execute instruction.
    WRITEBACK = 0x3  ///< Write back result.
};

/// @brief Encodes a 16-bit instruction with opcode and function code.
/// @param op The opcode (7 bits).
/// @param rs The first source register.
/// @param rt The second source register.
/// @param flag The flag (optional, default is 0).
/// @return The 16-bit encoded instruction.
inline uint16_t encode_instruction(opcode_t op, uint8_t rs, uint8_t rt, uint8_t flag = 0)
{
    return static_cast<uint16_t>((static_cast<uint8_t>(op) & 0x7F) << 9) | //
           static_cast<uint16_t>((rs & 0xF) << 5) |                        //
           static_cast<uint16_t>((rt & 0xF) << 1) |                        //
           static_cast<uint16_t>((flag & 0x1));
}

/// @brief Decodes a 16-bit instruction.
/// @param instruction The 16-bit encoded instruction.
/// @param op The opcode (7 bits).
/// @param rs The first source register.
/// @param rt The second source register.
/// @param flag The flag (optional, default is 0).
inline void decode_instruction(uint16_t instruction, uint8_t &op, uint8_t &rs, uint8_t &rt, uint8_t &flag)
{
    op   = static_cast<opcode_t>((instruction >> 9) & 0x7F);
    rs   = (instruction >> 5) & 0xF;
    rt   = (instruction >> 1) & 0xF;
    flag = instruction & 0x1;
}

inline const char *opcode_to_string(uint8_t op)
{
    switch (op) {
    case opcode_t::ALU_ADD:
        return "ALU_ADD";
    case opcode_t::ALU_SUB:
        return "ALU_SUB";
    case opcode_t::ALU_AND:
        return "ALU_AND";
    case opcode_t::ALU_OR:
        return "ALU_OR";
    case opcode_t::ALU_XOR:
        return "ALU_XOR";
    case opcode_t::ALU_NOT:
        return "ALU_NOT";
    case opcode_t::ALU_MUL:
        return "ALU_MUL";
    case opcode_t::ALU_DIV:
        return "ALU_DIV";
    case opcode_t::SHIFT_LEFT:
        return "SHIFT_LEFT";
    case opcode_t::SHIFT_RIGHT:
        return "SHIFT_RIGHT";
    case opcode_t::SHIFT_ARITH:
        return "SHIFT_ARITH";
    case opcode_t::SHIFT_ROTATE:
        return "SHIFT_ROTATE";
    case opcode_t::CMP_EQ:
        return "CMP_EQ";
    case opcode_t::CMP_LT:
        return "CMP_LT";
    case opcode_t::CMP_GT:
        return "CMP_GT";
    case opcode_t::CMP_NEQ:
        return "CMP_NEQ";
    case opcode_t::MEM_LOAD:
        return "MEM_LOAD";
    case opcode_t::MEM_STORE:
        return "MEM_STORE";
    case opcode_t::MEM_LOADI:
        return "MEM_LOADI";
    case opcode_t::MEM_MOVE:
        return "MEM_MOVE";
    case opcode_t::BR_JMP:
        return "BR_JMP";
    case opcode_t::BR_BRT:
        return "BR_BRT";
    case opcode_t::SYS_NOP:
        return "SYS_NOP";
    case opcode_t::SYS_HALT:
        return "SYS_HALT";
    case opcode_t::SYS_BREAK:
        return "SYS_BREAK";
    case opcode_t::SYS_CALL:
        return "SYS_CALL";
    case opcode_t::RESERVED1:
        return "RESERVED1";
    case opcode_t::RESERVED2:
        return "RESERVED2";
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

template <std::size_t N>
inline std::bitset<N> operator+(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return std::bitset<N>(lhs.to_ulong() + rhs.to_ulong());
}

template <std::size_t N>
inline std::bitset<N> operator-(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return std::bitset<N>(lhs.to_ulong() - rhs.to_ulong());
}

template <std::size_t N>
inline std::bitset<N> operator*(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return std::bitset<N>(lhs.to_ulong() * rhs.to_ulong());
}

template <std::size_t N>
inline std::bitset<N> operator/(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return std::bitset<N>(lhs.to_ulong() / rhs.to_ulong());
}

template <std::size_t N>
inline bool operator<(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return lhs.to_ulong() < rhs.to_ulong();
}

template <std::size_t N>
inline bool operator>(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return lhs.to_ulong() > rhs.to_ulong();
}

// template <std::size_t N> inline std::bitset<N> operator|(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
// {
//     return std::bitset<N>(lhs.to_ulong() | rhs.to_ulong());
// }

// template <std::size_t N> inline std::bitset<N> operator^(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
// {
//     return std::bitset<N>(lhs.to_ulong() ^ rhs.to_ulong());
// }

// template <std::size_t N> inline std::bitset<N> operator~(const std::bitset<N> &lhs)
// {
//     return std::bitset<N>(~lhs.to_ulong());
// }

template <std::size_t N>
inline std::bitset<N> operator<<(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return std::bitset<N>(lhs.to_ulong() << rhs.to_ulong());
}

template <std::size_t N>
inline std::bitset<N> operator>>(const std::bitset<N> &lhs, const std::bitset<N> &rhs)
{
    return std::bitset<N>(lhs.to_ulong() >> rhs.to_ulong());
}
