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
        const std::string &_name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::signal_t<bool> &sel_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , a(a_in)
        , b(b_in)
        , sel(sel_in)
        , out(_name + "_out", false, _delay)
    {
        add_sensitivity(&Mux2to1::evaluate, a, b, sel);
        add_produces(&Mux2to1::evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = sel.get() ? b.get() : a.get();

        std::stringstream ss;
        ss << "a:" << std::setw(1) << a.get() << ", ";
        ss << "b:" << std::setw(1) << b.get() << ", ";
        ss << "sel:" << std::setw(1) << sel.get() << " -> ";
        ss << "out:" << std::setw(1) << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << std::setw(1) << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
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
        const std::string &_name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::signal_t<bool> &cin_in,
        digsim::discrete_time_t _sum_delay  = 0,
        digsim::discrete_time_t _cout_delay = 0)
        : digsim::module_t(_name)
        , a(a_in)
        , b(b_in)
        , cin(cin_in)
        , sum(_name + "_sum", false, _sum_delay)
        , cout(_name + "_cout", false, _cout_delay)
    {
        add_sensitivity(&FullAdder::evaluate, a, b, cin);
        add_produces(&FullAdder::evaluate, sum, cout);
    }

private:
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
        if (sum.get_delay() > 0) {
            ss << " (delayed by " << std::setw(1) << sum.get_delay() << ")";
        }
        ss << ", ";
        ss << "cout:" << std::setw(1) << c;
        if (cout.get_delay() > 0) {
            ss << " (delayed by " << std::setw(1) << cout.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        sum.set(s);
        cout.set(c);
    }
};

class NotGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &input;
    digsim::signal_t<bool> output;

    NotGate(const std::string &_name, digsim::signal_t<bool> &input_in, digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , input(input_in)
        , output(_name + "_out", false, _delay)
    {
        add_sensitivity(&NotGate::evaluate, input);
        add_produces(&NotGate::evaluate, output);
    }

    void evaluate()
    {
        bool inv = !input.get();

        std::stringstream ss;
        ss << "input:" << std::setw(1) << input.get() << " -> ";
        ss << "output:" << std::setw(1) << inv;
        if (output.get_delay() > 0) {
            ss << " (delayed by " << std::setw(1) << output.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        output.set(inv);
    }
};

class AndGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    AndGate(
        const std::string &_name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , a(a_in)
        , b(b_in)
        , out(_name + "_out", false, _delay)
    {
        add_sensitivity(&AndGate::evaluate, a, b);
        add_produces(&AndGate::evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = a.get() && b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

class OrGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    OrGate(
        const std::string &_name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , a(a_in)
        , b(b_in)
        , out(_name + "_out", false, _delay)
    {
        add_sensitivity(&OrGate::evaluate, a, b);
        add_produces(&OrGate::evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = a.get() || b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

class XorGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    XorGate(
        const std::string &_name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , a(a_in)
        , b(b_in)
        , out(_name + "_out", false, _delay)
    {
        add_sensitivity(&XorGate::evaluate, a, b);
        add_produces(&XorGate::evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = a.get() ^ b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

class NandGate : public digsim::module_t
{
public:
    digsim::signal_t<bool> &a, &b;
    digsim::signal_t<bool> out;

    NandGate(
        const std::string &_name,
        digsim::signal_t<bool> &a_in,
        digsim::signal_t<bool> &b_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , a(a_in)
        , b(b_in)
        , out(_name + "_out", false, _delay)
    {
        add_sensitivity(&NandGate::evaluate, a, b);
        add_produces(&NandGate::evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = !(a.get() && b.get());

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
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
        const std::string &_name,
        digsim::signal_t<bool> &clk_in,
        digsim::signal_t<bool> &d_in,
        digsim::discrete_time_t _delay = 0)
        : digsim::module_t(_name)
        , clk(clk_in)
        , d(d_in)
        , q(_name + "_q", false, _delay)
        , q_not(_name + "_q_not", false, _delay)
    {
        add_sensitivity(&DFlipFlop::evaluate, clk);
        add_produces(&DFlipFlop::evaluate, q, q_not);
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
            if (q.get_delay() > 0) {
                ss << " (delayed by " << q.get_delay() << ")";
            }
            digsim::info(get_name(), ss.str());

            q.set(next_q);
            q_not.set(!next_q);
        }

        prev_clk = current;
    }
};

class Counter4BitRipple : public digsim::module_t
{
public:
    digsim::signal_t<bool> q0, q1, q2, q3; // Outputs (LSB to MSB)

    Counter4BitRipple(
        const std::string &_name,
        digsim::signal_t<bool> &clk,
        digsim::signal_t<bool> &enable,
        digsim::signal_t<bool> &reset,
        digsim::discrete_time_t delay = 1)
        : digsim::module_t(_name)
        , q0(_name + "_q0")
        , q1(_name + "_q1")
        , q2(_name + "_q2")
        , q3(_name + "_q3")
        , d0(_name + "_d0")
        , d1(_name + "_d1")
        , d2(_name + "_d2")
        , d3(_name + "_d3")
        , dff0(_name + "_dff0", clk, d0, delay)
        , dff1(_name + "_dff1", dff0.q, d1, delay)
        , dff2(_name + "_dff2", dff1.q, d2, delay)
        , dff3(_name + "_dff3", dff2.q, d3, delay)
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
        add_produces(&Counter4BitRipple::evaluate, q0, q1, q2, q3);
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
