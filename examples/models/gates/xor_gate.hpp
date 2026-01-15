/// @file xor_gate.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief This file contains the implementation of a simple XOR gate.

#pragma once

#include <simcore/simcore.hpp>

#include <iomanip>
#include <sstream>

class XorGate : public simcore::module_t
{
public:
    simcore::input_t<bool> a;
    simcore::input_t<bool> b;
    simcore::output_t<bool> out;

    XorGate(const std::string &_name, simcore::module_t *_parent = nullptr)
        : simcore::module_t(_name, _parent)
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
        simcore::info(get_name(), ss.str());

        out.set(result);
    }
};
