/// @file digital_circuit.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief
/// @version 0.1
/// @date 2024-05-13
///
/// @copyright Copyright (c) 2024
///

#include "digsim/digsim.hpp"

#include "models.hpp"

class ping_module : public digsim::module_t
{
public:
    digsim::signal_t<bool> &trigger;
    digsim::signal_t<bool> &clk;

    ping_module(const std::string &name, digsim::signal_t<bool> &trigger_in, digsim::signal_t<bool> &clk_in)
        : digsim::module_t(name)
        , trigger(trigger_in)
        , clk(clk_in)
        , waiting(false)
        , counter(0)
    {
        add_sensitivity(&ping_module::on_trigger, trigger);
        add_sensitivity(&ping_module::on_clock, clk);
    }

private:
    bool waiting;
    bool trigger_value;
    int counter;

    void on_trigger()
    {
        if (waiting)
            return;

        trigger_value = trigger.get();
        waiting       = true;
        counter       = 0;

        digsim::info(get_name(), "Trigger received, will report after 5 clock cycles...");
    }

    void on_clock()
    {
        if (clk.get() && waiting)
            if (counter++ == 5) {
                digsim::info(
                    get_name(), "Wake up after wait (value that triggered " + std::to_string(trigger_value) + ")!");
                waiting = false;
            }
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    digsim::signal_t<bool> ping("ping");

    digsim::clock_t clock("clock", 2, 0.5, 0, false);

    ping_module pinger("pinger", ping, clock.out);
    digsim::probe_t clk_probe("clk", clock.out);

    clk_probe.callback = [](const digsim::signal_t<bool> &signal) {
        digsim::info("Main", "clk = " + std::to_string(signal.get()));
    };

    ping.set(true);

    // Initialize simulation state
    digsim::scheduler.initialize();

    digsim::info("Main", "=== Begin wait_for test ===");

    digsim::info("Main", "Test printing int {}", 42);

    digsim::scheduler.run(20); // Run for enough time to see 5 clock cycles

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

