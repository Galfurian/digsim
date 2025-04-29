/// @file reg.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Register module.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class reg_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;
    digsim::input_t<bs_register_t> addr_a;
    digsim::input_t<bs_register_t> addr_b;
    digsim::input_t<bs_register_t> addr_w;
    digsim::input_t<bs_data_t> data_in;
    digsim::input_t<bool> write_enable;
    digsim::output_t<bs_data_t> data_a;
    digsim::output_t<bs_data_t> data_b;

    reg_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , addr_a("addr_a", this)
        , addr_b("addr_b", this)
        , addr_w("addr_w", this)
        , data_in("data_in", this)
        , write_enable("write_enable", this)
        , data_a("data_a", this)
        , data_b("data_b", this)
    {
        ADD_SENSITIVITY(reg_t, evaluate, clk, reset);
        ADD_PRODUCER(reg_t, evaluate, data_a, data_b);
    }

private:
    std::array<bs_data_t, NUM_REGS> regs{};

    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        if (reset.get()) {
            for (auto &reg : regs) {
                reg.reset();
            }
            data_a.set(0);
            data_b.set(0);
            return;
        }

        auto u_addr_a = addr_a.get().to_ulong();
        auto u_addr_b = addr_b.get().to_ulong();
        auto u_addr_w = addr_w.get().to_ulong();

        if (u_addr_a >= NUM_REGS) {
            digsim::error(get_name(), "Address A out of bounds: {} >= {}", u_addr_a, NUM_REGS);
            return;
        }
        if (u_addr_b >= NUM_REGS) {
            digsim::error(get_name(), "Address B out of bounds: {} >= {}", u_addr_b, NUM_REGS);
            return;
        }
        if (u_addr_w >= NUM_REGS) {
            digsim::error(get_name(), "Address W out of bounds: {} >= {}", u_addr_w, NUM_REGS);
            return;
        }

        data_a.set(regs[u_addr_a]);
        data_b.set(regs[u_addr_b]);

        if (write_enable.get()) {
            regs[u_addr_w] = data_in.get();
        }

        digsim::debug(
            get_name(), "addr_a: {}, data_a: {}, addr_b: {}, data_b: {}", u_addr_a, regs[u_addr_a], u_addr_b,
            regs[u_addr_b]);
    }
};
