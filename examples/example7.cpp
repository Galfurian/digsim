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
    digsim::input_t<bool> trigger;
    digsim::input_t<bool> clk;

    ping_module(const std::string &_name)
        : digsim::module_t(_name)
        , trigger(_name + ".trigger")
        , clk(_name + ".clk")
        , waiting(false)
        , counter(0)
    {
        ADD_SENSITIVITY(ping_module, on_trigger, trigger);
        ADD_SENSITIVITY(ping_module, on_clock, clk);
    }

private:
    bool waiting;
    bool trigger_value;
    int counter;

    void on_trigger()
    {
        if (waiting) {
            digsim::info(get_name(), "Already waiting for a trigger, ignoring new one.");
            return;
        }

        trigger_value = trigger.get();
        waiting       = true;
        counter       = 0;

        digsim::info(get_name(), "Trigger {} received, will report after 5 clock cycles...", trigger_value);
    }

    void on_clock()
    {
        if (!waiting) {
            digsim::info(get_name(), "Received clock signal but not waiting for a trigger.");
            return;
        }
        if (clk.get()) {
            if (counter++ == 5) {
                digsim::info(
                    get_name(), "Wake up after wait (value that triggered " + std::to_string(trigger_value) + ")!");
                waiting = false;
            }
        }
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    // === Signals ===
    digsim::signal_t<bool> trigger("trigger");
    digsim::signal_t<bool> clk_out("clk_out");

    // === Modules ===
    digsim::clock_t clock("clock");
    clock.out(clk_out);

    ping_module pinger("pinger");
    pinger.trigger(trigger);
    pinger.clk(clk_out);

    digsim::probe_t<bool> clk_probe("clk");
    clk_probe.in(clk_out);

    // === Stimuli ===
    trigger.set(true);

    // === Output Graph ===
    digsim::dependency_graph.export_dot("example7.dot");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Begin wait_for test ===");

    digsim::scheduler.run(20); // Enough time to see all events

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}

