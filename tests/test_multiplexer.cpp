/// @file example15.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/cpu_defines.hpp"
#include "cpu/multiplexer.hpp"

int run_test(
    uint16_t val_a,
    uint16_t val_b,
    bool sel_val,
    uint16_t expected_out,
    digsim::signal_t<bs_data_t> &a,
    digsim::signal_t<bs_data_t> &b,
    digsim::signal_t<bool> &sel,
    digsim::signal_t<bs_data_t> &out)
{
    a.set(val_a);
    b.set(val_b);
    sel.set(sel_val);
    digsim::scheduler.run();

    if (out.get().to_ulong() != expected_out) {
        digsim::error(
            "Test", "MUX FAILED: a=0x{:04X}, b=0x{:04X}, sel={} -> expected 0x{:04X}, got 0x{:04X}", val_a, val_b,
            sel_val, expected_out, out.get().to_ulong());
        return 1;
    }

    return 0;
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bs_data_t> a("a");
    digsim::signal_t<bs_data_t> b("b");
    digsim::signal_t<bool> sel("sel");
    digsim::signal_t<bs_data_t> out("out");

    // Instantiate the multiplexer
    multiplexer_t mux("mux");
    mux.a(a);
    mux.b(b);
    mux.sel(sel);
    mux.out(out);

    digsim::scheduler.initialize();

    // --------------------------------------------------
    // Test: sel = 0, expect output = a

    if (run_test(0x1234, 0xABCD, false, 0x1234, a, b, sel, out))
        return 1;

    // --------------------------------------------------
    // Test: sel = 1, expect output = b

    if (run_test(0x1234, 0xABCD, true, 0xABCD, a, b, sel, out))
        return 1;

    // --------------------------------------------------
    // Test: a == b, sel = 0

    if (run_test(0x5555, 0x5555, false, 0x5555, a, b, sel, out))
        return 1;

    // --------------------------------------------------
    // Test: a == b, sel = 1

    if (run_test(0x5555, 0x5555, true, 0x5555, a, b, sel, out))
        return 1;

    // --------------------------------------------------
    // Test: All zeros

    if (run_test(0x0000, 0x0000, false, 0x0000, a, b, sel, out))
        return 1;
    if (run_test(0x0000, 0x0000, true, 0x0000, a, b, sel, out))
        return 1;

    // --------------------------------------------------
    // Test: All ones

    if (run_test(0xFFFF, 0xFFFF, false, 0xFFFF, a, b, sel, out))
        return 1;
    if (run_test(0xFFFF, 0xFFFF, true, 0xFFFF, a, b, sel, out))
        return 1;

    // --------------------------------------------------
    // Test: Changing a, sel = 0

    a.set(0xAAAA);
    b.set(0xBBBB);
    sel.set(false);
    digsim::scheduler.run();
    a.set(0xCCCC); // Change input 'a' only
    digsim::scheduler.run();

    if (out.get().to_ulong() != 0xCCCC) {
        digsim::error(
            "Test", "Change in A not reflected when sel=0: expected 0xCCCC, got 0x{:04X}", out.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test: Changing b, sel = 1

    a.set(0x1111);
    b.set(0x2222);
    sel.set(true);
    digsim::scheduler.run();
    b.set(0x3333); // Change input 'b' only
    digsim::scheduler.run();

    if (out.get().to_ulong() != 0x3333) {
        digsim::error(
            "Test", "Change in B not reflected when sel=1: expected 0x3333, got 0x{:04X}", out.get().to_ulong());
        return 1;
    }

    return 0;
}
