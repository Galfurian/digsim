/// @file timer.hpp
/// @brief Timer module that generates periodic trigger signals.
/// @details A simple timer that sets its output to true at regular intervals,
/// providing a cleaner alternative to the full clock module for periodic events.

#pragma once

#include <simcore/simcore.hpp>

/// @brief Timer module that generates periodic trigger signals.
class Timer : public simcore::module_t
{
public:
    simcore::output_t<bool> trigger; ///< Periodic trigger output.

    Timer(const std::string &_name, simcore::discrete_time_t _period)
        : simcore::module_t(_name)
        , trigger("trigger", this)
        , period(_period)
    {
        // Create a process info for the timer evaluation method.
        auto proc_info = simcore::get_or_create_process(this, &Timer::evaluate, "start");
        // Schedule the first evaluation of the timer signal.
        simcore::scheduler.schedule_after(proc_info, period);
        // Register the output signal in the dependency graph.
        ADD_PRODUCER(Timer, evaluate, trigger);
    }

// private:
    simcore::discrete_time_t period; ///< Time interval between triggers.

    inline void evaluate()
    {
        // Set the trigger to true to signal the environment to evaluate
        trigger.set(trigger.get() ? false : true);
        // Create the process info for the next evaluation.
        auto proc_info = simcore::get_or_create_process(this, &Timer::evaluate, "evaluate");
        // Schedule the next evaluation of the timer signal.
        simcore::scheduler.schedule_after(proc_info, period);
    }
};