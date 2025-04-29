/// @file pc.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Program Counter (PC) module.

#pragma once

#include <digsim/digsim.hpp>

#include <bitset>
#include <iomanip>
#include <sstream>

template <size_t N> class pc_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;
    digsim::input_t<bool> load;
    digsim::input_t<std::bitset<N>> next_addr;
    digsim::output_t<std::bitset<N>> addr;

    pc_t(const std::string &_name)
        : module_t(_name)
        , clk("clk")
        , reset("reset")
        , load("load")
        , next_addr("next_addr")
        , addr("addr")
    {
        ADD_SENSITIVITY(pc_t, evaluate, clk);
        ADD_CONSUMER(pc_t, evaluate, next_addr);
        ADD_PRODUCER(pc_t, evaluate, addr);
    }

private:
    void evaluate()
    {
        if (!clk.posedge())
            return;
        // Prepare the new address value.
        std::bitset<N> new_addr;
        // If reset is active, set addr to 0.
        if (reset.get()) {
            new_addr = 0;
        }
        // If load is active, set addr to next_addr.
        else if (load.get()) {
            new_addr = next_addr.get();
        }
        // Otherwise, increment addr by 1.
        else {
            new_addr = addr.get().to_ulong() + 1;
        }
        // Update the addr output signal.
        addr.set(new_addr);

        // Debugging output.
        std::stringstream ss;
        ss << "clk:" << clk.get() << ", reset:" << reset.get() << ", load:" << load.get()
           << ", next_addr: " << next_addr.get();
        if (reset.get()) {
            ss << " -> addr: 00000000 (reset)";
        } else if (load.get()) {
            ss << " -> addr: " << new_addr << " (load)";
        } else {
            ss << " -> addr: " << new_addr << " (increment)";
        }
        if (addr.get_delay() > 0) {
            ss << " (+" << addr.get_delay() << "t)";
        }
        digsim::debug(get_name(), ss.str());
    }
};
