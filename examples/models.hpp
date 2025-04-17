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
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::input_t<bool> sel;
    digsim::output_t<bool> out;

    Mux2to1(const std::string &_name)
        : digsim::module_t(_name)
        , a(_name + "_a")
        , b(_name + "_b")
        , sel(_name + "_sel")
        , out(_name + "_out")
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
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::input_t<bool> cin;
    digsim::output_t<bool> sum;
    digsim::output_t<bool> cout;

    FullAdder(const std::string &_name)
        : digsim::module_t(_name)
        , a(_name + "_a")
        , b(_name + "_b")
        , cin(_name + "_cin")
        , sum(_name + "_sum")
        , cout(_name + "_cout")
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
    digsim::input_t<bool> in;
    digsim::output_t<bool> out;

    NotGate(const std::string &_name)
        : digsim::module_t(_name)
        , in(_name + "_in")
        , out(_name + "_out")
    {
        add_sensitivity(&NotGate::evaluate, in);
        add_produces(&NotGate::evaluate, out);
    }

    void evaluate()
    {
        bool inv = !in.get();

        std::stringstream ss;
        ss << "in:" << std::setw(1) << in.get() << " -> ";
        ss << "out:" << std::setw(1) << inv;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << std::setw(1) << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(inv);
    }
};

class AndGate : public digsim::module_t
{
public:
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::output_t<bool> out;

    AndGate(const std::string &_name)
        : digsim::module_t(_name)
        , a(_name + "_a")
        , b(_name + "_b")
        , out(_name + "_out")
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
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::output_t<bool> out;

    OrGate(const std::string &_name)
        : digsim::module_t(_name)
        , a(_name + "_a")
        , b(_name + "_b")
        , out(_name + "_out")
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
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::output_t<bool> out;

    XorGate(const std::string &_name)
        : digsim::module_t(_name)
        , a(_name + "_a")
        , b(_name + "_b")
        , out(_name + "_out")
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
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::output_t<bool> out;

    NandGate(const std::string &_name)
        : digsim::module_t(_name)
        , a(_name + "_a")
        , b(_name + "_b")
        , out(_name + "_out")
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
    digsim::input_t<bool> clk;
    digsim::input_t<bool> d;
    digsim::input_t<bool> enable;
    digsim::input_t<bool> reset;
    digsim::output_t<bool> q;
    digsim::output_t<bool> q_not;

    DFlipFlop(const std::string &_name)
        : digsim::module_t(_name)
        , clk(_name + "_clk")
        , d(_name + "_d")
        , enable(_name + "_enable")
        , reset(_name + "_reset")
        , q(_name + "_q")
        , q_not(_name + "_q_not")
    {
        add_sensitivity(&DFlipFlop::evaluate, clk, d, enable, reset);
        add_produces(&DFlipFlop::evaluate, q, q_not);
        prev_clk = clk.get();
    }

private:
    bool prev_clk;

    void evaluate()
    {
        bool current = clk.get();

        if (!prev_clk && current) {
            bool reset_active  = reset.bound() && reset.get();
            bool enable_active = !enable.bound() || enable.get();

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
