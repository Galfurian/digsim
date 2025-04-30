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

    /// @brief Debug read from memory.
    uint16_t debug_read(std::size_t index) const
    {
        if (index >= memory.size()) {
            digsim::error(get_name(), "debug_read: out-of-bounds access to memory {}", index);
            return 0;
        }
        return static_cast<uint16_t>(memory[index].to_ulong());
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

        const auto index     = addr.get().to_ulong();
        const auto wdata     = data_in.get();
        const auto phase_val = static_cast<phase_t>(phase.get().to_ulong());

        bool write = (phase_val == phase_t::WRITEBACK && write_enable.get());

        if (write) {
            if (index < RAM_SIZE) {
                memory[index] = wdata;
                digsim::debug(get_name(), "WRITE @ 0x{:04X} <= 0x{:02X}", index, wdata.to_ulong());
            } else {
                digsim::error(get_name(), "WRITE out-of-bounds: address 0x{:04X}", index);
            }
        }

        bs_data_t rdata = 0;
        if (index < RAM_SIZE) {
            rdata = memory[index];
        } else {
            digsim::error(get_name(), "READ out-of-bounds: address 0x{:04X}", index);
        }

        data_out.set(rdata);
        digsim::debug(
            get_name(), "address: 0x{:02X}, data_in: 0x{:02X}, data_out: 0x{:02X}, write_enable: {}", index,
            wdata.to_ulong(), rdata.to_ulong(), write_enable.get());
    }
};