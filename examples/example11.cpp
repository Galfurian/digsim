/// @file example11.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/cpu/alu.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Input signals.
    digsim ::signal_t<bs_data_t> a("a", 0, 0);
    digsim::signal_t<bs_data_t> b("b", 0, 0);
    digsim::signal_t<bs_opcode_t> op("op", 0, 0);
    digsim::signal_t<bool> clk("clk", false, 0);
    // Output signals.
    digsim::signal_t<bs_data_t> out("out", 0, 1);
    digsim::signal_t<bs_data_t> remainder("remainder", 0, 1);
    digsim::signal_t<bs_status_t> status("status", 0, 1);

    // Create the ALU.
    alu_t alu0("alu0");
    alu0.a(a);
    alu0.b(b);
    alu0.op(op);
    alu0.clk(clk);
    alu0.out(out);
    alu0.remainder(remainder);
    alu0.status(status);

    digsim::dependency_graph.export_dot("example11.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    digsim::info("Main", "=== Logic operations ===");

    clk.set(true);

    a.set(0b1100);
    b.set(0b1010);
    op.set(0); // AND
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(1); // OR
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(2); // XOR
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(3); // NOT (on A only)
    digsim::scheduler.run();

    digsim::info("Main", "=== Arithmetic operations ===");

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    a.set(0b0110);
    b.set(0b0011);

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(4); // ADD
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(5); // SUB
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(6); // MUL
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(7); // DIV
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(8); // MOD
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    // Test divide-by-zero
    b.set(0);
    op.set(7); // DIV by zero
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(8); // MOD by zero
    digsim::scheduler.run();

    digsim::info("Main", "=== Shift operations ===");

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    // Shifts
    a.set(0b0001);
    b.set(2); // shift amount

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(9); // SHL
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(10); // SHR
    digsim::scheduler.run();

    digsim::info("Main", "=== Comparison operations ===");

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    // Comparisons
    a.set(0b0101);
    b.set(0b0101);

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(11); // EQUAL (true)
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    a.set(0b0011);
    b.set(0b0110);
    op.set(11); // EQUAL (false)
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    op.set(12); // LESS THAN (true)
    digsim::scheduler.run();

    clk.set(false);
    digsim::scheduler.run();
    clk.set(true);

    a.set(0b1110);
    b.set(0b0010);
    op.set(12); // LESS THAN (false)
    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}
