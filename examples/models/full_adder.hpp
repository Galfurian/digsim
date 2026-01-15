/// @file full_adder.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Full adder model.

#pragma once

#include <simcore/simcore.hpp>

#include <iomanip>
#include <sstream>

class FullAdder : public simcore::module_t
{
public:
    simcore::input_t<bool> a;
    simcore::input_t<bool> b;
    simcore::input_t<bool> cin;
    simcore::output_t<bool> sum;
    simcore::output_t<bool> cout;

    FullAdder(const std::string &_name, simcore::module_t *_parent = nullptr)
        : simcore::module_t(_name, _parent)
        , a("a", this)
        , b("b", this)
        , cin("cin", this)
        , sum("sum", this)
        , cout("cout", this)
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
        simcore::info(get_name(), ss.str());

        sum.set(s);
        cout.set(c);
    }
};
