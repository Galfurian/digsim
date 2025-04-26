/// @file nand_gate.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple NAND gate implementation using digsim.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

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
        ADD_PRODUCER(NandGate, evaluate, out);
    }

private:
    void evaluate()
    {
        bool result = !(a.get() && b.get());

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
