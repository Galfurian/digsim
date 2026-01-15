/// @file mux2to1.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A 2-to-1 multiplexer.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

template <typename T> class Mux2to1 : public digsim::module_t
{
public:
    digsim::input_t<T> a;
    digsim::input_t<T> b;
    digsim::input_t<bool> sel;
    digsim::output_t<T> out;

    Mux2to1(const std::string &_name, digsim::module_t *_parent = nullptr)
        : digsim::module_t(_name, _parent)
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
        digsim::info(get_name(), ss.str());

        out.set(result);
    }
};