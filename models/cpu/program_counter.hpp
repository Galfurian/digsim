/// @file program_counter.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Program Counter (PC) module.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class program_counter_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;               ///< Clock signal.
    digsim::input_t<bool> reset;             ///< Reset signal.
    digsim::input_t<bool> load;              ///< Load enable.
    digsim::input_t<bs_address_t> next_addr; ///< Address to load if load is active.
    digsim::input_t<bs_phase_t> phase;       ///< CPU execution phase.
    digsim::output_t<bs_address_t> addr;     ///< Current PC value.

    program_counter_t(const std::string &_name)
        : digsim::module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , load("load", this)
        , next_addr("next_addr", this)
        , phase("phase", this)
        , addr("addr", this)
        , pc(0)
    {
        ADD_SENSITIVITY(program_counter_t, evaluate, clk, reset);
        ADD_PRODUCER(program_counter_t, evaluate, addr);
    }

private:
    bs_address_t pc;

    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        if (reset.get()) {
            pc = 0;
            addr.set(0);
            digsim::debug(get_name(), "reset     -> addr: 0x{:04X}", 0);
            return;
        }

        const auto phase_val = static_cast<phase_t>(phase.get().to_ulong());

        // PC should only change in the WRITEBACK phase
        if (phase_val != phase_t::WRITEBACK) {
            digsim::debug(get_name(), "hold      -> addr: 0x{:04X}", pc.to_ulong());
            addr.set(pc);
            return;
        }

        if (load.get()) {
            pc = next_addr.get();
            digsim::debug(get_name(), "load      -> addr: 0x{:04X}", pc.to_ulong());
        } else {
            pc = pc.to_ulong() + 1;
            digsim::debug(get_name(), "increment -> addr: 0x{:04X}", pc.to_ulong());
        }

        addr.set(pc);
    }
};