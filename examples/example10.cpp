/// @file digital_circuit.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief
/// @version 0.1
/// @date 2024-05-13
///
/// @copyright Copyright (c) 2024
///

#include "digsim/digsim.hpp"
#include "models.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    digsim::info("Main", "=== Creating modules ===");

    digsim::signal_t<std::bitset<4>> a("a");
    digsim::signal_t<std::bitset<4>> b("b");
    digsim::signal_t<uint8_t> op("op");
    digsim::signal_t<bool> clk("clk");

    digsim::signal_t<std::bitset<8>> out("out");
    digsim::signal_t<bool> zero_division("zero_division");

    digsim::clock_t clk0("clk0");
    clk0.out(clk);

    alu_t<4> alu0("alu0");
    alu0.a(a);
    alu0.b(b);
    alu0.op(op);
    alu0.clk(clk);
    alu0.out(out);
    alu0.zero_division(zero_division);

    digsim::dependency_graph.export_dot("example10.dot");

    digsim::info("Main", "=== Initializing simulation ===");
    digsim::scheduler.initialize();
    digsim::info("Main", "=== Running simulation ===");
    digsim::scheduler.run(5);
    digsim::info("Main", "=== Simulation finished ===");
    return 0;
}
