/// @file multiplexer.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A 2-to-1 multiplexer.

#pragma once

#include <digsim/digsim.hpp>

#include <iomanip>
#include <sstream>

class multiplexer_t : public digsim::module_t
{
public:
    digsim::input_t<bs_data_t> a;
    digsim::input_t<bs_data_t> b;
    digsim::input_t<bool> sel;
    digsim::output_t<bs_data_t> out;

    multiplexer_t(const std::string &_name)
        : digsim::module_t(_name)
        , a("a", this)
        , b("b", this)
        , sel("sel", this)
        , out("out", this)
    {
        ADD_SENSITIVITY(multiplexer_t, evaluate, a, b, sel);
        ADD_PRODUCER(multiplexer_t, evaluate, out);
    }

private:
    void evaluate()
    {
        auto result = sel.get() ? b.get() : a.get();
        out.set(result);

        digsim::debug(
            get_name(), "a: 0x{:04X}, b: 0x{:04X}, sel: {}, out: 0x{:04X}", a.get().to_ulong(), b.get().to_ulong(),
            sel.get(), result.to_ulong());
    }
};
