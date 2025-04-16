/// @file models.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/digsim.hpp"

#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>

class Mux2to1 : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a;
    digsim::signal_t<bool> &b;
    digsim::signal_t<bool> &sel;
    digsim::signal_t<bool> out;

    Mux2to1(
        const std::string &name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::signal_t<bool> &sel_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , a(a_in)
        , b(b_in)
        , sel(sel_in)
        , out(name + "_out")
        , delay(_delay)
    {
        add_sensitivity(&Mux2to1::evaluate, a, b, sel);
    }

private:
    digsim::discrete_time_t delay;

    void evaluate()
    {
        bool result = sel.get() ? b.get() : a.get();

        std::stringstream ss;
        ss << "a:" << std::setw(1) << a.get() << ", ";
        ss << "b:" << std::setw(1) << b.get() << ", ";
        ss << "sel:" << std::setw(1) << sel.get() << " -> ";
        ss << "out:" << std::setw(1) << result;
        if (delay > 0) {
            ss << " (delayed by " << std::setw(1) << delay << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result, delay);
    }
};

class FullAdder : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a;
    digsim::signal_t<bool> &b;
    digsim::signal_t<bool> &cin;
    digsim::signal_t<bool> sum;
    digsim::signal_t<bool> cout;

    FullAdder(
        const std::string &name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::signal_t<bool> &cin_in,
        digsim::discrete_time_t _sum_delay  = 0,
        digsim::discrete_time_t _cout_delay = 0)
        : digsim::module_t(name)
        , a(a_in)
        , b(b_in)
        , cin(cin_in)
        , sum(name + "_sum")
        , cout(name + "_cout")
        , sum_delay(_sum_delay)
        , cout_delay(_cout_delay)
    {
        add_sensitivity(&FullAdder::evaluate, a, b, cin);
    }

private:
    digsim::discrete_time_t sum_delay;
    digsim::discrete_time_t cout_delay;
    void evaluate()
    {
        bool aval   = a.get();
        bool bval   = b.get();
        bool cinval = cin.get();

        bool s = aval ^ bval ^ cinval;
        bool c = (aval & bval) | (bval & cinval) | (aval & cinval);

        std::stringstream ss;
        ss << "a:" << std::setw(1) << aval << ", ";
        ss << "b:" << std::setw(1) << bval << ", ";
        ss << "cin:" << std::setw(1) << cinval << " -> ";
        ss << "sum:" << std::setw(1) << s;
        if (sum_delay > 0) {
            ss << " (delayed by " << std::setw(1) << sum_delay << ")";
        }
        ss << ", ";
        ss << "cout:" << std::setw(1) << c;
        if (cout_delay > 0) {
            ss << " (delayed by " << std::setw(1) << cout_delay << ")";
        }
        digsim::info(get_name(), ss.str());

        sum.set(s, sum_delay);
        cout.set(c, cout_delay);
    }
};

class NotGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &input;
    digsim::signal_t<bool> output;

    NotGate(const std::string &name, digsim::signal_t<bool> &input_in, digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , input(input_in)
        , output(name + "_out")
        , delay(_delay)
    {
        add_sensitivity(&NotGate::evaluate, input);
    }

private:
    digsim::discrete_time_t delay;

    void evaluate()
    {
        bool inv = !input.get();

        std::stringstream ss;
        ss << "input:" << std::setw(1) << input.get() << " -> ";
        ss << "output:" << std::setw(1) << inv;
        if (delay > 0) {
            ss << " (delayed by " << std::setw(1) << delay << ")";
        }
        digsim::info(get_name(), ss.str());

        output.set(inv, delay);
    }
};

class AndGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    AndGate(
        const std::string &name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , a(a_in)
        , b(b_in)
        , out(name + "_out")
        , delay(_delay)
    {
        add_sensitivity(&AndGate::evaluate, a, b);
    }

private:
    digsim::discrete_time_t delay;
    void evaluate()
    {
        bool result = a.get() && b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (delay > 0) {
            ss << " (delayed by " << delay << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result, delay);
    }
};

class OrGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    OrGate(
        const std::string &name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , a(a_in)
        , b(b_in)
        , out(name + "_out")
        , delay(_delay)
    {
        add_sensitivity(&OrGate::evaluate, a, b);
    }

private:
    digsim::discrete_time_t delay;
    void evaluate()
    {
        bool result = a.get() || b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (delay > 0) {
            ss << " (delayed by " << delay << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result, delay);
    }
};

class XorGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    XorGate(
        const std::string &name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , a(a_in)
        , b(b_in)
        , out(name + "_out")
        , delay(_delay)
    {
        add_sensitivity(&XorGate::evaluate, a, b);
    }

private:
    digsim::discrete_time_t delay;
    void evaluate()
    {
        bool result = a.get() ^ b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (delay > 0) {
            ss << " (delayed by " << delay << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result, delay);
    }
};

class NandGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    NandGate(
        const std::string &name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , a(a_in)
        , b(b_in)
        , out(name + "_out")
        , delay(_delay)
    {
        add_sensitivity(&NandGate::evaluate, a, b);
    }

private:
    digsim::discrete_time_t delay;
    void evaluate()
    {
        bool result = !(a.get() && b.get());

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (delay > 0) {
            ss << " (delayed by " << delay << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result, delay);
    }
};

class DFlipFlop : public digsim::module_t
{
public:
    digsim::signal_t<bool> &clk;
    digsim::signal_t<bool> &d;
    digsim::signal_t<bool> q;
    digsim::signal_t<bool> q_not;

    // Optional control signals
    digsim::signal_t<bool> *enable = nullptr;
    digsim::signal_t<bool> *reset  = nullptr;

    DFlipFlop(
        const std::string &name,
        digsim::signal_t<bool> &clk_in,
        digsim::signal_t<bool> &d_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(name)
        , clk(clk_in)
        , d(d_in)
        , q(name + "_q")
        , q_not(name + "_q_not")
        , delay(_delay)
    {
        add_sensitivity(&DFlipFlop::evaluate, clk);
        prev_clk = clk.get();
    }

    // Optional wiring functions
    void connect_enable(digsim::signal_t<bool> &en)
    {
        enable = &en;
        add_sensitivity(&DFlipFlop::evaluate, en);
    }

    void connect_reset(digsim::signal_t<bool> &rst)
    {
        reset = &rst;
        add_sensitivity(&DFlipFlop::evaluate, rst);
    }

private:
    digsim::discrete_time_t delay;
    bool prev_clk;

    void evaluate()
    {
        bool current = clk.get();

        if (!prev_clk && current) {
            bool reset_active  = reset && reset->get();
            bool enable_active = !enable || enable->get(); // default: enabled

            bool next_q = q.get(); // retain old value by default

            if (reset_active) {
                next_q = false;
            } else if (enable_active) {
                next_q = d.get();
            }

            std::stringstream ss;
            ss << "clk ↑, ";
            if (reset_active) {
                ss << "reset=1 -> q:0";
            } else if (!enable_active) {
                ss << "enable=0 -> q held";
            } else {
                ss << "d:" << d.get() << " -> q:" << next_q;
            }
            if (delay > 0) {
                ss << " (delayed by " << delay << ")";
            }
            digsim::info(get_name(), ss.str());

            q.set(next_q, delay);
            q_not.set(!next_q, delay);
        }

        prev_clk = current;
    }
};

class Counter4BitRipple : public digsim::module_t
{
public:
    digsim::signal_t<bool> q0, q1, q2, q3; // Outputs (LSB to MSB)

    Counter4BitRipple(
        const std::string &name,
        digsim::signal_t<bool> &clk,
        digsim::signal_t<bool> &enable,
        digsim::signal_t<bool> &reset,
        digsim::discrete_time_t delay = 1)
        : digsim::module_t(name)
        , q0(name + "_q0")
        , q1(name + "_q1")
        , q2(name + "_q2")
        , q3(name + "_q3")
        , d0(name + "_d0")
        , d1(name + "_d1")
        , d2(name + "_d2")
        , d3(name + "_d3")
        , dff0(name + "_dff0", clk, d0, delay)
        , dff1(name + "_dff1", dff0.q, d1, delay)
        , dff2(name + "_dff2", dff1.q, d2, delay)
        , dff3(name + "_dff3", dff2.q, d3, delay)
    {
        dff0.connect_enable(enable);
        dff1.connect_enable(enable);
        dff2.connect_enable(enable);
        dff3.connect_enable(enable);

        dff0.connect_reset(reset);
        dff1.connect_reset(reset);
        dff2.connect_reset(reset);
        dff3.connect_reset(reset);

        // Tie D inputs to Q_NOT (toggle)
        d0.set(dff0.q_not.get());
        d1.set(dff1.q_not.get());
        d2.set(dff2.q_not.get());
        d3.set(dff3.q_not.get());

        add_sensitivity(&Counter4BitRipple::evaluate, dff0.q, dff1.q, dff2.q, dff3.q);
        evaluate();
    }

private:
    digsim::signal_t<bool> d0, d1, d2, d3;
    DFlipFlop dff0, dff1, dff2, dff3;

    void evaluate()
    {
        q0.set(dff0.q.get());
        q1.set(dff1.q.get());
        q2.set(dff2.q.get());
        q3.set(dff3.q.get());

        unsigned q0_val = q0.get();
        unsigned q1_val = q1.get();
        unsigned q2_val = q2.get();
        unsigned q3_val = q3.get();

        unsigned val = (q3_val << 3) | (q2_val << 2) | (q1_val << 1) | q0_val;
        std::stringstream ss;
        ss << "→ binary: " << std::bitset<4>(val) << " | decimal: " << val;
        digsim::info(get_name(), ss.str());
    }
};
