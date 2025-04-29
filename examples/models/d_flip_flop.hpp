/// @file d_flip_flop.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple D flip-flop model.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

class DFlipFlop : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;
    digsim::input_t<bool> d;
    digsim::input_t<bool> enable;
    digsim::input_t<bool> reset;
    digsim::output_t<bool> q;
    digsim::output_t<bool> q_not;

    DFlipFlop(const std::string &_name, digsim::module_t *_parent = nullptr)
        : digsim::module_t(_name, _parent)
        , clk("clk", this)
        , d("d", this)
        , enable("enable", this)
        , reset("reset", this)
        , q("q", this)
        , q_not("q_not", this)
    {
        ADD_SENSITIVITY(DFlipFlop, evaluate, clk);
        ADD_CONSUMER(DFlipFlop, evaluate, d, enable, reset);
        ADD_PRODUCER(DFlipFlop, evaluate, q, q_not);
    }

private:
    void evaluate()
    {
        if (!clk.posedge()) {
            return;
        }

        bool next_q = q.get();

        if (reset.get()) {
            next_q = false;
        } else if (enable.get()) {
            next_q = d.get();
        }

        std::stringstream ss;
        ss << "clk ↑, ";
        ss << "enable:" << enable.get() << ", ";
        ss << "reset:" << reset.get() << ", ";
        ss << "d:" << d.get() << " -> ";
        ss << "q:" << next_q;
        if (q.get_delay() > 0) {
            ss << " (+" << q.get_delay() << "t)";
        }
        ss << ", q_not:" << !next_q;
        if (q_not.get_delay() > 0) {
            ss << " (+" << q_not.get_delay() << "t)";
        }
        digsim::info(get_name(), ss.str());

        q.set(next_q);
        q_not.set(!next_q);
    }
};
