/// @file example2.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/mux2to1.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    // Inputs.
    simcore::signal_t<bool> a("a");
    simcore::signal_t<bool> b("b");
    simcore::signal_t<bool> sel("sel");
    simcore::signal_t<bool> mux_out("mux_out");
    simcore::signal_t<bool> inv1_out("inv1_out");
    simcore::signal_t<bool> inv2_out("inv2_out");

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

    Probe<bool> probe1("probe1");
    probe1.in(mux_out);

    simcore::dependency_graph.export_dot("example2.dot");

    simcore::info("Main", "=== Initializing simulation ===");

    simcore::scheduler.initialize();

    simcore::info("Main", "=== Running simulation ===");

    a.set(0);
    b.set(1);
    sel.set(0); // select 'a' = 0

    simcore::scheduler.run();

    sel.set(1); // select 'b' = 1

    simcore::scheduler.run();

    simcore::info("Main", "=== Simulation finished ===");
}

