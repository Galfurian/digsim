/// @file or_gate.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief This file contains the implementation of a simple OR gate.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

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
        ADD_PRODUCER(OrGate, evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = a.get() || b.get();

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
