/// @file test_pc.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/program_counter.hpp"

/// @brief Holds the full environment to test the program counter (PC).
struct pc_env_t {
    digsim::signal_t<bool> clk{"clk"};
    digsim::signal_t<bool> reset{"reset"};
    digsim::signal_t<bool> load{"load"};
    digsim::signal_t<bool> jump_enable{"jump_enable"};
    digsim::signal_t<bool> branch_enable{"branch_enable"};
    digsim::signal_t<bs_address_t> next_addr{"next_addr"};
    digsim::signal_t<bs_status_t> alu_status{"alu_status"};
    digsim::signal_t<bs_opcode_t> opcode{"opcode"};
    digsim::signal_t<bs_phase_t> phase{"phase"};
    digsim::signal_t<bs_address_t> addr{"addr"};

    program_counter_t pc{"pc"};

    pc_env_t()
    {
        pc.clk(clk);
        pc.reset(reset);
        pc.load(load);
        pc.jump_enable(jump_enable);
        pc.branch_enable(branch_enable);
        pc.next_addr(next_addr);
        pc.alu_status(alu_status);
        pc.opcode(opcode);
        pc.phase(phase);
        pc.addr(addr);
        digsim::scheduler.initialize();
    }

    void toggle_clock()
    {
        clk.set(false);
        digsim::scheduler.run();
        clk.set(true);
        digsim::scheduler.run();
    }

    void step_writeback()
    {
        phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
        toggle_clock();
    }

    void reset_pc()
    {
        reset.set(true);
        step_writeback();
        reset.set(false);
    }

    void load_address(uint16_t value)
    {
        next_addr.set(value);
        load.set(true);
        step_writeback();
        load.set(false);
    }

    void expect_addr(uint16_t expected, const std::string &label)
    {
        if (addr.get().to_ulong() != expected) {
            digsim::error("Test", "{} FAILED: expected 0x{:04X}, got 0x{:04X}", label, expected, addr.get().to_ulong());
            std::exit(1);
        }
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    pc_env_t env;

    // === RESET
    env.reset_pc();
    env.expect_addr(0x0000, "Reset");

    // === LOAD + HOLD
    env.load_address(0x1234);
    env.expect_addr(0x1234, "Load immediate");

    env.step_writeback();
    env.expect_addr(0x1235, "Increment after load");

    env.step_writeback();
    env.expect_addr(0x1236, "Increment again");

    // === RESET after increment
    env.reset_pc();
    env.expect_addr(0x0000, "Reset after increment");

    // === MULTIPLE LOADS
    env.load_address(0xAAAA);
    env.load_address(0x5555); // load again immediately
    env.expect_addr(0x5555, "Multiple loads");

    // === LOAD disabled — should increment instead
    env.next_addr.set(0xDEAD);
    env.load.set(false);
    env.step_writeback();
    env.expect_addr(0x5556, "Disabled load -> increment");

    // === Reset with active load — reset wins
    env.next_addr.set(0xBEEF);
    env.load.set(true);
    env.reset.set(true);
    env.step_writeback();
    env.load.set(false);
    env.reset.set(false);
    env.expect_addr(0x0000, "Reset overrides load");

    // === Max wraparound
    env.load_address(0xFFFF);
    env.step_writeback();
    env.expect_addr(0x0000, "Wraparound after 0xFFFF");

    // === No-op rising clocks still increment
    env.step_writeback();
    env.expect_addr(0x0001, "Increment after wrap");
    env.step_writeback();
    env.expect_addr(0x0002, "Increment again");

    // === Glitch test: no rising edge, just scheduler run
    env.next_addr.set(0x1234);
    env.load.set(true);
    digsim::scheduler.run(); // no posedge → should NOT latch
    if (env.addr.get().to_ulong() == 0x1234) {
        digsim::error("Test", "Glitch: Load latched without rising edge!");
        return 1;
    }

    env.step_writeback(); // now apply load cleanly
    env.expect_addr(0x1234, "Proper load after glitch");
    env.load.set(false);

    // === Branch test: only if FLAG_CMP_TRUE and enabled
    env.reset_pc();
    env.alu_status.set(static_cast<uint8_t>(alu_t::FLAG_CMP_TRUE));
    env.opcode.set(static_cast<uint8_t>(opcode_t::BR_BEQ));
    env.branch_enable.set(true);
    env.next_addr.set(0x2000);
    env.step_writeback();
    env.branch_enable.set(false);
    env.expect_addr(0x2000, "Branch taken (CMP_TRUE)");

    // === Branch fail: CMP_FALSE → should increment instead
    env.alu_status.set(static_cast<uint8_t>(alu_t::FLAG_CMP_FALSE));
    env.next_addr.set(0xDEAD);
    env.step_writeback();
    env.expect_addr(0x2001, "Branch NOT taken (CMP_FALSE)");

    // === Jump test: unconditional
    env.jump_enable.set(true);
    env.branch_enable.set(false);
    env.next_addr.set(0x9999);
    env.step_writeback();
    env.jump_enable.set(false);
    env.expect_addr(0x9999, "Jump taken");

    digsim::info("Test", "All program counter tests passed successfully.");
    return 0;
}
