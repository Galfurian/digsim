/// @file example14.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/rom.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Create ROM contents (simple program).
    std::vector<uint64_t> program = {0x12, 0x34, 0x56, 0x78, 0x9A};

    // Signals.
    digsim::signal_t<std::bitset<8>> addr("addr", 0);
    digsim::signal_t<std::bitset<8>> instr("instr");

    // Instantiate the ROM.
    rom_t<8> rom("ROM", program);
    rom.addr(addr);
    rom.instruction(instr);

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running ROM tests ===");

    for (uint64_t i = 0; i < program.size(); ++i) {
        addr.set(i);
        digsim::scheduler.run();
        if (instr.get().to_ulong() == program[i]) {
            digsim::info("Test", "Instruction at 0x{:02X} OK: 0x{:02X}", i, program[i]);
        } else {
            digsim::error(
                "Test", "Mismatch at 0x{:02X}: expected 0x{:02X}, got 0x{:02X}", i, program[i], instr.get().to_ulong());
        }
    }

    // Test out of bounds.
    addr.set(program.size() + 1);
    digsim::scheduler.run();
    if (instr.get().to_ulong() == 0) {
        digsim::info("Test", "Out-of-bounds read OK: got 0x00");
    } else {
        digsim::error("Test", "Out-of-bounds read error: got 0x{:02X}", instr.get().to_ulong());
    }

    digsim::info("Main", "=== ROM tests finished ===");

    return 0;
}
