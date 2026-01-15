/// @file phase_fsm.hpp
/// @brief
/// @copyright Copyright (c) 2025

#include <simcore/simcore.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

/// @brief A simple phase FSM for a CPU with 4 stages.
class phase_fsm_t : public simcore::module_t
{
public:
    simcore::input_t<bool> clk;          ///< The clock signal.
    simcore::input_t<bool> reset;        ///< The reset signal.
    simcore::output_t<bs_phase_t> phase; ///< Current phase (FETCH, DECODE, etc.).

    phase_fsm_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , phase("phase", this)
        , state(phase_t::FETCH)
    {
        ADD_SENSITIVITY(phase_fsm_t, evaluate, clk, reset);
        ADD_PRODUCER(phase_fsm_t, evaluate, phase);
    }

private:
    phase_t state;

    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }
        if (reset.get()) {
            state = phase_t::FETCH;
        } else {
            state = static_cast<phase_t>((state + 1) % NUM_PHASES);
        }
        phase.set(state);
        simcore::debug(get_name(), "Phase changed to [{:2}] {:10}", static_cast<uint8_t>(state), phase_to_string(state));
    }
};