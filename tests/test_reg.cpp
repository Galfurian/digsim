/// @file example15.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "cpu/reg_file.hpp"

void toggle_clock(simcore::signal_t<bool> &clk)
{
    clk.set(false);
    simcore::scheduler.run(); // Falling edge
    clk.set(true);
    simcore::scheduler.run(); // Rising edge (posedge)
}

// --------------------------------------------------
// Utilities for register file testing
// --------------------------------------------------

void run_write(
    simcore::signal_t<bs_register_t> &addr_w,
    simcore::signal_t<bs_data_t> &data_in,
    simcore::signal_t<bool> &write_enable,
    simcore::signal_t<bs_phase_t> &phase,
    simcore::signal_t<bool> &clk,
    uint8_t reg,
    uint8_t value)
{
    addr_w.set(reg);
    data_in.set(value);
    write_enable.set(true);
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    toggle_clock(clk);
    write_enable.set(false);
}

void run_read(
    simcore::signal_t<bs_register_t> &addr_a,
    simcore::signal_t<bs_register_t> &addr_b,
    simcore::signal_t<bs_phase_t> &phase,
    simcore::signal_t<bool> &clk,
    uint8_t reg_a,
    uint8_t reg_b)
{
    addr_a.set(reg_a);
    addr_b.set(reg_b);
    phase.set(static_cast<uint8_t>(phase_t::FETCH));
    toggle_clock(clk);
}

int main()
{
    simcore::set_log_level(simcore::log_level_t::debug);

    // --------------------------------------------------
    // Setup
    // --------------------------------------------------

    simcore::signal_t<bool> clk("clk");
    simcore::signal_t<bool> reset("reset", false);
    simcore::signal_t<bs_phase_t> phase("phase", 0);
    simcore::signal_t<bs_register_t> addr_a("addr_a");
    simcore::signal_t<bs_register_t> addr_b("addr_b");
    simcore::signal_t<bs_register_t> addr_w("addr_w");
    simcore::signal_t<bool> write_enable("write_enable");
    simcore::signal_t<bs_data_t> data_in("data_in");
    simcore::signal_t<bs_data_t> data_a("data_a");
    simcore::signal_t<bs_data_t> data_b("data_b");

    reg_file_t reg0("reg0");
    reg0.clk(clk);
    reg0.reset(reset);
    reg0.phase(phase);
    reg0.addr_a(addr_a);
    reg0.addr_b(addr_b);
    reg0.addr_w(addr_w);
    reg0.write_enable(write_enable);
    reg0.data_in(data_in);
    reg0.data_a(data_a);
    reg0.data_b(data_b);

    simcore::scheduler.initialize();

    // --------------------------------------------------
    // Test 1: Basic write and readback
    // --------------------------------------------------
    run_write(addr_w, data_in, write_enable, phase, clk, 1, 0xAB);
    run_write(addr_w, data_in, write_enable, phase, clk, 2, 0xCD);
    run_read(addr_a, addr_b, phase, clk, 1, 2);

    if (data_a.get().to_ulong() != 0xAB || data_b.get().to_ulong() != 0xCD) {
        simcore::error(
            "Test", "Readback FAILED: r1=0x{:02X}, r2=0x{:02X}", data_a.get().to_ulong(), data_b.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 2: Uninitialized read
    // --------------------------------------------------
    run_read(addr_a, addr_b, phase, clk, 3, 3);
    if (data_a.get().to_ulong() != 0x00) {
        simcore::error("Test", "Uninitialized read FAILED: r3 = 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 3: Disabled write
    // --------------------------------------------------
    addr_w.set(4);
    data_in.set(0xEE);
    write_enable.set(false);
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    toggle_clock(clk);
    run_read(addr_a, addr_b, phase, clk, 4, 4);
    if (data_a.get().to_ulong() != 0x00) {
        simcore::error("Test", "Write-disabled register changed! r4 = 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 4: Overwrite
    // --------------------------------------------------
    run_write(addr_w, data_in, write_enable, phase, clk, 1, 0x11);
    run_read(addr_a, addr_b, phase, clk, 1, 1);
    if (data_a.get().to_ulong() != 0x11) {
        simcore::error("Test", "Overwrite FAILED: r1 = 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 5: Reset behavior
    // --------------------------------------------------
    reset.set(true);
    toggle_clock(clk);
    reset.set(false);
    run_read(addr_a, addr_b, phase, clk, 1, 2);
    if (data_a.get().to_ulong() != 0x00 || data_b.get().to_ulong() != 0x00) {
        simcore::error(
            "Test", "Reset FAILED: r1 = 0x{:02X}, r2 = 0x{:02X}", data_a.get().to_ulong(), data_b.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 6: Out-of-bounds access (logged error expected)
    // --------------------------------------------------
    addr_a.set(NUM_REGS);
    addr_b.set(0);
    addr_w.set(0);
    write_enable.set(true);
    data_in.set(0x55);
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    toggle_clock(clk); // Should log error but not crash

    // --------------------------------------------------
    // Test 7: Read-after-write visibility
    // --------------------------------------------------
    reset.set(true);
    toggle_clock(clk);
    reset.set(false);

    // Write value to r5
    run_write(addr_w, data_in, write_enable, phase, clk, 5, 0xAA);

    // Read immediately in WRITEBACK — value should be visible!
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    addr_a.set(5);
    toggle_clock(clk); // Read occurs after write applied

    if (data_a.get().to_ulong() != 0xAA) {
        simcore::error("Test", "Immediate read FAILED: expected 0xAA, got 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    // Optional: Read again in next cycle, should still match
    phase.set(static_cast<uint8_t>(phase_t::FETCH));
    toggle_clock(clk);

    if (data_a.get().to_ulong() != 0xAA) {
        simcore::error("Test", "Post-write read FAILED: expected 0xAA, got 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 8: Back-to-back writes
    // --------------------------------------------------
    run_write(addr_w, data_in, write_enable, phase, clk, 6, 0x66);
    run_write(addr_w, data_in, write_enable, phase, clk, 7, 0x77);
    run_read(addr_a, addr_b, phase, clk, 6, 7);
    if (data_a.get().to_ulong() != 0x66 || data_b.get().to_ulong() != 0x77) {
        simcore::error(
            "Test", "Back-to-back write FAILED: r6=0x{:02X}, r7=0x{:02X}", data_a.get().to_ulong(),
            data_b.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 9: Register 0 write
    // --------------------------------------------------
    run_write(addr_w, data_in, write_enable, phase, clk, 0, 0xFF);
    run_read(addr_a, addr_b, phase, clk, 0, 0);
    if (data_a.get().to_ulong() != 0xFF) {
        simcore::error("Test", "Write to r0 FAILED: expected 0xFF, got 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test 10: Invalid then valid write
    // --------------------------------------------------
    addr_w.set(NUM_REGS);
    data_in.set(0x55);
    write_enable.set(true);
    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));
    toggle_clock(clk);
    run_write(addr_w, data_in, write_enable, phase, clk, 1, 0x12);
    run_read(addr_a, addr_b, phase, clk, 1, 1);
    if (data_a.get().to_ulong() != 0x12) {
        simcore::error("Test", "Valid write after invalid FAILED: got 0x{:02X}", data_a.get().to_ulong());
        return 1;
    }

    return 0;
}
