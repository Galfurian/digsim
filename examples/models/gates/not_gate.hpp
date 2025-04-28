/// @file not_gate.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A NOT gate.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

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
        ADD_PRODUCER(NotGate, evaluate, out);
    }

    void evaluate()
    {
        bool result = !in.get();

        std::stringstream ss;
        ss << "in:" << in.get() << ", ";
        ss << "out:" << result;
        if (out.get_delay() > 0) {
            ss << " (+" << out.get_delay() << "t)";
        }
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};
