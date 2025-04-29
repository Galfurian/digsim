/// @file example16.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/control_unit.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Input signals
    digsim::signal_t<std::bitset<16>> instruction("instruction", 0);

    // Output signals
    digsim::signal_t<uint8_t> alu_op("alu_op");
    digsim::signal_t<bool> reg_write("reg_write");
    digsim::signal_t<bool> mem_write("mem_write");
    digsim::signal_t<bool> mem_to_reg("mem_to_reg");

    // Instantiate the control unit
    control_unit_t<16> cu0("cu0");

    cu0.instruction(instruction);
    cu0.alu_op(alu_op);
    cu0.reg_write(reg_write);
    cu0.mem_write(mem_write);
    cu0.mem_to_reg(mem_to_reg);

    digsim::info("Main", "=== Initializing simulation ===");
    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running Control Unit tests ===");

    auto test = [&](uint16_t instr_val, uint8_t expected_alu, bool exp_reg_write, bool exp_mem_write,
                    bool exp_mem_to_reg) {
        instruction.set(instr_val);
        digsim::scheduler.run();
        bool ok = (alu_op.get() == expected_alu) && (reg_write.get() == exp_reg_write) &&
                  (mem_write.get() == exp_mem_write) && (mem_to_reg.get() == exp_mem_to_reg);
        if (ok) {
            digsim::info("Test", "Instruction {:04X} OK", instr_val);
        } else {
            digsim::error(
                "Test", "Instruction {:04X} Failed (alu_op={}, reg_write={}, mem_write={}, mem_to_reg={})", instr_val,
                alu_op.get(), reg_write.get(), mem_write.get(), mem_to_reg.get());
        }
    };

    // -------------------------
    // Tests
    // -------------------------
    test(0x0000, 0, true, false, false);   // ALU op
    test(0x1000, 1, true, false, false);   // ALU op
    test(0x2000, 2, true, false, false);   // ALU op
    test(0x3000, 3, false, true, false);   // STORE
    test(0x4000, 4, true, false, true);    // LOAD
    test(0x5000, 5, false, false, false);  // Unknown or NOP
    test(0xF000, 15, false, false, false); // Reserved or illegal

    digsim::info("Main", "=== Control Unit tests finished ===");

    return 0;
}
