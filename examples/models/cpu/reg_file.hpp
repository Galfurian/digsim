/// @file reg.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Register module.

#pragma once

#include <simcore/simcore.hpp>

#include "cpu_defines.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

/// @brief Register module with phase awareness.
class reg_file_t : public simcore::module_t
{
public:
    simcore::input_t<bool> clk;             ///< Clock signal.
    simcore::input_t<bool> reset;           ///< Reset signal.
    simcore::input_t<bs_phase_t> phase;     ///< CPU execution phase.
    simcore::input_t<bs_register_t> addr_a; ///< Address for register A.
    simcore::input_t<bs_register_t> addr_b; ///< Address for register B.
    simcore::input_t<bs_register_t> addr_w; ///< Address for writing register.
    simcore::input_t<bs_data_t> data_in;    ///< Data input to register file.
    simcore::input_t<bool> write_enable;    ///< Write enable signal.
    simcore::output_t<bs_data_t> data_a;    ///< Output data from register A.
    simcore::output_t<bs_data_t> data_b;    ///< Output data from register B.

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
            simcore::error(get_name(), "debug_read: out of bounds access to register 0x{:04X}", index);
            return 0;
        }
        return static_cast<uint16_t>(regs[index].to_ulong());
    }

    /// @brief Set register value by index (for test/debug only)
    void debug_write(std::size_t index, uint16_t value)
    {
        if (index >= regs.size()) {
            simcore::error(get_name(), "debug_write: out of bounds access to register 0x{:04X}", index);
            return;
        }
        regs[index] = value;

        simcore::debug(
            get_name(), "debug_write: writing 0x{:04X} to register 0x{:04X}, verify: 0x{:04X}.", value, index,
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
            simcore::debug(get_name(), "Resetting registers...");
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

        // Check the register addresses.
        if (u_addr_a >= NUM_REGS) {
            simcore::error(get_name(), "Register A address is out of bounds: 0x{:04X}", u_addr_a);
            return;
        }
        if (u_addr_b >= NUM_REGS) {
            simcore::error(get_name(), "Register B address is out of bounds: 0x{:04X}", u_addr_b);
            return;
        }
        if (u_addr_w >= NUM_REGS) {
            simcore::error(get_name(), "Register W address is out of bounds: 0x{:04X}", u_addr_w);
            return;
        }

        // Always expose read values,
        data_a.set(regs[u_addr_a]);
        data_b.set(regs[u_addr_b]);

        // Only perform write during WRITEBACK phase
        if (current_phase == phase_t::WRITEBACK && write_enable.get()) {
            regs[u_addr_w] = data_in.get();
        }

        simcore::debug(
            get_name(),
            "[{:5}] "
            "A: 0x{:04X} (out: 0x{:04X}), "
            "B: 0x{:04X} (out: 0x{:04X}), "
            "W: 0x{:04X} (in: 0x{:04X}), "
            "phase: {}",
            (current_phase == phase_t::WRITEBACK) ? "RD/WR" : "READ",
            u_addr_a, regs[u_addr_a].to_ulong(),
            u_addr_b, regs[u_addr_b].to_ulong(),
            u_addr_w, data_in.get().to_ulong(),
            static_cast<int>(current_phase));
    }
};
