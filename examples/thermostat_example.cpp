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
    digsim::input_t<double> cooler_heat;  ///< Cooling input from cooler.
    digsim::input_t<double> outside_temp; ///< Outside temperature affecting heat transfer with environment.
    digsim::output_t<double> temperature; ///< Current temperature output.

    Environment(const std::string &_name)
        : digsim::module_t(_name)
        , trigger("trigger", this)
        , heater_heat("heater_heat", this)
        , cooler_heat("cooler_heat", this)
        , outside_temp("outside_temp", this)
        , temperature("temperature", this)
    {
        ADD_SENSITIVITY(Environment, evaluate, trigger, heater_heat, cooler_heat, outside_temp);
        ADD_PRODUCER(Environment, evaluate, temperature);
    }

private:
    void evaluate()
    {
        double current_temp = temperature.get();
        double heat         = heater_heat.get();
        double cooling      = cooler_heat.get();
        double outdoor_temp = outside_temp.get();

        // More realistic heat transfer: exponential approach to equilibrium
        // Heat transfer coefficient (higher = faster heat exchange with outside)
        double heat_transfer_coeff         = 0.15;
        // Heat transfer with outside environment (positive = heat loss, negative = heat gain)
        double environmental_heat_transfer = heat_transfer_coeff * (current_temp - outdoor_temp);
        // Heater and cooler inputs
        double heating_effect              = heat;
        double cooling_effect              = cooling;
        // Small random variation (±0.1°C)
        double noise                       = ((rand() % 21) - 10) * 0.01;

        double new_temp = current_temp - environmental_heat_transfer + heating_effect + cooling_effect + noise;
        temperature.set(new_temp);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Temperature: " << current_temp << "°C -> " << new_temp << "°C ";
        ss << "(env_transfer: " << std::fixed << std::setprecision(2) << environmental_heat_transfer << "°C, ";
        ss << "heating: +" << heating_effect << "°C, ";
        ss << "AC: " << cooling_effect << "°C, ";
        ss << "outdoor: " << outdoor_temp << "°C)";
        digsim::info(get_name(), ss.str());
    }
};

/// @brief Thermostat module that controls the heater based on temperature and setpoint.
class Thermostat : public digsim::module_t
{
public:
    digsim::input_t<double> temperature;  ///< Current temperature input.
    digsim::input_t<double> setpoint;     ///< Desired temperature setpoint.
    digsim::output_t<bool> heater_on;     ///< Control signal for heater.
    digsim::output_t<bool> cooler_on;     ///< Control signal for cooler.
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
        , cooler_on("cooler_on", this)
        , energy_used("energy_used", this)
        , heating_state(false)
        , cooling_state(false)
        , mode(initial_mode)
        , total_energy(0.0)
    {
        ADD_SENSITIVITY(Thermostat, evaluate, temperature, setpoint);
        ADD_PRODUCER(Thermostat, evaluate, heater_on, cooler_on, energy_used);
    }

    void set_mode(Mode new_mode) { 
        mode = new_mode; 
        // Manually trigger evaluation when mode changes
        evaluate();
    }

private:
    bool heating_state;  ///< Internal state for heating hysteresis
    bool cooling_state;  ///< Internal state for cooling hysteresis
    Mode mode;           ///< Current operating mode
    double total_energy; ///< Total energy consumed

    void evaluate()
    {
        double current_temp = temperature.get();
        double target_temp  = setpoint.get();

        bool heat_control = false;
        bool cool_control = false;

        switch (mode) {
        case HEAT:
            // Heating mode with hysteresis - only heat, never cool
            if (!heating_state && current_temp < target_temp - 0.5) {
                heating_state = true;
            } else if (heating_state && current_temp > target_temp + 0.5) {
                heating_state = false;
            }
            heat_control = heating_state;
            cool_control = false;
            break;

        case COOL:
            // Cooling mode with hysteresis - only cool, never heat
            if (!cooling_state && current_temp > target_temp + 0.5) {
                cooling_state = true;
            } else if (cooling_state && current_temp < target_temp - 0.5) {
                cooling_state = false;
            }
            heat_control = false;
            cool_control = cooling_state;
            break;

        case AUTO:
            // Auto mode: intelligently choose heating or cooling
            if (current_temp < target_temp - 1.0) {
                // Too cold - turn on heating
                heating_state = true;
                cooling_state = false;
                heat_control  = true;
                cool_control  = false;
            } else if (current_temp > target_temp + 1.0) {
                // Too hot - turn on cooling
                heating_state = false;
                cooling_state = true;
                heat_control  = false;
                cool_control  = true;
            } else {
                // Within range - turn everything off
                heating_state = false;
                cooling_state = false;
                heat_control  = false;
                cool_control  = false;
            }
            break;

        case OFF:
            // Everything off
            heating_state = false;
            cooling_state = false;
            heat_control  = false;
            cool_control  = false;
            break;
        }

        heater_on.set(heat_control);
        cooler_on.set(cool_control);

        // Track energy usage (1.0 energy units per time step when active)
        if (heat_control || cool_control) {
            total_energy += 1.0;
        }
        energy_used.set(total_energy);

        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Mode: " << mode_to_string(mode) << ", ";
        ss << "Temperature: " << current_temp << "°C, ";
        ss << "Setpoint: " << target_temp << "°C, ";
        ss << "Heater: " << (heat_control ? "ON" : "OFF") << ", ";
        ss << "Cooler: " << (cool_control ? "ON" : "OFF") << ", ";
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
        double heat = is_on ? 1.5 : 0.0;
        heat_output.set(heat);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Heater is " << (is_on ? "ON" : "OFF") << " (output: " << heat << "°C)";
        digsim::info(get_name(), ss.str());
    }
};

/// @brief Cooler module that decreases temperature when activated.
class Cooler : public digsim::module_t
{
public:
    digsim::input_t<bool> control;        ///< Control signal from thermostat.
    digsim::output_t<double> cool_output; ///< Cooling contribution to environment.

    Cooler(const std::string &_name)
        : digsim::module_t(_name)
        , control("control", this)
        , cool_output("cool_output", this)
    {
        ADD_SENSITIVITY(Cooler, evaluate, control);
        ADD_PRODUCER(Cooler, evaluate, cool_output);
    }

private:
    void evaluate()
    {
        bool is_on     = control.get();
        double cooling = is_on ? -1.5 : 0.0;
        cool_output.set(cooling);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Cooler is " << (is_on ? "ON" : "OFF") << " (output: " << cooling << "°C)";
        digsim::info(get_name(), ss.str());
    }
};

int main()
{
    digsim::logger.set_level(digsim::log_level_t::info);

    // Create signals
    digsim::signal_t<bool> timer_trigger("timer_trigger", false);
    digsim::signal_t<double> temperature("temperature", 25.0, 1); // Start warmer to test cooling
    digsim::signal_t<double> setpoint_signal("setpoint", 21.0);
    digsim::signal_t<bool> heater_control("heater_control", false);
    digsim::signal_t<double> heater_output("heater_output", 0.0);
    digsim::signal_t<bool> cooler_control("cooler_control", false);
    digsim::signal_t<double> cooler_output("cooler_output", 0.0);
    digsim::signal_t<double> outside_temp_signal("outside_temp", 15.0);
    digsim::signal_t<double> energy_used_signal("energy_used", 0.0);

    Timer timer("timer", 1);
    timer.trigger(timer_trigger);

    // Create modules
    Environment env("environment");
    env.trigger(timer_trigger);
    env.heater_heat(heater_output);
    env.cooler_heat(cooler_output);
    env.outside_temp(outside_temp_signal);
    env.temperature(temperature);

    Thermostat thermo("thermostat", Thermostat::COOL); // Start in COOL mode to test cooling
    thermo.temperature(temperature);
    thermo.setpoint(setpoint_signal);
    thermo.heater_on(heater_control);
    thermo.cooler_on(cooler_control);
    thermo.energy_used(energy_used_signal);

    Heater heater("heater");
    heater.control(heater_control);
    heater.heat_output(heater_output);

    Cooler cooler("cooler");
    cooler.control(cooler_control);
    cooler.cool_output(cooler_output);

    // Export dependency graph for visualization
    digsim::dependency_graph.export_dot("thermostat_example.dot");

    digsim::info("Main", "");
    digsim::info("Main", "=== Initializing thermostat simulation ===");
    digsim::info("Main", "");

    digsim::scheduler.initialize();

    digsim::info("Main", "");
    digsim::info("Main", "=== Running simulation for 120 seconds ===");
    digsim::info("Main", "");

    // Run simulation for 120 seconds with mode changes
    for (int time = 0; time < 120; ++time) {
        // Change modes at different times to demonstrate functionality
        if (time == 20) {
            thermo.set_mode(Thermostat::HEAT);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching to HEAT mode ===");
            digsim::info("Main", "");
        } else if (time == 50) {
            thermo.set_mode(Thermostat::AUTO);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching to AUTO mode ===");
            digsim::info("Main", "");
        } else if (time == 80) {
            thermo.set_mode(Thermostat::OFF);
            outside_temp_signal.set(24);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching to OFF mode ===");
            digsim::info("Main", "");
            digsim::info("Main", "=== Setting outside temperature to 24°C ===");
            digsim::info("Main", "");

        } else if (time == 100) {
            thermo.set_mode(Thermostat::COOL);
            digsim::info("Main", "");
            digsim::info("Main", "=== Switching back to COOL mode ===");
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