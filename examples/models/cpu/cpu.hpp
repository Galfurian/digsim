/// @file cpu.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple CPU model.

#pragma once

#include "alu.hpp"
#include "control_unit.hpp"
#include "multiplexer.hpp"
#include "pc.hpp"
#include "ram.hpp"
#include "reg.hpp"
#include "rom.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

template <size_t N> class cpu_t : public digsim::module_t
{
public:
    // Ports
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;

    cpu_t(const std::string &_name, const std::vector<uint64_t> &_rom_contents)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , pc("pc")
        , rom("rom", _rom_contents)
        , control("control")
        , reg("reg")
        , alu("alu")
        , ram("ram")
        , multiplexer("multiplexer")
        , pc_to_rom("pc_to_rom")
        , rom_to_control("rom_to_control")
        , control_to_aluop("control_to_aluop")
        , control_to_regwrite("control_to_regwrite")
        , control_to_memwrite("control_to_memwrite")
        , control_to_memtoreg("control_to_memtoreg")
        , reg_a("reg_a")
        , reg_b("reg_b")
        , alu_out("alu_out")
        , alu_remainder("alu_remainder")
        , alu_status("alu_status")
        , ram_out("ram_out")
        , multiplexer_out("multiplexer_out")
        , signal_dummy_false("signal_dummy_false")
        , signal_dummy_zero("signal_dummy_zero")
        , rs("rs")
        , rt("rt")
        , rd("rd")
    {
        // === Program Counter ===
        pc.set_parent(this);
        pc.clk(clk);
        pc.reset(reset);
        pc.load(signal_dummy_false);     // No branching yet
        pc.next_addr(signal_dummy_zero); // No jumps yet
        pc.addr(pc_to_rom);

        // === ROM ===
        rom.set_parent(this);
        rom.addr(pc_to_rom);
        rom.instruction(rom_to_control);

        // === Control Unit ===
        control.set_parent(this);
        control.instruction(rom_to_control);
        control.alu_op(control_to_aluop);
        control.reg_write(control_to_regwrite);
        control.mem_write(control_to_memwrite);
        control.mem_to_reg(control_to_memtoreg);

        // === Register File ===
        reg.set_parent(this);
        reg.clk(clk);
        reg.addr_a(rs);
        reg.addr_b(rt);
        reg.addr_w(rd);
        reg.write_enable(control_to_regwrite);
        reg.data_in(multiplexer_out);
        reg.data_a(reg_a);
        reg.data_b(reg_b);

        // === ALU ===
        alu.set_parent(this);
        alu.clk(clk);                 // Clock
        alu.a(reg_a);                 // A operand from reg A
        alu.b(reg_b);                 // B operand from reg B
        alu.op(control_to_aluop);     // ALU operation from Control Unit
        alu.out(alu_out);             // ALU result
        alu.remainder(alu_remainder); // Optional remainder output
        alu.status(alu_status);       // ALU flags output

        // === RAM ===
        ram.set_parent(this);
        ram.clk(clk);       // Clock
        ram.addr(alu_out);  // Address comes from ALU result
        ram.data_in(reg_b); // Data to store comes from reg B
        ram.write_enable(control_to_memwrite);
        ram.data_out(ram_out);

        // === Write-Back Mux ===
        multiplexer.set_parent(this);
        multiplexer.a(alu_out);
        multiplexer.b(ram_out);
        multiplexer.sel(control_to_memtoreg);
        multiplexer.out(multiplexer_out);

        ADD_SENSITIVITY(cpu_t, decode_instruction, rom_to_control);
        ADD_PRODUCER(cpu_t, decode_instruction, rs, rt, rd);
    }

private:
    void decode_instruction()
    {
        // Decode instruction fields
        // Assuming:
        // [15:12] = opcode
        // [11:8]  = rs (source A)
        // [7:4]   = rt (source B)
        // [3:0]   = rd (dest or immediate)

        uint16_t instr = static_cast<uint16_t>(rom_to_control.get().to_ulong());
        rs.set((instr >> 8) & 0xF); // bits [11:8]
        rt.set((instr >> 4) & 0xF); // bits [7:4]
        rd.set((instr >> 0) & 0xF); // bits [3:0]
    }

    // Internal modules
    pc_t<N> pc;
    rom_t<N> rom;
    control_unit_t<N> control;
    reg_t<N> reg;
    alu_t<N> alu;
    ram_t<N> ram;
    multiplexer_t<std::bitset<N>> multiplexer;

    // Internal signals (wires)
    digsim::signal_t<std::bitset<N>> pc_to_rom;
    digsim::signal_t<std::bitset<N>> rom_to_control;
    digsim::signal_t<std::bitset<4>> control_to_aluop;
    digsim::signal_t<bool> control_to_regwrite;
    digsim::signal_t<bool> control_to_memwrite;
    digsim::signal_t<bool> control_to_memtoreg;
    digsim::signal_t<std::bitset<N>> reg_a;
    digsim::signal_t<std::bitset<N>> reg_b;
    digsim::signal_t<std::bitset<N>> alu_out;
    digsim::signal_t<std::bitset<N>> alu_remainder;
    digsim::signal_t<std::bitset<4>> alu_status;
    digsim::signal_t<std::bitset<N>> ram_out;
    digsim::signal_t<std::bitset<N>> multiplexer_out;

    digsim::signal_t<bool> signal_dummy_false;
    digsim::signal_t<std::bitset<N>> signal_dummy_zero;

    digsim::signal_t<uint8_t> rs;
    digsim::signal_t<uint8_t> rt;
    digsim::signal_t<uint8_t> rd;
};
