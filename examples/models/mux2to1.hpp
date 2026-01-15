/// @file mux2to1.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A 2-to-1 multiplexer.

#pragma once

#include <simcore/simcore.hpp>

#include <iomanip>
#include <sstream>

template <typename T> class Mux2to1 : public simcore::module_t
{
public:
    simcore::input_t<T> a;
    simcore::input_t<T> b;
    simcore::input_t<bool> sel;
    simcore::output_t<T> out;

    Mux2to1(const std::string &_name, simcore::module_t *_parent = nullptr)
        : simcore::module_t(_name, _parent)
        , a("a", this)
        , b("b", this)
        , sel("sel", this)
        , out("out", this)
    {
        ADD_SENSITIVITY(Mux2to1, evaluate, a, b, sel);
        ADD_PRODUCER(Mux2to1, evaluate, out);
    }

private:
    void evaluate()
    {
        T result = sel.get() ? b.get() : a.get();

        std::stringstream ss;
        ss << "a:" << a.get() << ", ";
        ss << "b:" << b.get() << ", ";
        ss << "sel:" << sel.get() << " -> ";
        ss << "out:" << result;
        if (out.get_delay() > 0) {
            ss << " (+" << out.get_delay() << "t)";
        }
        simcore::info(get_name(), ss.str());

        out.set(result);
    }
};