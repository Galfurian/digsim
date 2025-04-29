/// @file xor_gate.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief This file contains the implementation of a simple XOR gate.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

class XorGate : public digsim::module_t
{
public:
    digsim::input_t<bool> a;
    digsim::input_t<bool> b;
    digsim::output_t<bool> out;

    XorGate(const std::string &_name, digsim::module_t *_parent = nullptr)
        : digsim::module_t(_name, _parent)
        , a("a", this)
        , b("b", this)
        , out("out", this)
    {
        ADD_SENSITIVITY(XorGate, evaluate, a, b);
        ADD_PRODUCER(XorGate, evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = a.get() ^ b.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", ";
        ss << "b:" << b.get() << " -> ";
        ss << "out:" << result;
        if (out.get_delay() > 0) {
            ss << " (+" << out.get_delay() << "t)";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};
