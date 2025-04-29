/// @file example15.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/reg.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bs_register_t> addr_a("addr_a", 0);
    digsim::signal_t<bs_register_t> addr_b("addr_b", 0);
    digsim::signal_t<bs_register_t> addr_w("addr_w", 0);
    digsim::signal_t<bool> write_enable("write_enable", false);
    digsim::signal_t<bs_data_t> data_in("data_in", 0);
    digsim::signal_t<bs_data_t> data_a("data_a", 0);
    digsim::signal_t<bs_data_t> data_b("data_b", 0);

    // Instantiate register file: 8 registers, each 8 bits
    reg_t reg0("reg0");
    reg0.addr_a(addr_a);
    reg0.addr_b(addr_b);
    reg0.addr_w(addr_w);
    reg0.write_enable(write_enable);
    reg0.data_in(data_in);
    reg0.data_a(data_a);
    reg0.data_b(data_b);

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running register_t tests ===");

    // Write 0xAB to register 1
    addr_w.set(1);
    data_in.set(0xAB);
    write_enable.set(true);
    digsim::scheduler.run();

    // Write 0xCD to register 2
    addr_w.set(2);
    data_in.set(0xCD);
    write_enable.set(true);
    digsim::scheduler.run();

    // Disable writing
    write_enable.set(false);
    digsim::scheduler.run();

    // Read from register 1 and 2
    addr_a.set(1);
    addr_b.set(2);
    digsim::scheduler.run();

    if (data_a.get().to_ulong() == 0xAB && data_b.get().to_ulong() == 0xCD) {
        digsim::info("Test", "Readback OK: r1=0x{:02X}, r2=0x{:02X}", data_a.get().to_ulong(), data_b.get().to_ulong());
    } else {
        digsim::error(
            "Test", "Readback Error: r1=0x{:02X}, r2=0x{:02X}", data_a.get().to_ulong(), data_b.get().to_ulong());
    }

    // Try reading uninitialized register
    addr_a.set(3);
    digsim::scheduler.run();
    if (data_a.get().to_ulong() == 0x00) {
        digsim::info("Test", "Uninitialized read OK (r3 == 0)");
    } else {
        digsim::error("Test", "Uninitialized read Error (r3 = 0x{:02X})", data_a.get().to_ulong());
    }

    digsim::info("Main", "=== RegFile tests finished ===");

    return 0;
}
