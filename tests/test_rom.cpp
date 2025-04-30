/// @file example14.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/rom.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // --------------------------------------------------
    // Create ROM contents (simple test program).
    std::vector<uint16_t> program = {
        0x1234, // opcode=0x1
        0x5678, // opcode=0x5
        0x9ABC, // opcode=0x9
        0xDEF0, // opcode=0xD
        0x000F  // opcode=0x0
    };

    // Signals
    digsim::signal_t<bs_address_t> addr("addr", 0);
    digsim::signal_t<bs_instruction_t> instr("instr");

    // Instantiate the ROM
    rom_t rom0("rom0", program);
    rom0.addr(addr);
    rom0.instruction(instr);

    digsim::scheduler.initialize();

    // --------------------------------------------------
    // Test: Sequential Valid Reads.

    for (std::size_t i = 0; i < program.size(); ++i) {
        addr.set(i);
        digsim::scheduler.run();

        uint16_t actual   = static_cast<uint16_t>(instr.get().to_ulong());
        uint16_t expected = program[i];

        if (actual != expected) {
            digsim::error("Test", "Mismatch at 0x{:02X}: expected 0x{:04X}, got 0x{:04X}", i, expected, actual);
            return 1;
        }
    }

    // --------------------------------------------------
    // Test: Out-of-Bounds Read.

    addr.set(program.size()); // First invalid address
    digsim::scheduler.run();

    if (instr.get().to_ulong() != 0x0000) {
        digsim::error("Test", "Out-of-bounds read FAILED: Expected 0x0000, got 0x{:04X}", instr.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test: High Out-of-Bounds Read.

    addr.set(0xFFFF); // Far out-of-bounds
    digsim::scheduler.run();

    if (instr.get().to_ulong() != 0x0000) {
        digsim::error("Test", "High OOB read FAILED: Expected 0x0000, got 0x{:04X}", instr.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test: Repeated Access (No Change).

    addr.set(2); // Valid address
    digsim::scheduler.run();
    uint16_t expected = program[2];

    // Set the same address again
    addr.set(2);
    digsim::scheduler.run();

    if (instr.get().to_ulong() != expected) {
        digsim::error(
            "Test", "Repeated read FAILED at 0x02: Expected 0x{:04X}, got 0x{:04X}", expected, instr.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test: Random Valid Reads.

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int k = 0; k < 10; ++k) {
        std::size_t random_addr = static_cast<std::size_t>(std::rand()) % program.size();
        addr.set(random_addr);
        digsim::scheduler.run();
        uint16_t actual = static_cast<uint16_t>(instr.get().to_ulong());
        expected        = program[random_addr];
        if (actual != expected) {
            digsim::error(
                "Test", "Mismatch at 0x{:02X}: expected 0x{:04X}, got 0x{:04X}", random_addr, expected, actual);
            return 1;
        }
    }

    // --------------------------------------------------
    // Test: Stable Reads at Same Address.

    addr.set(1); // Use a valid address, e.g., address 1
    digsim::scheduler.run();
    expected = static_cast<uint16_t>(instr.get().to_ulong());
    for (int i = 0; i < 5; ++i) {
        digsim::scheduler.run();
        if (instr.get().to_ulong() != expected) {
            digsim::error(
                "Test", "Stable read FAILED at iteration {}: expected 0x{:04X}, got 0x{:04X}", i, expected,
                instr.get().to_ulong());
            return 1;
        }
    }

    // --------------------------------------------------
    // Test: Multiple Sequential Passes Through ROM.

    for (int pass = 0; pass < 3; ++pass) {
        for (std::size_t i = 0; i < program.size(); ++i) {
            addr.set(i);
            digsim::scheduler.run();
            if (instr.get().to_ulong() != program[i]) {
                digsim::error(
                    "Test", "Pass {}: Mismatch at 0x{:02X}: expected 0x{:04X}, got 0x{:04X}", pass, i, program[i],
                    instr.get().to_ulong());
                return 1;
            }
        }
    }

    // --------------------------------------------------
    // Test: Rapid Consecutive Address Changes.

    for (std::size_t i = 0; i < program.size(); ++i) {
        addr.set(i);
        // Not calling scheduler.run() immediately for each address change
    }
    digsim::scheduler.run(); // Process the final address update
    if (instr.get().to_ulong() != program.back()) {
        digsim::error(
            "Test", "Rapid update FAILED: expected 0x{:04X}, got 0x{:04X}", program.back(), instr.get().to_ulong());
        return 1;
    }

    return 0;
}
