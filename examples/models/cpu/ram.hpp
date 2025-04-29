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
    digsim::input_t<std::bitset<N>> addr;
    digsim::input_t<std::bitset<N>> data_in;
    digsim::input_t<bool> write_enable;
    digsim::output_t<std::bitset<N>> data_out;

    ram_t(const std::string &_name)
        : module_t(_name)
        , addr("addr")
        , data_in("data_in")
        , write_enable("write_enable")
        , data_out("data_out")
    {
        ADD_SENSITIVITY(ram_t, evaluate, addr, data_in, write_enable);
        ADD_PRODUCER(ram_t, evaluate, data_out);
    }

private:
    /// @brief An array of memory cells.
    std::array<std::bitset<N>, SIZE> mem{};

    void evaluate()
    {
        uint64_t address = addr.get().to_ulong();
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
