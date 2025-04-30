/// @file test_phase_fsm.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "cpu/phase_fsm.hpp"

/// Toggle clock with rising edge
void toggle_clock(digsim::signal_t<bool> &clk)
{
    clk.set(false);
    digsim::scheduler.run();

    clk.set(true); // rising edge
    digsim::scheduler.run();
}

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Signals
    digsim::signal_t<bool> clk("clk", 0, 0);
    digsim::signal_t<bool> reset("reset", 0, 0);
    digsim::signal_t<bs_phase_t> phase("phase", 0, 0);

    // Instantiate FSM
    phase_fsm_t fsm("fsm");
    fsm.clk(clk);
    fsm.reset(reset);
    fsm.phase(phase);

    phase.set(static_cast<uint8_t>(phase_t::FETCH));

    // Initialize scheduler
    digsim::scheduler.initialize();

    // --------------------------------------------------
    // Test: Reset behavior
    // --------------------------------------------------

    reset.set(true);
    clk.set(true);
    digsim::scheduler.run(); // Trigger posedge evaluation
    reset.set(false);
    clk.set(false);
    digsim::scheduler.run();

    if (static_cast<phase_t>(phase.get().to_ulong()) != phase_t::FETCH) {
        digsim::error("Test", "Reset FAILED: Expected FETCH, got {}", phase.get().to_ulong());
        return 1;
    }

    // --------------------------------------------------
    // Test: Phase counting
    // --------------------------------------------------
    constexpr std::size_t test_cycles = NUM_PHASES * 2;

    for (std::size_t i = 0; i < test_cycles; ++i) {
        phase_t expected = static_cast<phase_t>((static_cast<uint8_t>(phase_t::FETCH) + i + 1) % NUM_PHASES);

        toggle_clock(clk);

        phase_t current = static_cast<phase_t>(phase.get().to_ulong());
        if (current != expected) {
            digsim::error(
                "Test", "Cycle {}: Phase mismatch. Expected {}, got {}", i, phase_to_string(expected),
                phase_to_string(current));
            return 1;
        }
    }
    return 0;
}