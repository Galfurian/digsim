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
        , a("a")
        , b("b")
        , sel("sel")
        , out("out")
    {
        ADD_SENSITIVITY(Mux2to1, evaluate, a, b, sel);
        ADD_PRODUCES(Mux2to1, evaluate, out);
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
        , a("a")
        , b("b")
        , cin("cin")
        , sum("sum")
        , cout("cout")
    {
        ADD_SENSITIVITY(FullAdder, evaluate, a, b, cin);
        ADD_PRODUCES(FullAdder, evaluate, sum, cout);
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
        , in("in")
        , out("out")
    {
        ADD_SENSITIVITY(NotGate, evaluate, in);
        ADD_PRODUCES(NotGate, evaluate, out);
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
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(AndGate, evaluate, a, b);
        ADD_PRODUCES(AndGate, evaluate, out);
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
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(OrGate, evaluate, a, b);
        ADD_PRODUCES(OrGate, evaluate, out);
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
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(XorGate, evaluate, a, b);
        ADD_PRODUCES(XorGate, evaluate, out);
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
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(NandGate, evaluate, a, b);
        ADD_PRODUCES(NandGate, evaluate, out);
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
        , clk("clk")
        , d("d")
        , enable("enable")
        , reset("reset")
        , q("q")
        , q_not("q_not")
    {
        ADD_SENSITIVITY(DFlipFlop, evaluate, clk, d);
        ADD_PRODUCES(DFlipFlop, evaluate, q, q_not);
    }

    void bind_reset(digsim::signal_t<bool> &reset_signal)
    {
        reset(reset_signal);
        ADD_SENSITIVITY(DFlipFlop, evaluate, reset);
    }

    void bind_enable(digsim::signal_t<bool> &enable_signal)
    {
        enable(enable_signal);
        ADD_SENSITIVITY(DFlipFlop, evaluate, enable);
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

template <size_t N> class alu_and_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<N>> out;

    alu_and_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(alu_and_t, evaluate, a, b);
        ADD_PRODUCES(alu_and_t, evaluate, out);
    }

private:
    void evaluate()
    {
        const auto a_val      = a.get();
        const auto b_val      = b.get();
        std::bitset<N> result = a_val & b_val;

        std::stringstream ss;
        ss << "a:" << a_val << ", b:" << b_val << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

template <size_t N> class alu_or_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<N>> out;

    alu_or_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(alu_or_t, evaluate, a, b);
        ADD_PRODUCES(alu_or_t, evaluate, out);
    }

private:
    void evaluate()
    {
        const auto a_val      = a.get();
        const auto b_val      = b.get();
        std::bitset<N> result = a_val | b_val;

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

template <size_t N> class alu_xor_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<N>> out;

    alu_xor_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(alu_xor_t, evaluate, a, b);
        ADD_PRODUCES(alu_xor_t, evaluate, out);
    }

private:
    void evaluate()
    {
        const auto a_val      = a.get();
        const auto b_val      = b.get();
        std::bitset<N> result = a_val ^ b_val;

        std::stringstream ss;
        ss << "a:" << a.get() << ", b:" << b.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

template <size_t N> class alu_not_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::output_t<std::bitset<N>> out;

    alu_not_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , out("out")
    {
        ADD_SENSITIVITY(alu_not_t, evaluate, a);
        ADD_PRODUCES(alu_not_t, evaluate, out);
    }

private:
    void evaluate()
    {
        const auto a_val      = a.get();
        std::bitset<N> result = ~a_val;

        std::stringstream ss;
        ss << "a:" << a.get() << " -> out:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};

template <size_t N> class alu_sum_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<N>> out;
    digsim::output_t<bool> carry;

    alu_sum_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
        , carry("carry")
    {
        ADD_SENSITIVITY(alu_sum_t, evaluate, a, b);
        ADD_PRODUCES(alu_sum_t, evaluate, out, carry);
    }

private:
    void evaluate()
    {
        const auto a_val = a.get();
        const auto b_val = b.get();

        unsigned long a_u   = a_val.to_ulong();
        unsigned long b_u   = b_val.to_ulong();
        unsigned long sum_u = a_u + b_u;

        std::bitset<N + 1> full_sum(sum_u);
        std::bitset<N> result(sum_u & ((1UL << N) - 1));

        out.set(result);

        std::stringstream ss;
        ss << "a:" << a_val << ", b:" << b_val << " -> sum:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());
    }
};

template <size_t N> class alu_sub_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<N>> out;
    digsim::output_t<bool> borrow;

    alu_sub_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
        , borrow("borrow")
    {
        ADD_SENSITIVITY(alu_sub_t, evaluate, a, b);
        ADD_PRODUCES(alu_sub_t, evaluate, out, borrow);
    }

private:
    void evaluate()
    {
        const auto a_val = a.get();
        const auto b_val = b.get();

        unsigned long a_u    = a_val.to_ulong();
        unsigned long b_u    = b_val.to_ulong();
        unsigned long diff_u = a_u - b_u;

        std::bitset<N> diff(diff_u);
        bool borrow_out = a_u < b_u;

        out.set(diff);
        borrow.set(borrow_out);

        std::stringstream ss;
        ss << "a:" << a_val << ", b:" << b_val << " -> diff:" << diff << ", borrow:" << borrow_out;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());
    }
};

template <size_t N> class alu_mul_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<2 * N>> out;

    alu_mul_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
    {
        ADD_SENSITIVITY(alu_mul_t, evaluate, a, b);
        ADD_PRODUCES(alu_mul_t, evaluate, out);
    }

private:
    void evaluate()
    {
        const auto a_val = a.get();
        const auto b_val = b.get();

        auto a_u   = a_val.to_ullong();
        auto b_u   = b_val.to_ullong();
        auto mul_u = a_u * b_u;

        std::bitset<2 * N> result(mul_u);

        out.set(result);

        std::stringstream ss;
        ss << "a:" << a_val << ", b:" << b_val << " -> mul:" << result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());
    }
};

template <size_t N> class alu_div_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::output_t<std::bitset<N>> out;
    digsim::output_t<std::bitset<N>> remainder;
    digsim::output_t<bool> zero_division;

    alu_div_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a")
        , b("b")
        , out("out")
        , remainder("remainder")
        , zero_division("zero_division")
    {
        ADD_SENSITIVITY(alu_div_t, evaluate, a, b);
        ADD_PRODUCES(alu_div_t, evaluate, out, remainder, zero_division);
    }

private:
    void evaluate()
    {
        const auto a_val = a.get();
        const auto b_val = b.get();

        auto a_u = a_val.to_ullong();
        auto b_u = b_val.to_ullong();

        if (b_u == 0) {
            zero_division.set(true);
            out.set(0);
            digsim::info(get_name(), "Division by zero");
            return;
        }

        auto div_u = a_u / b_u;
        auto rem_u = a_u % b_u;

        std::bitset<N> div_result(div_u);
        std::bitset<N> rem_result(rem_u);

        out.set(div_result);
        remainder.set(rem_result);
        zero_division.set(false);

        std::stringstream ss;
        ss << "a:" << a_val << ", b:" << b_val << " -> div:" << div_result << ", rem:" << rem_result;
        if (out.get_delay() > 0) {
            ss << " (delayed by " << out.get_delay() << ")";
        }
        digsim::info(get_name(), ss.str());
    }
};

template <size_t N> class alu_t : public digsim::module_t
{
public:
    digsim::input_t<std::bitset<N>> a;
    digsim::input_t<std::bitset<N>> b;
    digsim::input_t<uint8_t> op;
    digsim::input_t<bool> clk;

    digsim::output_t<std::bitset<2 * N>> out;
    digsim::output_t<bool> zero_division;

    alu_t(const std::string &_name)
        : module_t(_name)
        , a("a")
        , b("b")
        , op("op")
        , clk("clk")
        , out("out")
        , zero_division("zero_division")
        , add("add")
        , sub("sub")
        , mul("mul")
        , div("div")
        , add_out("add_out")
        , sub_out("sub_out")
        , mul_out("mul_out")
        , div_out("div_out")
        , div_zero_division("div_zero_division")
    {
        // Bind inputs
        add.a(a);
        add.b(b);
        add.out(add_out);

        sub.a(a);
        sub.b(b);
        sub.out(sub_out);

        mul.a(a);
        mul.b(b);
        mul.out(mul_out);

        div.a(a);
        div.b(b);
        div.out(div_out);
        div.zero_division(div_zero_division);

        // ALU selector process
        ADD_SENSITIVITY(alu_t, evaluate, clk);
        ADD_PRODUCES(alu_t, evaluate, out, zero_division);
    }

private:
    // Submodules
    alu_sum_t<N> add;
    alu_sub_t<N> sub;
    alu_mul_t<N> mul;
    alu_div_t<N> div;

    digsim::signal_t<std::bitset<2 * N>> add_out;
    digsim::signal_t<std::bitset<2 * N>> sub_out;
    digsim::signal_t<std::bitset<2 * N>> mul_out;
    digsim::signal_t<std::bitset<2 * N>> div_out;
    digsim::signal_t<bool> div_zero_division;

    void evaluate()
    {
        const uint8_t op_val = op.get();

        switch (op_val) {
        case 0: // ADD
            out.set(add_out.get());
            zero_division.set(false);
            break;
        case 1: // SUB
            out.set(sub_out.get());
            zero_division.set(false);
            break;
        case 2: // MUL
            out.set(mul_out.get());
            zero_division.set(false);
            break;
        case 3: // DIV
            out.set(div_out.get());
            zero_division.set(div_zero_division.get());
            break;
        default:
            digsim::error(get_name(), "Invalid op code: {}", op_val);
            out.set(0);
            zero_division.set(false);
            break;
        }
    }
};
