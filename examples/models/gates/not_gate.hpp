/// @file not_gate.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A NOT gate.

#pragma once

#include <simcore/simcore.hpp>

#include <iomanip>
#include <sstream>

class NotGate : public simcore::module_t
{
public:
    simcore::input_t<bool> in;
    simcore::output_t<bool> out;

    NotGate(const std::string &_name, simcore::module_t *_parent = nullptr)
        : simcore::module_t(_name, _parent)
        , in("in", this)
        , out("out", this)
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
        simcore::info(get_name(), ss.str());

        out.set(result);
    }
};
