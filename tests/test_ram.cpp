/// @file test_ram.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/ram.hpp"

void toggle_clock(digsim::signal_t<bool> &clk)
{
    clk.set(false);
    digsim::scheduler.run(); // Run with falling edge
    clk.set(true);
    digsim::scheduler.run(); // Run with rising edge
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals.
    digsim::signal_t<bool> clk("clk", 0, 0);
    digsim::signal_t<bool> reset("reset", 0, 0);
    digsim::signal_t<bs_address_t> addr("addr", 0, 0);
    digsim::signal_t<bs_data_t> data_in("data_in", 0, 0);
    digsim::signal_t<bool> write_enable("write_enable", 0, 0);
    digsim::signal_t<bs_phase_t> phase("phase", 0, 0);
    digsim::signal_t<bs_data_t> data_out("data_out", 0, 0);

    // Instantiate RAM
    ram_t ram0("ram0");
    ram0.clk(clk);
    ram0.reset(reset);
    ram0.addr(addr);
    ram0.data_in(data_in);
    ram0.write_enable(write_enable);
    ram0.phase(phase);
    ram0.data_out(data_out);

    phase.set(static_cast<uint8_t>(phase_t::WRITEBACK));

    digsim::scheduler.initialize();

    // --------------------------------------------------
    // Write a value to address 0x10
    addr.set(0x10);
    data_in.set(0xAA);
    write_enable.set(true);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0xAA) {
        digsim::error("Test", "Readback FAILED at 0x10!");
        return 1;
    }

    // --------------------------------------------------
    // Overwrite value
    addr.set(0x10);
    data_in.set(0x55);
    write_enable.set(true);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0x55) {
        digsim::error("Test", "Overwrite FAILED at 0x10!");
        return 1;
    }

    // --------------------------------------------------
    // Read uninitialized address
    addr.set(0x20);
    write_enable.set(false);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0) {
        digsim::error("Test", "Unexpected data at 0x20!");
        return 1;
    }

    // --------------------------------------------------
    // Reset test
    reset.set(true);
    toggle_clock(clk); // Triggers memory clear
    reset.set(false);

    addr.set(0x10);
    write_enable.set(false);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0) {
        digsim::error("Test", "Reset FAILED: 0x10 not cleared!");
        return 1;
    }

    // --------------------------------------------------
    // Write to highest valid address
    addr.set(RAM_SIZE - 1);
    data_in.set(0xAB);
    write_enable.set(true);
    toggle_clock(clk);

    // Read back from highest valid address
    write_enable.set(false);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0xAB) {
        digsim::error("Test", "Max address read FAILED");
        return 1;
    }

    // --------------------------------------------------
    // Repeated write test.

    addr.set(0x30);
    data_in.set(0x11);
    write_enable.set(true);
    toggle_clock(clk);

    data_in.set(0x22);
    toggle_clock(clk);

    write_enable.set(false);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0x22) {
        digsim::error("Test", "Repeated write FAILED at 0x30!");
        return 1;
    }

    // --------------------------------------------------
    // Write then reset then read.

    addr.set(0x40);
    data_in.set(0x77);
    write_enable.set(true);
    toggle_clock(clk);

    reset.set(true);
    toggle_clock(clk);
    reset.set(false);

    write_enable.set(false);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0) {
        digsim::error("Test", "Write-reset-read FAILED at 0x40!");
        return 1;
    }

    // --------------------------------------------------
    // Write with write_enable low, meaning, should not store.

    addr.set(0x50);
    data_in.set(0x99);
    write_enable.set(false);
    toggle_clock(clk); // Should *not* write

    toggle_clock(clk); // Read cycle

    if (data_out.get().to_ulong() != 0) {
        digsim::error("Test", "Unexpected write occurred at 0x50!");
        return 1;
    }

    // --------------------------------------------------
    // Test address independence.

    addr.set(0x60);
    data_in.set(0xAA);
    write_enable.set(true);
    toggle_clock(clk);

    addr.set(0x61);
    data_in.set(0xBB);
    toggle_clock(clk);

    write_enable.set(false);
    addr.set(0x60);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0xAA) {
        digsim::error("Test", "Address independence FAILED at 0x60!");
        return 1;
    }

    addr.set(0x61);
    toggle_clock(clk);

    if (data_out.get().to_ulong() != 0xBB) {
        digsim::error("Test", "Address independence FAILED at 0x61!");
        return 1;
    }

    // --------------------------------------------------
    // Test out-of-bounds address
    bs_address_t oob_addr;
    oob_addr = RAM_SIZE; // 1 << 16 -> 0x10000, which is OOB for 16-bit address
    addr.set(oob_addr);
    data_in.set(0xCD);
    write_enable.set(true);
    toggle_clock(clk); // This should trigger internal RAM error message (no main error needed)

    return 0;
}
