/// @file example17.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/decoder.hpp"

static int test_result = 0;

struct decoder_env_t {
    digsim::signal_t<bs_instruction_t> instruction{"instruction", 0, 0};
    digsim::signal_t<bs_phase_t> phase{"phase", static_cast<uint16_t>(phase_t::DECODE), 0};
    digsim::signal_t<bs_opcode_t> opcode{"opcode", 0, 0};
    digsim::signal_t<bs_register_t> rs{"rs", 0, 0};
    digsim::signal_t<bs_register_t> rt{"rt", 0, 0};
    digsim::signal_t<bs_register_t> rd{"rd", 0, 0};
    decoder_t decoder{"decoder"};

    decoder_env_t()
    {
        decoder.instruction(instruction);
        decoder.phase(phase);
        decoder.opcode(opcode);
        decoder.rs(rs);
        decoder.rt(rt);
        decoder.rd(rd);
    }

    void set_instruction(bs_instruction_t instr)
    {
        instruction.set(instr);
        digsim::scheduler.run();
    }
};

void run_test(decoder_env_t &env, uint16_t instruction)
{
    env.set_instruction(instruction);
    digsim::scheduler.run();

    uint8_t opcode, rs, rt, rd;
    decode_instruction(instruction, opcode, rs, rt, rd);

    if (env.opcode.get() != opcode) {
        digsim::error("Test", "OP mismatch (got 0x{:X}, expected 0x{:X})", env.opcode.get().to_ulong(), opcode);
        test_result = 1;
    }
    if (env.rs.get() != rs) {
        digsim::error("Test", "RS mismatch (got 0x{:X}, expected 0x{:X})", env.rs.get().to_ulong(), rs);
        test_result = 1;
    }
    if (env.rt.get() != rt) {
        digsim::error("Test", "RT mismatch (got 0x{:X}, expected 0x{:X})", env.rt.get().to_ulong(), rt);
        test_result = 1;
    }
    if (env.rd.get() != rd) {
        digsim::error("Test", "RD mismatch (got 0x{:X}, expected 0x{:X})", env.rd.get().to_ulong(), rd);
        test_result = 1;
    }
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);
    digsim::scheduler.initialize();

    decoder_env_t env;

    std::vector<uint16_t> raw_instructions = {
        encode_instruction(opcode_t::ALU_DIV, 0x8, 0x7, 1), // 0x0F0F
        encode_instruction(opcode_t::CMP_GT, 0xB, 0x3, 1),  // 0x4567
        encode_instruction(opcode_t::BR_BGT, 0xD, 0x5, 1),  // 0x89AB
        encode_instruction(opcode_t::ALU_ADD, 0x0, 0x0, 0), // 0x0000
    };

    for (auto instruction : raw_instructions) {
        run_test(env, instruction);
    }

    return test_result;
}
