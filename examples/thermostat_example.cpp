/// @file thermostat_example.cpp
/// @brief A simple thermostat simulation example using DigSim.
/// @details This example models an environment with temperature, a thermostat that controls a heater,
/// and the heater that affects the environment temperature.

#include <digsim/digsim.hpp>
#include <iomanip>
#include <sstream>

/// @brief Timer module that generates periodic trigger signals.
/// @details A simple timer that sets its output to true at regular intervals,
/// providing a cleaner alternative to the full clock module for periodic events.
class Timer : public digsim::module_t
{
public:
    digsim::output_t<bool> trigger; ///< Periodic trigger output.

    Timer(const std::string &_name, digsim::discrete_time_t period)
        : digsim::module_t(_name)
        , trigger("trigger", this)
        , period(period)
    {
        // Register the output signal in the dependency graph.
        ADD_PRODUCER(Timer, evaluate, trigger);
        // Create a process info for the timer evaluation method.
        auto proc_info = digsim::get_or_create_process(this, &Timer::evaluate, "start");
        // Schedule the first evaluation of the timer signal.
        digsim::scheduler.schedule_after(proc_info, period);
    }

private:
    digsim::discrete_time_t period; ///< Time interval between triggers.

    void evaluate()
    {
        // Set the trigger to true to signal the environment to evaluate
        trigger.set(trigger.get() ? false : true);

        // Create the process info for the next evaluation.
        auto proc_info = digsim::get_or_create_process(this, &Timer::evaluate, "evaluate");
        // Schedule the next evaluation of the timer signal.
        digsim::scheduler.schedule_after(proc_info, period);
    }
};

/// @brief Environment module that simulates temperature changes over time.
class Environment : public digsim::module_t
{
public:
    digsim::input_t<bool> trigger;        ///< Timer trigger input to drive temperature changes.
    digsim::input_t<double> heater_heat;  ///< Heat input from heater.
    digsim::input_t<double> outside_temp; ///< Outside temperature affecting heat loss.
    digsim::output_t<double> temperature; ///< Current temperature output.

    Environment(const std::string &_name)
        : digsim::module_t(_name)
        , trigger("trigger", this)
        , heater_heat("heater_heat", this)
        , outside_temp("outside_temp", this)
        , temperature("temperature", this)
    {
        ADD_SENSITIVITY(Environment, evaluate, trigger, heater_heat, outside_temp);
        ADD_PRODUCER(Environment, evaluate, temperature);
    }

private:
    void evaluate()
    {
        if (trigger.get()) {
            double current_temp = temperature.get();
            double heat         = heater_heat.get();
            double outdoor_temp = outside_temp.get();

            // More realistic heat transfer: exponential approach to equilibrium
            // Heat loss coefficient (higher = faster cooling)
            double heat_loss_coeff = 0.15;
            // Natural cooling toward outside temperature
            double natural_cooling = heat_loss_coeff * (current_temp - outdoor_temp);
            // Heater input
            double heating_effect  = heat;
            // Small random variation (±0.1°C)
            double noise           = ((rand() % 21) - 10) * 0.01;

            double new_temp = current_temp - natural_cooling + heating_effect + noise;
            temperature.set(new_temp);

            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Temperature: " << current_temp << "°C -> " << new_temp << "°C ";
            ss << "(cooling: -" << std::fixed << std::setprecision(2) << natural_cooling << "°C, ";
            ss << "heating: +" << heating_effect << "°C, ";
            ss << "outdoor: " << outdoor_temp << "°C)";
            digsim::info(get_name(), ss.str());
        }
    }
};

/// @brief Thermostat module that controls the heater based on temperature and setpoint.
class Thermostat : public digsim::module_t
{
public:
    digsim::input_t<double> temperature;  ///< Current temperature input.
    digsim::input_t<double> setpoint;     ///< Desired temperature setpoint.
    digsim::output_t<bool> heater_on;     ///< Control signal for heater.
    digsim::output_t<double> energy_used; ///< Energy consumption tracking.

    enum Mode {
        HEAT,
        COOL,
        AUTO,
        OFF,
    };

    inline const char *mode_to_string(Mode m) const
    {
        switch (m) {
        case HEAT:
            return "HEAT";
        case COOL:
            return "COOL";
        case AUTO:
            return "AUTO";
        case OFF:
            return "OFF";
        default:
            return "UNKNOWN";
        }
    }

    Thermostat(const std::string &_name, Mode initial_mode = HEAT)
        : digsim::module_t(_name)
        , temperature("temperature", this)
        , setpoint("setpoint", this)
        , heater_on("heater_on", this)
        , energy_used("energy_used", this)
        , heating_state(false)
        , mode(initial_mode)
        , total_energy(0.0)
    {
        ADD_SENSITIVITY(Thermostat, evaluate, temperature, setpoint);
        ADD_PRODUCER(Thermostat, evaluate, heater_on, energy_used);
    }

    void set_mode(Mode new_mode) { mode = new_mode; }

private:
    bool heating_state;  ///< Internal state for hysteresis
    Mode mode;           ///< Current operating mode
    double total_energy; ///< Total energy consumed

    void evaluate()
    {
        double current_temp = temperature.get();
        double target_temp  = setpoint.get();

        bool should_control = false;

        switch (mode) {
        case HEAT:
            // Heating mode with hysteresis
            if (!heating_state && current_temp < target_temp - 0.5) {
                heating_state = true;
            } else if (heating_state && current_temp > target_temp + 0.5) {
                heating_state = false;
            }
            should_control = heating_state;
            break;

        case COOL:
            // Cooling mode (would control AC)
            if (!heating_state && current_temp > target_temp + 0.5) {
                heating_state = true; // Actually cooling
            } else if (heating_state && current_temp < target_temp - 0.5) {
                heating_state = false;
            }
            should_control = heating_state;
            break;

        case AUTO:
            // Auto mode: heat or cool as needed
            if (current_temp < target_temp - 1.0) {
                heating_state = true; // Heat
            } else if (current_temp > target_temp + 1.0) {
                heating_state = false; // Cool (not implemented)
            }
            should_control = heating_state;
            break;

        case OFF:
            heating_state  = false;
            should_control = false;
            break;
        }

        heater_on.set(should_control);

        // Track energy usage (1.0 energy units per time step when heating)
        if (should_control) {
            total_energy += 1.0;
        }
        energy_used.set(total_energy);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Mode: " << mode_to_string(mode) << ", ";
        ss << "Temperature: " << current_temp << "°C, ";
        ss << "Setpoint: " << target_temp << "°C, ";
        ss << "Heater: " << (should_control ? "ON" : "OFF") << ", ";
        ss << "Energy: " << total_energy << " units";
        digsim::info(get_name(), ss.str());
    }
};

/// @brief Heater module that increases temperature when activated.
class Heater : public digsim::module_t
{
public:
    digsim::input_t<bool> control;        ///< Control signal from thermostat.
    digsim::output_t<double> heat_output; ///< Heat contribution to environment.

    Heater(const std::string &_name)
        : digsim::module_t(_name)
        , control("control", this)
        , heat_output("heat_output", this)
    {
        ADD_SENSITIVITY(Heater, evaluate, control);
        ADD_PRODUCER(Heater, evaluate, heat_output);
    }

private:
    void evaluate()
    {
        bool is_on  = control.get();
        double heat = is_on ? 2.0 : 0.0; // Heater provides 2.0 degrees of heat when on
        heat_output.set(heat);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Heater is " << (is_on ? "ON" : "OFF") << " (output: " << heat << "°C)";
        digsim::info(get_name(), ss.str());
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    // Create signals
    digsim::signal_t<bool> timer_trigger("timer_trigger", false);
    digsim::signal_t<double> temperature("temperature", 20.0, 1);
    digsim::signal_t<double> setpoint_signal("setpoint", 21.0);
    digsim::signal_t<bool> heater_control("heater_control", false);
    digsim::signal_t<double> heater_output("heater_output", 0.0);
    digsim::signal_t<double> outside_temp_signal("outside_temp", 15.0);
    digsim::signal_t<double> energy_used_signal("energy_used", 0.0);

    Timer timer("timer", 1);
    timer.trigger(timer_trigger);

    // Create modules
    Environment env("environment");
    env.trigger(timer_trigger);
    env.heater_heat(heater_output);
    env.outside_temp(outside_temp_signal);
    env.temperature(temperature);

    Thermostat thermo("thermostat", Thermostat::OFF);
    thermo.temperature(temperature);
    thermo.setpoint(setpoint_signal);
    thermo.heater_on(heater_control);
    thermo.energy_used(energy_used_signal);

    Heater heater("heater");
    heater.control(heater_control);
    heater.heat_output(heater_output);

    // Export dependency graph for visualization
    digsim::dependency_graph.export_dot("thermostat_example.dot");

    digsim::info("Main", "");
    digsim::info("Main", "=== Initializing thermostat simulation ===");
    digsim::info("Main", "");

    digsim::scheduler.initialize();

    digsim::info("Main", "");
    digsim::info("Main", "=== Running simulation for 50 time units ===");
    digsim::info("Main", "");

    // Run simulation for 50 time units with mode changes
    for (int time = 0; time < 50; ++time) {
        // Change modes at different times to demonstrate functionality
        if (time == 10) {
            thermo.set_mode(Thermostat::AUTO);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching to AUTO mode ===");
            digsim::info("Main", "");
        } else if (time == 25) {
            thermo.set_mode(Thermostat::OFF);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching to OFF mode ===");
            digsim::info("Main", "");
        } else if (time == 35) {
            thermo.set_mode(Thermostat::HEAT);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching back to HEAT mode ===");
            digsim::info("Main", "");
        }

        digsim::scheduler.run(1); // Run one time unit at a time
    }

    digsim::info("Main", "");
    digsim::info("Main", "=== Simulation finished ===");
    digsim::info("Main", "");
    digsim::info("Main", "Total energy consumed: " + std::to_string(energy_used_signal.get()) + " units");

    return 0;
}