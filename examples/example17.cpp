/// @file example17.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/decoder.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bs_instruction_t> instruction("instruction");
    digsim::signal_t<bs_register_t> opcode("opcode");
    digsim::signal_t<bs_register_t> rs("rs");
    digsim::signal_t<bs_register_t> rt("rt");
    digsim::signal_t<bs_opcode_t> rd("rd");

    // Instantiate the decoder
    decoder_t decoder("decoder");
    decoder.instruction(instruction);
    decoder.opcode(opcode);
    decoder.rs(rs);
    decoder.rt(rt);
    decoder.rd(rd);

    digsim::info("Main", "=== Initializing simulation ===");
    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running Decoder tests ===");

    // Define the test function
    auto test = [&](uint16_t instr_val, uint8_t exp_opcode, uint8_t exp_rs, uint8_t exp_rt, uint8_t exp_rd) {
        instruction.set(std::bitset<16>(instr_val));
        digsim::scheduler.run();
        bool ok = (opcode.get().to_ulong() == exp_opcode) && (rs.get().to_ulong() == exp_rs) &&
                  (rt.get().to_ulong() == exp_rt) && (rd.get().to_ulong() == exp_rd);
        if (!ok) {
            digsim::error(
                "Test", "instruction 0x{:04X} FAILED (opcode={}, rs={}, rt={}, rd={})", instr_val,
                opcode.get().to_ulong(), rs.get().to_ulong(), rt.get().to_ulong(), rd.get().to_ulong());
        }
    };

    // -------------------------
    // Tests
    // -------------------------
    test(0x1234, 0x1, 0x2, 0x3, 0x4); // opcode:1, rs:2, rt:3, rd:4
    test(0xABCD, 0xA, 0xB, 0xC, 0xD); // opcode:A, rs:B, rt:C, rd:D
    test(0x0F0F, 0x0, 0xF, 0x0, 0xF); // opcode:0, rs:F, rt:0, rd:F
    test(0xFFFF, 0xF, 0xF, 0xF, 0xF); // opcode:F, rs:F, rt:F, rd:F
    test(0x0000, 0x0, 0x0, 0x0, 0x0); // opcode:0, rs:0, rt:0, rd:0
    test(0x4567, 0x4, 0x5, 0x6, 0x7); // opcode:4, rs:5, rt:6, rd:7
    test(0x89AB, 0x8, 0x9, 0xA, 0xB); // opcode:8, rs:9, rt:A, rd:B

    digsim::info("Main", "=== Decoder tests finished ===");

    return 0;
}
