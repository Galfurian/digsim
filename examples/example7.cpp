/// @file example7.cpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief A simple example of a digital circuit simulation using SimCore.

#include "models/clock.hpp"
#include "models/probe.hpp"

#include <simcore/simcore.hpp>

class ping_module : public simcore::module_t
{
public:
    simcore::input_t<bool> trigger;
    simcore::input_t<bool> clk;

    ping_module(const std::string &_name)
        : simcore::module_t(_name)
        , trigger("trigger")
        , clk("clk")
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
            simcore::info(get_name(), "Already waiting for a trigger, ignoring new one.");
            return;
        }

        trigger_value = trigger.get();
        waiting       = true;
        counter       = 0;

        simcore::info(get_name(), "Trigger {} received, will report after 5 clock cycles...", trigger_value);
    }

    void on_clock()
    {
        if (!waiting) {
            simcore::info(get_name(), "Received clock signal but not waiting for a trigger.");
            return;
        }
        if (clk.get()) {
            if (counter++ == 5) {
                simcore::info(
                    get_name(), "Wake up after wait (value that triggered " + std::to_string(trigger_value) + ")!");
                waiting = false;
            }
        }
    }
};

int main()
{
    simcore::logger.set_level(simcore::log_level_t::debug);

    // === Signals ===
    simcore::signal_t<bool> trigger("trigger");
    simcore::signal_t<bool> clk_out("clk_out");

    // === Modules ===
    Clock clock("clock");
    clock.out(clk_out);

    ping_module pinger("pinger");
    pinger.trigger(trigger);
    pinger.clk(clk_out);

    Probe<bool> clk_probe("clk");
    clk_probe.in(clk_out);

    // === Stimuli ===
    trigger.set(true);

    // === Output Graph ===
    simcore::dependency_graph.export_dot("example7.dot");

    simcore::scheduler.initialize();

    simcore::info("Main", "=== Begin wait_for test ===");

    simcore::scheduler.run(20); // Enough time to see all events

    simcore::info("Main", "=== Simulation finished ===");

    return 0;
}

