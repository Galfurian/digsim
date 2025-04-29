/// @file pc.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Program Counter (PC) module.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class pc_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;
    digsim::input_t<bool> load;
    digsim::input_t<bs_address_t> next_addr;
    digsim::output_t<bs_address_t> addr;

    pc_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , load("load", this)
        , next_addr("next_addr", this)
        , addr("addr", this)
    {
        ADD_SENSITIVITY(pc_t, evaluate, clk);
        ADD_CONSUMER(pc_t, evaluate, next_addr);
        ADD_PRODUCER(pc_t, evaluate, addr);
    }

private:
    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        bs_address_t new_addr;

        if (reset.get()) {
            new_addr = 0;
        } else if (load.get()) {
            new_addr = next_addr.get();
        } else {
            new_addr = addr.get().to_ulong() + 1;
        }

        addr.set(new_addr);

        if (reset.get()) {
            digsim::debug(get_name(), "reset     -> addr: 0x{:04X}", new_addr.to_ulong());
        } else if (load.get()) {
            digsim::debug(get_name(), "load      -> addr: 0x{:04X}", new_addr.to_ulong());
        } else {
            digsim::debug(get_name(), "increment -> addr: 0x{:04X}", new_addr.to_ulong());
        }
    }
};
