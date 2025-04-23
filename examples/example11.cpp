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
    digsim::signal_t<bool> s_in("s_in");
    digsim::signal_t<bool> s_out("s_out");

    TopModule top("top");
    top.in(s_in);
    top.out(s_out);

    digsim::dependency_graph.export_dot("example11.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    digsim::scheduler.initialize();

    s_in.set(true);
    digsim::scheduler.run();

    s_in.set(false);
    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}
