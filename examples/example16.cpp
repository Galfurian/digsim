/// @file example16.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/control_unit.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals.
    digsim::signal_t<bs_opcode_t> opcode("opcode", 0);
    digsim::signal_t<bs_opcode_t> alu_op("alu_op");
    digsim::signal_t<bool> reg_write("reg_write");
    digsim::signal_t<bool> mem_write("mem_write");
    digsim::signal_t<bool> mem_to_reg("mem_to_reg");

    // Instantiate the control unit.
    control_unit_t cu0("cu0");
    cu0.opcode(opcode);
    cu0.alu_op(alu_op);
    cu0.reg_write(reg_write);
    cu0.mem_write(mem_write);
    cu0.mem_to_reg(mem_to_reg);

    digsim::info("Main", "=== Initializing simulation ===");
    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running Control Unit tests ===");

    auto test = [&](opcode_t opcode_val, uint8_t expected_alu, bool exp_reg_write, bool exp_mem_write,
                    bool exp_mem_to_reg) {
        // Set the opcode value.
        opcode.set(static_cast<uint8_t>(opcode_val));
        // Run the scheduler to propagate the changes.
        digsim::scheduler.run();
        // If the opcode involves the ALU, check the opcode value.
        bool alu_ok;
        if (opcode_val >= opcode_t::ALU_AND && opcode_val <= opcode_t::ALU_LT) {
            alu_ok = (alu_op.get() == expected_alu);
        } else {
            alu_ok = true;
        }
        bool other_ok = (reg_write.get() == exp_reg_write) && (mem_write.get() == exp_mem_write) &&
                        (mem_to_reg.get() == exp_mem_to_reg);
        if (!alu_ok || !other_ok) {
            digsim::error(
                "Test", "Error with opcode {} (alu_op={}, reg_write={}, mem_write={}, mem_to_reg={})",
                static_cast<int>(opcode_val), alu_op.get(), reg_write.get(), mem_write.get(), mem_to_reg.get());
        }
    };

    // Tests
    test(opcode_t::ALU_ADD, 4, true, false, false);
    test(opcode_t::ALU_SUB, 5, true, false, false);
    test(opcode_t::ALU_MUL, 6, true, false, false);
    test(opcode_t::STORE, 0, false, true, false);
    test(opcode_t::LOAD, 0, true, false, true);
    test(opcode_t::NOP, 0, false, false, false);
    test(opcode_t::ILLEGAL, 0, false, false, false);

    digsim::info("Main", "=== Control Unit tests finished ===");

    return 0;
}
