/// @file example10.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/gates/not_gate.hpp"
#include "models/probe.hpp"

class InnerModule : public simcore::module_t
{
public:
    simcore::input_t<bool> in;
    simcore::output_t<bool> out0;
    simcore::output_t<bool> out1;

    NotGate not0;
    NotGate not1;

    InnerModule(const std::string &_name)
        : simcore::module_t(_name)
        , in("in", this)
        , out0("out0", this)
        , out1("out1", this)
        , not0("not0")
        , not1("not1")
    {
        not0.set_parent(this);
        not0.in(in);
        not0.out(out0);

        not1.set_parent(this);
        not1.in(in);
        not1.out(out1);
    }
};

class TopModule : public simcore::module_t
{
public:
    simcore::input_t<bool> in;
    simcore::output_t<bool> out0;
    simcore::output_t<bool> out1;

    InnerModule inner;

    TopModule(const std::string &_name)
        : simcore::module_t(_name)
        , in("in")
        , out0("out0")
        , out1("out1")
        , inner("inner")
    {
        inner.set_parent(this);
        inner.in(in);
        inner.out0(out0);
        inner.out1(out1);
    }
};

int main()
{
    simcore::set_log_level(simcore::log_level_t::trace);

    simcore::signal_t<bool> s_in("s_in");
    simcore::signal_t<bool> s_out0("s_out0");
    simcore::signal_t<bool> s_out1("s_out1");

    TopModule top("top");
    top.in(s_in);
    top.out0(s_out0);
    top.out1(s_out1);

    simcore::dependency_graph.export_dot("example10.dot");

    simcore::info("Main", "=== Initializing simulation ===");

    simcore::scheduler.initialize();

    simcore::info("Main", "=== Running simulation ===");

    s_in.set(true);
    simcore::scheduler.run();

    s_in.set(false);
    simcore::scheduler.run();

    simcore::info("Main", "=== Simulation finished ===");

    return 0;
}
