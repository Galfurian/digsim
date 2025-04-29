/// @file example10.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using DigSim.

#include "models/gates/not_gate.hpp"

class InnerModule : public digsim::module_t
{
public:
    digsim::input_t<bool> in;
    digsim::output_t<bool> out;

    NotGate inv;

    InnerModule(const std::string &_name)
        : digsim::module_t(_name)
        , in("in")
        , out("out")
        , inv("not_gate")
    {
        inv.set_parent(this);
        inv.in(in);
        inv.out(out);
    }
};

class TopModule : public digsim::module_t
{
public:
    digsim::input_t<bool> in;
    digsim::output_t<bool> out;

    InnerModule inner;

    TopModule(const std::string &_name)
        : digsim::module_t(_name)
        , in("in")
        , out("out")
        , inner("inner")
    {
        inner.set_parent(this);
        inner.in(in);
        inner.out(out);
    }
};

int main()
{
    digsim::signal_t<bool> s_in("s_in");
    digsim::signal_t<bool> s_out("s_out");

    TopModule top("top");
    top.in(s_in);
    top.out(s_out);

    digsim::dependency_graph.export_dot("example10.dot");

    digsim::info("Main", "=== Initializing simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation ===");

    s_in.set(true);
    digsim::scheduler.run();

    s_in.set(false);
    digsim::scheduler.run();

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}
