/// @file example13.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/ram.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals.
    digsim::signal_t<bs_address_t> addr("addr");
    digsim::signal_t<bs_data_t> data_in("data_in");
    digsim::signal_t<bool> write_enable("write_enable");
    digsim::signal_t<bs_data_t> data_out("data_out");

    // Instantiate RAM
    ram_t ram0("ram0");
    ram0.addr(addr);
    ram0.data_in(data_in);
    ram0.write_enable(write_enable);
    ram0.data_out(data_out);

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Starting RAM tests ===");

    // --------------------------------------------------
    // 1. Write a value to address 0x10
    addr.set(0x10);
    data_in.set(0xAA);
    write_enable.set(true);

    digsim::scheduler.run(); // Perform the write

    // --------------------------------------------------
    // 2. Read the value back
    write_enable.set(false);
    digsim::scheduler.run();

    if (data_out.get().to_ulong() == 0xAA) {
        digsim::info("Test", "Readback OK: 0x10 -> 0xAA");
    } else {
        digsim::error("Test", "Readback FAILED at 0x10!");
    }

    // --------------------------------------------------
    // 3. Overwrite value
    addr.set(0x10);
    data_in.set(0x55);
    write_enable.set(true);

    digsim::scheduler.run(); // Perform the overwrite

    // --------------------------------------------------
    // 4. Read overwritten value
    write_enable.set(false);
    digsim::scheduler.run();

    if (data_out.get().to_ulong() == 0x55) {
        digsim::info("Test", "Overwrite OK: 0x10 -> 0x55");
    } else {
        digsim::error("Test", "Overwrite FAILED at 0x10!");
    }

    // --------------------------------------------------
    // 5. Read uninitialized address
    addr.set(0x20);
    digsim::scheduler.run();

    if (data_out.get().to_ulong() == 0) {
        digsim::info("Test", "Uninitialized read OK (0x20 == 0x00)");
    } else {
        digsim::error("Test", "Unexpected data at 0x20!");
    }

    digsim::info("Main", "=== RAM tests finished ===");

    return 0;
}
