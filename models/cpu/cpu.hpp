/// @file cpu.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple CPU model.

#pragma once

#include "alu.hpp"
#include "control_unit.hpp"
#include "decoder.hpp"
#include "multiplexer.hpp"
#include "phase_fsm.hpp"
#include "program_counter.hpp"
#include "ram.hpp"
#include "reg_file.hpp"
#include "rom.hpp"

#include <bitset>
#include <iomanip>
#include <sstream>

class cpu_t : public digsim::module_t
{
public:
    // Ports
    digsim::input_t<bool> clk;
    digsim::input_t<bool> reset;

    // Submodules.
    program_counter_t pc;
    rom_t rom;
    decoder_t decoder;
    control_unit_t control;
    reg_file_t reg;
    alu_t alu;
    ram_t ram;
    multiplexer_t<bs_data_t> multiplexer;
    multiplexer_t<bs_register_t> reg_write_mux;
    phase_fsm_t phase_fsm;

    cpu_t(const std::string &_name, const std::vector<uint16_t> &_rom_contents)
        : module_t(_name)
        , clk("clk", this)
        , reset("reset", this)
        , pc("pc")
        , rom("rom", _rom_contents)
        , decoder("decoder")
        , control("control")
        , reg("reg")
        , alu("alu")
        , ram("ram")
        , multiplexer("multiplexer")
        , reg_write_mux("reg_write_mux")
        , phase_fsm("phase_fsm")
        , program_counter_to_rom("program_counter_to_rom")
        , rom_to_decoder("rom_to_decoder")
        , decoder_to_control_opcode("decoder_to_control_opcode")
        , decoder_rs("decoder_rs")
        , decoder_rt("decoder_rt")
        , decoder_flag("decoder_flag")
        , control_to_aluop("control_to_aluop")
        , control_phase("control_phase")
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
        , reg_write_mux_out("reg_write_mux_out")
        , control_select_rt_as_dest("control_select_rt_as_dest")
        , pc_load("pc_load")
        , pc_next_addr("pc_next_addr")
    {
        // === Program Counter ===
        pc.set_parent(this);
        pc.clk(clk);
        pc.reset(reset);
        pc.load(pc_load);
        pc.next_addr(pc_next_addr);
        pc.phase(control_phase);
        pc.addr(program_counter_to_rom);

        // === ROM ===
        rom.set_parent(this);
        rom.addr(program_counter_to_rom);
        rom.instruction(rom_to_decoder);

        // === Instruction Decoder ===
        decoder.set_parent(this);
        decoder.instruction(rom_to_decoder);
        decoder.phase(control_phase);
        decoder.opcode(decoder_to_control_opcode);
        decoder.rs(decoder_rs);
        decoder.rt(decoder_rt);
        decoder.flag(decoder_flag);

        // === Phase FSM ===
        phase_fsm.set_parent(this);
        phase_fsm.clk(clk);
        phase_fsm.reset(reset);
        phase_fsm.phase(control_phase);

        // === Control Unit ===
        control.set_parent(this);
        control.opcode(decoder_to_control_opcode);
        control.alu_op(control_to_aluop);
        control.phase(control_phase);
        control.reg_write(control_to_regwrite);
        control.mem_write(control_to_memwrite);
        control.mem_to_reg(control_to_memtoreg);
        control.rt_as_dest(control_select_rt_as_dest);

        // === Register File ===
        reg.set_parent(this);
        reg.clk(clk);
        reg.reset(reset);
        reg.phase(control_phase);
        reg.addr_a(decoder_rs);
        reg.addr_b(decoder_rt);
        reg.addr_w(reg_write_mux_out);
        reg.write_enable(control_to_regwrite);
        reg.data_in(multiplexer_out);
        reg.data_a(reg_a);
        reg.data_b(reg_b);

        // === ALU ===
        alu.set_parent(this);
        alu.clk(clk);
        alu.reset(reset);
        alu.a(reg_a);
        alu.b(reg_b);
        alu.op(control_to_aluop);
        alu.out(alu_out);
        alu.phase(control_phase);
        alu.remainder(alu_remainder);
        alu.status(alu_status);

        // === RAM ===
        ram.set_parent(this);
        ram.clk(clk);
        ram.reset(reset);
        ram.addr(alu_out);
        ram.data_in(reg_b);
        ram.write_enable(control_to_memwrite);
        ram.phase(control_phase);
        ram.data_out(ram_out);

        // === Write-Back MUX ===
        multiplexer.set_parent(this);
        multiplexer.a(alu_out);
        multiplexer.b(ram_out);
        multiplexer.sel(control_to_memtoreg);
        multiplexer.out(multiplexer_out);

        // === Register Write MUX ===
        reg_write_mux.set_parent(this);
        reg_write_mux.sel(control_select_rt_as_dest);
        reg_write_mux.a(decoder_rs);
        reg_write_mux.b(decoder_rt);
        reg_write_mux.out(reg_write_mux_out);
    }

private:
    // === Internal signals ===
    digsim::signal_t<bs_address_t> program_counter_to_rom;
    digsim::signal_t<bs_instruction_t> rom_to_decoder;
    digsim::signal_t<bs_opcode_t> decoder_to_control_opcode;
    digsim::signal_t<bs_register_t> decoder_rs;
    digsim::signal_t<bs_register_t> decoder_rt;
    digsim::signal_t<bool> decoder_flag;
    digsim::signal_t<bs_opcode_t> control_to_aluop;
    digsim::signal_t<bs_phase_t> control_phase;
    digsim::signal_t<bool> control_to_regwrite;
    digsim::signal_t<bool> control_to_memwrite;
    digsim::signal_t<bool> control_to_memtoreg;
    digsim::signal_t<bs_address_t> reg_a;
    digsim::signal_t<bs_address_t> reg_b;
    digsim::signal_t<bs_address_t> alu_out;
    digsim::signal_t<bs_address_t> alu_remainder;
    digsim::signal_t<bs_status_t> alu_status;
    digsim::signal_t<bs_address_t> ram_out;
    digsim::signal_t<bs_address_t> multiplexer_out;
    digsim::signal_t<bs_register_t> reg_write_mux_out;
    digsim::signal_t<bool> control_select_rt_as_dest;
    digsim::signal_t<bool> pc_load;
    digsim::signal_t<bs_address_t> pc_next_addr;
};
