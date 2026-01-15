/// @file d_flip_flop.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple D flip-flop model.

#pragma once

#include <simcore/simcore.hpp>

#include <iomanip>
#include <sstream>

class DFlipFlop : public simcore::module_t
{
public:
    simcore::input_t<bool> clk;
    simcore::input_t<bool> d;
    simcore::input_t<bool> enable;
    simcore::input_t<bool> reset;
    simcore::output_t<bool> q;
    simcore::output_t<bool> q_not;

    DFlipFlop(const std::string &_name, simcore::module_t *_parent = nullptr)
        : simcore::module_t(_name, _parent)
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
        simcore::info(get_name(), ss.str());

        q.set(next_q);
        q_not.set(!next_q);
    }
};
