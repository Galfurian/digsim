/// @file test_cpu_support.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/cpu_defines.hpp"

#include <digsim/digsim.hpp>

static int test_result = 0;

inline void check_equal(const std::string &field, uint16_t got, uint16_t expected, uint16_t instr)
{
    if (got != expected) {
        digsim::error(
            "Encode/Decode", "{} mismatch for instr 0x{:04X}: got {}, expected {}", field, instr, got, expected);
        test_result = 1;
    }
}

void test_flat_opcode(opcode_t op, uint8_t rs, uint8_t rt, uint8_t rd)
{
    uint16_t instr = encode_instruction(op, rs, rt, rd);

    uint8_t dec_op;
    uint8_t dec_rs, dec_rt, dec_rd;
    decode_instruction(instr, dec_op, dec_rs, dec_rt, dec_rd);

    uint8_t flat_op = static_cast<uint8_t>(op);
    check_equal("full_opcode", dec_op, flat_op, instr);
    check_equal("rs", dec_rs, rs, instr);
    check_equal("rt", dec_rt, rt, instr);
    check_equal("rd", dec_rd, rd, instr);
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    digsim::info("Encode/Decode Test", "Running flattened opcode tests...");

    for (uint16_t flat = 0; flat < 128; ++flat) {
        opcode_t op = static_cast<opcode_t>(flat);
        for (uint8_t rs = 0; rs < 16; ++rs) {
            for (uint8_t rt = 0; rt < 16; ++rt) {
                for (uint8_t rd = 0; rd < 2; ++rd) {
                    test_flat_opcode(op, rs, rt, rd);
                }
            }
        }
    }

    if (test_result == 0)
        digsim::info("Encode/Decode Test", "✅ All tests passed.");

    return test_result;
}