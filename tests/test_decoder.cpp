/// @file example17.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/decoder.hpp"

int run_test(
    uint16_t instr_val,
    uint8_t exp_opcode,
    uint8_t exp_rs,
    uint8_t exp_rt,
    uint8_t exp_rd,
    digsim::signal_t<bs_instruction_t> &instruction,
    digsim::signal_t<bs_opcode_t> &opcode,
    digsim::signal_t<bs_register_t> &rs,
    digsim::signal_t<bs_register_t> &rt,
    digsim::signal_t<bs_register_t> &rd)
{
    instruction.set(std::bitset<16>(instr_val));
    digsim::scheduler.run();

    bool fail = false;

    if (opcode.get().to_ulong() != exp_opcode) {
        digsim::error(
            "Test", "Opcode mismatch: instr=0x{:04X}, expected {}, got {}", instr_val, exp_opcode,
            opcode.get().to_ulong());
        fail = true;
    }
    if (rs.get().to_ulong() != exp_rs) {
        digsim::error(
            "Test", "RS mismatch: instr=0x{:04X}, expected {}, got {}", instr_val, exp_rs, rs.get().to_ulong());
        fail = true;
    }
    if (rt.get().to_ulong() != exp_rt) {
        digsim::error(
            "Test", "RT mismatch: instr=0x{:04X}, expected {}, got {}", instr_val, exp_rt, rt.get().to_ulong());
        fail = true;
    }
    if (rd.get().to_ulong() != exp_rd) {
        digsim::error(
            "Test", "RD mismatch: instr=0x{:04X}, expected {}, got {}", instr_val, exp_rd, rd.get().to_ulong());
        fail = true;
    }

    return fail ? 1 : 0;
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bs_instruction_t> instruction("instruction", 0, 0);
    digsim::signal_t<bs_phase_t> phase("phase", 0, 0);
    digsim::signal_t<bs_opcode_t> opcode("opcode", 0, 0);
    digsim::signal_t<bs_register_t> rs("rs", 0, 0);
    digsim::signal_t<bs_register_t> rt("rt", 0, 0);
    digsim::signal_t<bs_register_t> rd("rd", 0, 0);

    // Instantiate the decoder
    decoder_t decoder("decoder");
    decoder.instruction(instruction);
    decoder.phase(phase);
    decoder.opcode(opcode);
    decoder.rs(rs);
    decoder.rt(rt);
    decoder.rd(rd);

    phase.set(static_cast<uint16_t>(phase_t::DECODE));

    digsim::scheduler.initialize();

    // --------------------------------------------------
    // Test: Basic decoding 0x1234
    if (run_test(0x1234, 0x1, 0x2, 0x3, 0x4, instruction, opcode, rs, rt, rd))
        return 1;

    // --------------------------------------------------
    // Test: All fields at max value 0xFFFF
    if (run_test(0xFFFF, 0xF, 0xF, 0xF, 0xF, instruction, opcode, rs, rt, rd))
        return 1;

    // --------------------------------------------------
    // Test: All fields at zero 0x0000
    if (run_test(0x0000, 0x0, 0x0, 0x0, 0x0, instruction, opcode, rs, rt, rd))
        return 1;

    // --------------------------------------------------
    // Test: Alternating pattern 0x0F0F
    if (run_test(0x0F0F, 0x0, 0xF, 0x0, 0xF, instruction, opcode, rs, rt, rd))
        return 1;

    // --------------------------------------------------
    // Test: Increasing fields 0x4567
    if (run_test(0x4567, 0x4, 0x5, 0x6, 0x7, instruction, opcode, rs, rt, rd))
        return 1;

    // --------------------------------------------------
    // Test: Decreasing fields 0x89AB
    if (run_test(0x89AB, 0x8, 0x9, 0xA, 0xB, instruction, opcode, rs, rt, rd))
        return 1;

    // --------------------------------------------------
    // Test: High-value random 0xABCD
    if (run_test(0xABCD, 0xA, 0xB, 0xC, 0xD, instruction, opcode, rs, rt, rd))
        return 1;

    return 0;
}
