/// @file test_pc.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/program_counter.hpp"

void toggle_clock(digsim::signal_t<bool> &clk)
{
    clk.set(false);
    digsim::scheduler.run(); // Falling edge
    clk.set(true);
    digsim::scheduler.run(); // Rising edge
}

void step(digsim::signal_t<bool> &clk, digsim::signal_t<bs_phase_t> &phase)
{
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    toggle_clock(clk);
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bool> clk("clk", 0, 0);
    digsim::signal_t<bool> reset("reset", 0, 0);
    digsim::signal_t<bool> load("load", 0, 0);
    digsim::signal_t<bs_address_t> next_addr("next_addr", 0, 0);
    digsim::signal_t<bs_phase_t> phase("phase", 0, 0);
    digsim::signal_t<bs_address_t> addr("addr", 0, 0);

    // Instantiate the Program Counter
    program_counter_t pc0("pc0");
    pc0.clk(clk);
    pc0.reset(reset);
    pc0.load(load);
    pc0.next_addr(next_addr);
    pc0.phase(phase);
    pc0.addr(addr);

    digsim::scheduler.initialize();

    // ============================================================
    // Test: Reset
    reset.set(true);
    step(clk, phase);
    reset.set(false);

    if (addr.get().to_ulong() != 0) {
        digsim::error("Test", "Reset FAILED: Expected 0x0000, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Load Specific Address
    next_addr.set(0x1234);
    load.set(true);
    step(clk, phase);
    load.set(false);

    if (addr.get().to_ulong() != 0x1234) {
        digsim::error("Test", "Load FAILED: Expected 0x1234, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Increment from Loaded Value
    step(clk, phase); // Increment
    if (addr.get().to_ulong() != 0x1235) {
        digsim::error("Test", "Increment FAILED: Expected 0x1235, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    step(clk, phase); // Increment again
    if (addr.get().to_ulong() != 0x1236) {
        digsim::error("Test", "Increment FAILED: Expected 0x1236, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Reset After Increment
    reset.set(true);
    step(clk, phase);
    reset.set(false);

    if (addr.get().to_ulong() != 0x0000) {
        digsim::error("Test", "Reset FAILED after increment: Expected 0x0000, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Multiple Loads
    next_addr.set(0xAAAA);
    load.set(true);
    step(clk, phase);

    next_addr.set(0x5555);
    step(clk, phase);
    load.set(false);

    if (addr.get().to_ulong() != 0x5555) {
        digsim::error("Test", "Multiple Load FAILED: Expected 0x5555, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Load Disabled Should Not Affect
    next_addr.set(0xDEAD);
    load.set(false);
    step(clk, phase);

    if (addr.get().to_ulong() != 0x5556) {
        digsim::error(
            "Test", "Load Inactive FAILED: Expected 0x5556 (incremented), got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Reset While Load Active
    next_addr.set(0xBEEF);
    load.set(true);
    reset.set(true);
    step(clk, phase); // Reset should override load
    load.set(false);
    reset.set(false);

    if (addr.get().to_ulong() != 0x0000) {
        digsim::error("Test", "Reset While Load FAILED: Expected 0x0000, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Max value wraparound
    next_addr.set(0xFFFF);
    load.set(true);
    step(clk, phase);
    load.set(false);

    step(clk, phase); // Should wrap to 0x0000
    if (addr.get().to_ulong() != 0x0000) {
        digsim::error("Test", "Wraparound FAILED: Expected 0x0000, got 0x{:04X}", addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Multiple rising clocks with no state change
    bs_address_t prev = addr.get();
    step(clk, phase);
    step(clk, phase);

    if (addr.get() != bs_address_t(prev.to_ulong() + 2)) {
        digsim::error(
            "Test", "Idle Increment FAILED: Expected 0x{:04X}, got 0x{:04X}", prev.to_ulong() + 2,
            addr.get().to_ulong());
        return 1;
    }

    // ============================================================
    // Test: Mid-cycle Load Glitch.
    reset.set(false);
    load.set(false);
    next_addr.set(0x1234);
    step(clk, phase); // normal increment

    // Simulate glitch: set load after posedge but before another clock
    load.set(true);
    digsim::scheduler.run(); // No clock edge

    if (addr.get().to_ulong() == 0x1234) {
        digsim::error("Test", "Glitch: Load occurred without posedge!");
        return 1;
    }

    step(clk, phase); // Apply load properly
    if (addr.get().to_ulong() != 0x1234) {
        digsim::error("Test", "Load FAILED after being stable before rising edge");
        return 1;
    }

    return 0;
}
