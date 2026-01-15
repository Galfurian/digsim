/// @file example15.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "cpu/cpu_defines.hpp"
#include "cpu/multiplexer.hpp"

struct multipleser_env_t {
    simcore::signal_t<bs_data_t> a{"a", 0, 0};
    simcore::signal_t<bs_data_t> b{"b", 0, 0};
    simcore::signal_t<bool> sel{"sel", 0, 0};
    simcore::signal_t<bs_data_t> out{"out", 0, 0};

    multiplexer_t<bs_data_t> mux{"mux"};

    int test_result = 0;

    multipleser_env_t()
    {
        mux.a(a);
        mux.b(b);
        mux.sel(sel);
        mux.out(out);
    }

    void apply_inputs(uint16_t a_val, uint16_t b_val, bool sel_val)
    {
        a.set(a_val);
        b.set(b_val);
        sel.set(sel_val);
    }

    void execute_cycle() { simcore::scheduler.run(); }

    void run_test(uint16_t a_val, uint16_t b_val, bool sel_val, bs_data_t expected_out)
    {
        // Apply inputs.
        apply_inputs(a_val, b_val, sel_val);
        // Execute the cycle.
        execute_cycle();
        // Validate the output.
        validate_output(expected_out);
    }

    void validate_output(bs_data_t expected_out)
    {
        auto a_val   = a.get().to_ulong();
        auto b_val   = b.get().to_ulong();
        auto sel_val = sel.get();
        auto got_out = out.get().to_ulong();
        auto exp_out = expected_out.to_ulong();
        if (got_out != exp_out) {
            simcore::error("Test", "MUX FAILED: a=0x{:04X}, b=0x{:04X}, sel={} -> expected 0x{:04X}, got 0x{:04X}", a_val, b_val, sel_val, exp_out, got_out);
            test_result = 1;
        }
    }
};

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    multipleser_env_t env;

    simcore::scheduler.initialize();

    env.run_test(0x1234, 0xABCD, false, 0x1234);
    env.run_test(0x1234, 0xABCD, true, 0xABCD);
    env.run_test(0x5555, 0x5555, false, 0x5555);
    env.run_test(0x5555, 0x5555, true, 0x5555);
    env.run_test(0x0000, 0x0000, false, 0x0000);
    env.run_test(0x0000, 0x0000, true, 0x0000);
    env.run_test(0xFFFF, 0xFFFF, false, 0xFFFF);
    env.run_test(0xFFFF, 0xFFFF, true, 0xFFFF);

    // --------------------------------------------------
    // Test: Changing a, sel = 0

    env.a.set(0xAAAA);
    env.b.set(0xBBBB);
    env.sel.set(false);
    simcore::scheduler.run();
    env.a.set(0xCCCC);
    simcore::scheduler.run();
    env.validate_output(0xCCCC);

    // --------------------------------------------------
    // Test: Changing b, sel = 1

    env.a.set(0x1111);
    env.b.set(0x2222);
    env.sel.set(true);
    simcore::scheduler.run();
    env.b.set(0x3333);
    simcore::scheduler.run();
    env.validate_output(0x3333);

    return 0;
}
