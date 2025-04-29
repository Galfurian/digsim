/// @file ram.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple RAM (Random Access Memory) implementation.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class ram_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;
    digsim::input_t<bs_address_t> addr;
    digsim::input_t<bs_data_t> data_in;
    digsim::input_t<bool> write_enable;
    digsim::output_t<bs_data_t> data_out;

    ram_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , addr("addr", this)
        , data_in("data_in", this)
        , write_enable("write_enable", this)
        , data_out("data_out", this)
    {
        ADD_SENSITIVITY(ram_t, evaluate, clk, reset);
        ADD_PRODUCER(ram_t, evaluate, data_out);
    }

private:
    std::array<bs_data_t, MEM_SIZE> mem{};

    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        if (reset.get()) {
            for (auto &cell : mem) {
                cell.reset();
            }
            data_out.set(0);
            return;
        }

        auto address = addr.get().to_ulong();
        if (address >= MEM_SIZE) {
            digsim::error(get_name(), "Address out of bounds: {} >= {}", address, MEM_SIZE);
            return;
        }

        if (write_enable.get()) {
            mem[address] = data_in.get();
        }
        data_out.set(mem[address]);

        digsim::debug(
            get_name(), "address: 0x{:02X}, data_in: 0x{:02X}, data_out: 0x{:02X}, write_enable: {}", address,
            data_in.get().to_ulong(), data_out.get().to_ulong(), write_enable.get());
    }
};
