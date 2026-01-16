/// @file thermostat_example.cpp
/// @brief A simple thermostat simulation example using SimCore.
/// @details This example models an environment with temperature, a thermostat that controls a heater,
/// and the heater that affects the environment temperature.

#include <iomanip>
#include <sstream>

#include "models/timer.hpp"

/// @brief Environment module that simulates temperature changes over time.
class Environment : public simcore::module_t
{
public:
    simcore::input_t<bool> trigger;        ///< Timer trigger input to drive temperature changes.
    simcore::input_t<double> heater_heat;  ///< Heat input from heater.
    simcore::input_t<double> cooler_heat;  ///< Cooling input from cooler.
    simcore::input_t<double> outside_temp; ///< Outside temperature affecting heat transfer with environment.
    simcore::output_t<double> temperature; ///< Current temperature output.

    Environment(const std::string &_name)
        : simcore::module_t(_name)
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
        simcore::info(get_name(), ss.str());
    }
};

/// @brief Thermostat module that controls the heater based on temperature and setpoint.
class Thermostat : public simcore::module_t
{
public:
    simcore::input_t<double> temperature;  ///< Current temperature input.
    simcore::input_t<double> setpoint;     ///< Desired temperature setpoint.
    simcore::output_t<bool> heater_on;     ///< Control signal for heater.
    simcore::output_t<bool> cooler_on;     ///< Control signal for cooler.
    simcore::output_t<double> energy_used; ///< Energy consumption tracking.

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
        : simcore::module_t(_name)
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

    void set_mode(Mode new_mode)
    {
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
        simcore::info(get_name(), ss.str());
    }
};

/// @brief Heater module that increases temperature when activated.
class Heater : public simcore::module_t
{
public:
    simcore::input_t<bool> control;        ///< Control signal from thermostat.
    simcore::output_t<double> heat_output; ///< Heat contribution to environment.

    Heater(const std::string &_name)
        : simcore::module_t(_name)
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
        simcore::info(get_name(), ss.str());
    }
};

/// @brief Cooler module that decreases temperature when activated.
class Cooler : public simcore::module_t
{
public:
    simcore::input_t<bool> control;        ///< Control signal from thermostat.
    simcore::output_t<double> cool_output; ///< Cooling contribution to environment.

    Cooler(const std::string &_name)
        : simcore::module_t(_name)
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
        simcore::info(get_name(), ss.str());
    }
};

int main()
{
    simcore::set_log_level(simcore::log_level_t::debug);

    // Create signals
    simcore::signal_t<bool> timer_trigger("timer_trigger", false);
    simcore::signal_t<double> temperature("temperature", 25.0, 1); // Start warmer to test cooling
    simcore::signal_t<double> setpoint_signal("setpoint", 21.0);
    simcore::signal_t<bool> heater_control("heater_control", false);
    simcore::signal_t<double> heater_output("heater_output", 0.0);
    simcore::signal_t<bool> cooler_control("cooler_control", false);
    simcore::signal_t<double> cooler_output("cooler_output", 0.0);
    simcore::signal_t<double> outside_temp_signal("outside_temp", 15.0);
    simcore::signal_t<double> energy_used_signal("energy_used", 0.0);

    Timer timer("timer", simcore::nanoseconds(1));
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
    simcore::dependency_graph.export_dot("thermostat_example.dot");

    simcore::info("Main", "");
    simcore::info("Main", "=== Initializing thermostat simulation ===");
    simcore::info("Main", "");

    simcore::scheduler.initialize();

    simcore::info("Main", "");
    simcore::info("Main", "=== Running simulation for 120 seconds ===");
    simcore::info("Main", "");

    // Run simulation for 120 seconds with mode changes
    for (int time = 0; time < 120; ++time) {
        // Change modes at different times to demonstrate functionality
        if (time == 20) {
            thermo.set_mode(Thermostat::HEAT);
            simcore::info("Main", "");
            simcore::info("Main", "=== Switching to HEAT mode ===");
            simcore::info("Main", "");
        } else if (time == 50) {
            thermo.set_mode(Thermostat::AUTO);
            simcore::info("Main", "");
            simcore::info("Main", "=== Switching to AUTO mode ===");
            simcore::info("Main", "");
        } else if (time == 80) {
            thermo.set_mode(Thermostat::OFF);
            outside_temp_signal.set(24);
            simcore::info("Main", "");
            simcore::info("Main", "=== Switching to OFF mode ===");
            simcore::info("Main", "");
            simcore::info("Main", "=== Setting outside temperature to 24°C ===");
            simcore::info("Main", "");

        } else if (time == 100) {
            thermo.set_mode(Thermostat::COOL);
            simcore::info("Main", "");
            simcore::info("Main", "=== Switching back to COOL mode ===");
            simcore::info("Main", "");
        }

        simcore::scheduler.run(1); // Run one time unit at a time
    }

    simcore::info("Main", "");
    simcore::info("Main", "=== Simulation finished ===");
    simcore::info("Main", "");
    simcore::info("Main", "Total energy consumed: " + std::to_string(energy_used_signal.get()) + " units");

    return 0;
}