/// @file example2.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/mux2to1.hpp"
#include "models/gates/not_gate.hpp"

int main()
{
    digsim::logger.set_level(digsim::log_level_t::debug);

    // Inputs.
    digsim::signal_t<bool> a("a");
    digsim::signal_t<bool> b("b");
    digsim::signal_t<bool> sel("sel");
    digsim::signal_t<bool> mux_out("mux_out");
    digsim::signal_t<bool> inv1_out("inv1_out");
    digsim::signal_t<bool> inv2_out("inv2_out");

    // Models.
    Mux2to1<bool> mux("mux");
    mux.a(a);
    mux.b(b);
    mux.sel(sel);
    mux.out(mux_out);
    mux_out.set_delay(1);

    NotGate inv1("inv1");
    inv1.in(mux_out);
    inv1.out(inv1_out);
    inv1_out.set_delay(2);

    NotGate inv2("inv2");
    inv2.in(inv1_out);
    inv2.out(inv2_out);
    inv2_out.set_delay(3);

    digsim::probe_t<bool> probe1("probe1");
    probe1.in(mux_out);

    digsim::dependency_graph.export_dot("example2.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    a.set(0);
    b.set(1);
    sel.set(0); // select 'a' = 0

    digsim::scheduler.run();

    sel.set(1); // select 'b' = 1

    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");
}

