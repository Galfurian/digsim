/// @file ram.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple RAM (Random Access Memory) implementation.

#pragma once

#include <digsim/digsim.hpp>

#include <bitset>
#include <iomanip>
#include <sstream>

template <std::size_t N, std::size_t SIZE = 256> class ram_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;
    digsim::input_t<std::bitset<N>> addr;
    digsim::input_t<std::bitset<N>> data_in;
    digsim::input_t<bool> write_enable;
    digsim::output_t<std::bitset<N>> data_out;

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
    /// @brief An array of memory cells.
    std::array<std::bitset<N>, SIZE> mem{};

    void evaluate()
    {
        // Only evaluate on rising edge.
        if (!clk.posedge()) {
            return;
        }

        // Handle reset signal.
        if (reset.get()) {
            // Reset all memory cells to zero.
            for (auto &cell : mem) {
                cell.reset();
            }
            data_out.set(0);
            return;
        }

        // Get the address and check if it's within bounds.
        auto address = addr.get().to_ulong();
        if (address >= SIZE) {
            digsim::error("RAM", "Address out of bounds: {} >= {}", address, SIZE);
            return;
        }

        // Read or write data based on the write_enable signal.
        if (write_enable.get()) {
            mem[address] = data_in.get();
        }
        data_out.set(mem[address]);

        // Debugging output.
        std::stringstream ss;
        ss << "address: " << addr.get() << ", data_in: " << data_in.get() << ", write_enable: " << write_enable.get()
           << ", data_out: " << data_out.get();
        digsim::debug("RAM", ss.str());
    }
};
