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
    digsim::input_t<bool> clk;            ///< Clock signal.
    digsim::input_t<bool> reset;          ///< Reset signal.
    digsim::input_t<bs_address_t> addr;   ///< Address input.
    digsim::input_t<bs_data_t> data_in;   ///< Data input for write.
    digsim::input_t<bool> write_enable;   ///< Write enable signal.
    digsim::input_t<bs_phase_t> phase;    ///< Current CPU phase.
    digsim::output_t<bs_data_t> data_out; ///< Data output for read.

    ram_t(const std::string &_name)
        : digsim::module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , addr("addr", this)
        , data_in("data_in", this)
        , write_enable("write_enable", this)
        , phase("phase", this)
        , data_out("data_out", this)
    {
        memory.fill(0);

        ADD_SENSITIVITY(ram_t, evaluate, clk, reset, addr, data_in, write_enable, phase);
        ADD_PRODUCER(ram_t, evaluate, data_out);
    }

    /// @brief Read from memory.
    /// @param index The index to read from.
    /// @return The value at the specified index.
    uint16_t debug_read(std::size_t index) const
    {
        if (index >= memory.size()) {
            digsim::error(get_name(), "debug_read: out-of-bounds access to memory {}", index);
            return 0;
        }
        return static_cast<uint16_t>(memory[index].to_ulong());
    }

    /// @brief Debug write to memory.
    /// @param index The index to write to.
    /// @param value The value to write.
    void debug_write(std::size_t index, uint16_t value)
    {
        if (index >= memory.size()) {
            digsim::error(get_name(), "debug_write: out-of-bounds access to memory {}", index);
            return;
        }
        memory[index] = value;
        digsim::debug(get_name(), "debug_write: memory[{}] = 0x{:04X}", index, value);
    }

private:
    std::array<bs_data_t, RAM_SIZE> memory;

    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        if (reset.get()) {
            digsim::debug(get_name(), "Resetting RAM...");
            memory.fill(0);
            data_out.set(0);
            return;
        }
        // Read the inputs.
        const auto index     = addr.get().to_ulong();
        const auto wdata     = data_in.get();
        const auto phase_val = static_cast<phase_t>(phase.get().to_ulong());
        const bool write     = (phase_val == phase_t::WRITEBACK && write_enable.get());
        bs_data_t rdata      = 0;

        // Check the address.
        if (index >= RAM_SIZE) {
            digsim::error(get_name(), "Address out of bounds: 0x{:04X}", index);
            return;
        }

        // Perform the read or write operation.
        if (write) {
            if (index < RAM_SIZE) {
                memory[index] = wdata;
            }
        }
        if (index < RAM_SIZE) {
            rdata = memory[index];
        }
        // Set the output.
        data_out.set(rdata);

        digsim::debug(get_name(), "[{:5}] address: 0x{:04X}, data_in : 0x{:04X}, data_out : 0x{:04X}", (write ? "WR/RD" : "READ"), index, wdata.to_ulong(), rdata.to_ulong());
    }
};