/// @file full_adder.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Full adder model.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

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
        ADD_PRODUCER(FullAdder, evaluate, sum, cout);
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
        ss << "a:" << aval << ", ";
        ss << "b:" << bval << ", ";
        ss << "cin:" << cinval << " -> ";
        ss << "sum:" << s;
        if (sum.get_delay() > 0) {
            ss << " (+" << sum.get_delay() << "t)";
        }
        ss << ", cout:" << c;
        if (cout.get_delay() > 0) {
            ss << " (+" << cout.get_delay() << "t)";
        }
        digsim::info(get_name(), ss.str());

        sum.set(s);
        cout.set(c);
    }
};
