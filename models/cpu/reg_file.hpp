/// @file reg.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Register module.

#pragma once

#include <digsim/digsim.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

/// @file reg.hpp
/// @brief Register module with phase awareness.
/// @author ...

#pragma once

#include "cpu_defines.hpp"
#include <digsim/digsim.hpp>

#include <array>
#include <bitset>
#include <iomanip>
#include <sstream>

/// @brief Register module with phase awareness.
class reg_file_t : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;             ///< Clock signal.
    digsim::input_t<bool> reset;           ///< Reset signal.
    digsim::input_t<bs_phase_t> phase;     ///< CPU execution phase.
    digsim::input_t<bs_register_t> addr_a; ///< Address for register A.
    digsim::input_t<bs_register_t> addr_b; ///< Address for register B.
    digsim::input_t<bs_register_t> addr_w; ///< Address for writing register.
    digsim::input_t<bs_data_t> data_in;    ///< Data input to register file.
    digsim::input_t<bool> write_enable;    ///< Write enable signal.
    digsim::output_t<bs_data_t> data_a;    ///< Output data from register A.
    digsim::output_t<bs_data_t> data_b;    ///< Output data from register B.

    reg_file_t(const std::string &_name)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , phase("phase", this)
        , addr_a("addr_a", this)
        , addr_b("addr_b", this)
        , addr_w("addr_w", this)
        , data_in("data_in", this)
        , write_enable("write_enable", this)
        , data_a("data_a", this)
        , data_b("data_b", this)
    {
        ADD_SENSITIVITY(reg_file_t, evaluate, clk, reset);
        ADD_PRODUCER(reg_file_t, evaluate, data_a, data_b);
    }

    /// @brief Read register value by index (for test/debug only)
    uint16_t debug_read(std::size_t index) const
    {
        if (index >= regs.size()) {
            digsim::error(get_name(), "debug_read: out of bounds access to register {}", index);
            return 0;
        }
        return static_cast<uint16_t>(regs[index].to_ulong());
    }

    /// @brief Set register value by index (for test/debug only)
    void debug_write(std::size_t index, uint16_t value)
    {
        if (index >= regs.size()) {
            digsim::error(get_name(), "debug_write: out of bounds access to register {}", index);
            return;
        }
        regs[index] = value;

        digsim::debug(
            get_name(), "debug_write: writing 0x{:02X} to register 0x{:02X}, verify: 0x{:02X}.", value, index,
            regs[index].to_ulong());
    }

private:
    std::array<bs_data_t, NUM_REGS> regs{}; ///< Register storage

    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        // Handle reset
        if (reset.get()) {
            digsim::debug(get_name(), "Resetting registers...");
            for (auto &reg : regs) {
                reg.reset();
            }
            data_a.set(0);
            data_b.set(0);
            return;
        }

        const auto current_phase = static_cast<phase_t>(phase.get().to_ulong());
        const auto u_addr_a      = addr_a.get().to_ulong();
        const auto u_addr_b      = addr_b.get().to_ulong();
        const auto u_addr_w      = addr_w.get().to_ulong();

        if (u_addr_a >= NUM_REGS || u_addr_b >= NUM_REGS || u_addr_w >= NUM_REGS) {
            digsim::error(get_name(), "Register address out of bounds");
            return;
        }

        // Always expose read values
        data_a.set(regs[u_addr_a]);
        data_b.set(regs[u_addr_b]);

        // Only perform write during WRITEBACK phase
        if (current_phase == phase_t::WRITEBACK && write_enable.get()) {
            regs[u_addr_w] = data_in.get();
            digsim::debug(get_name(), "WRITEBACK phase: wrote 0x{:04X} to r{}", data_in.get().to_ulong(), u_addr_w);
        }

        digsim::debug(
            get_name(),
            "addr_a: 0x{:02X}, data_a: 0x{:02X}, "
            "addr_b: 0x{:02X}, data_b: 0x{:02X}, "
            "addr_w: 0x{:02X}, data_w: 0x{:02X}, phase: {}",
            u_addr_a, regs[u_addr_a].to_ulong(), //
            u_addr_b, regs[u_addr_b].to_ulong(), //
            u_addr_w, data_in.get().to_ulong(), static_cast<int>(current_phase));
    }
};
