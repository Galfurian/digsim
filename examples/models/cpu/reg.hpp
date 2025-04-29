/// @file reg.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Register module.

#pragma once

#include <digsim/digsim.hpp>

#include <bitset>
#include <iomanip>
#include <sstream>

template <std::size_t N, std::size_t NUM_REGS = 8> class reg_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;
    digsim::input_t<uint8_t> addr_a;
    digsim::input_t<uint8_t> addr_b;
    digsim::input_t<uint8_t> addr_w;
    digsim::input_t<bool> write_enable;
    digsim::input_t<std::bitset<N>> data_in;

    digsim::output_t<std::bitset<N>> data_a;
    digsim::output_t<std::bitset<N>> data_b;

    reg_t(const std::string &_name)
        : module_t(_name)
        , clk("clk")
        , reset("reset")
        , addr_a("addr_a")
        , addr_b("addr_b")
        , addr_w("addr_w")
        , write_enable("write_enable")
        , data_in("data_in")
        , data_a("data_a")
        , data_b("data_b")
    {
        ADD_SENSITIVITY(reg_t, evaluate, clk, reset);
        ADD_PRODUCER(reg_t, evaluate, data_a, data_b);
    }

private:
    /// @brief An array of 8 registers, each of size N bits.
    std::array<std::bitset<N>, NUM_REGS> regs{};

    void evaluate()
    {
        // Only evaluate on the rising edge of the clock.
        if (!clk.posedge()) {
            return;
        }

        // Handle reset first.
        if (reset.get()) {
            // Set all registers to zero.
            for (auto &reg : regs) {
                reg.set(0);
            }
            data_a.set(0);
            data_b.set(0);
            return;
        }

        // Check if the addresses are within bounds.
        if (addr_a.get() >= NUM_REGS) {
            digsim::error("REG", "Address A out of bounds: {} >= {}", addr_a.get(), NUM_REGS);
            return;
        }
        if (addr_b.get() >= NUM_REGS) {
            digsim::error("REG", "Address B out of bounds: {} >= {}", addr_b.get(), NUM_REGS);
            return;
        }
        if (addr_w.get() >= NUM_REGS) {
            digsim::error("REG", "Address W out of bounds: {} >= {}", addr_w.get(), NUM_REGS);
            return;
        }

        // Read the values from the registers.
        data_a.set(regs[addr_a.get()]);
        data_b.set(regs[addr_b.get()]);

        // Write to the register if write_enable is high.
        if (write_enable.get()) {
            regs[addr_w.get()] = data_in.get();
        }
    }
};
