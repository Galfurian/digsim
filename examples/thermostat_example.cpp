/// @file thermostat_example.cpp
/// @brief A simple thermostat simulation example using DigSim.
/// @details This example models an environment with temperature, a thermostat that controls a heater,
/// and the heater that affects the environment temperature.

#include <digsim/digsim.hpp>

/// @brief Environment module that simulates temperature changes over time.
class Environment : public digsim::module_t
{
public:
    digsim::input_t<bool> clk;            ///< Clock input to drive temperature changes.
    digsim::input_t<double> heater_heat;  ///< Heat input from heater.
    digsim::output_t<double> temperature; ///< Current temperature output.

    Environment(const std::string &_name)
        : digsim::module_t(_name)
        , clk("clk", this)
        , heater_heat("heater_heat", this)
        , temperature("temperature", this)
    {
        ADD_SENSITIVITY(Environment, evaluate, clk, heater_heat);
        ADD_PRODUCER(Environment, evaluate, temperature);
    }

private:
    void evaluate()
    {
        if (clk.get()) {
            // On clock edge, temperature naturally decreases (cooling effect) and increases with heat.
            double current_temp = temperature.get();
            double heat         = heater_heat.get();
            // Cool down by 0.5, heat up by heater amount.
            double new_temp     = current_temp - 0.5 + heat;
            temperature.set(new_temp);
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2);
            ss << "Temperature updated: " << current_temp << "°C -> " << new_temp << "°C (heat: " << heat << "°C)";
            digsim::info(get_name(), ss.str());
        }
    }
};

/// @brief Thermostat module that controls the heater based on temperature and setpoint.
class Thermostat : public digsim::module_t
{
public:
    digsim::input_t<double> temperature; ///< Current temperature input.
    digsim::input_t<double> setpoint;    ///< Desired temperature setpoint.
    digsim::output_t<bool> heater_on;    ///< Control signal for heater.

    Thermostat(const std::string &_name)
        : digsim::module_t(_name)
        , temperature("temperature", this)
        , setpoint("setpoint", this)
        , heater_on("heater_on", this)
        , heating_state(false)
    {
        ADD_SENSITIVITY(Thermostat, evaluate, temperature, setpoint);
        ADD_PRODUCER(Thermostat, evaluate, heater_on);
    }

private:
    bool heating_state; ///< Internal state for hysteresis

    void evaluate()
    {
        double current_temp = temperature.get();
        double target_temp  = setpoint.get();
        if (!heating_state && current_temp < target_temp - 0.5) {
            heating_state = true;
        } else if (heating_state && current_temp > target_temp + 0.5) {
            heating_state = false;
        }
        heater_on.set(heating_state);
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << "Temperature: " << current_temp << "°C, Setpoint: ";
        ss << target_temp << "°C, Heater: " << (heating_state ? "ON" : "OFF");
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
    digsim::signal_t<bool> clk_signal("clock_signal", false);
    digsim::signal_t<double> temperature("temperature", 20.0, 1); // delayed
    digsim::signal_t<double> setpoint_signal("setpoint", 21.0);
    digsim::signal_t<bool> heater_control("heater_control", false);
    digsim::signal_t<double> heater_output("heater_output", 0.0);

    // Create clock
    digsim::clock_t clk("clock", 2.0, 0.5, 0, true);
    clk.out(clk_signal);

    // Create modules
    Environment env("environment");
    env.clk(clk_signal);
    env.heater_heat(heater_output);
    env.temperature(temperature);

    Thermostat thermo("thermostat");
    thermo.temperature(temperature);
    thermo.setpoint(setpoint_signal);
    thermo.heater_on(heater_control);

    Heater heater("heater");
    heater.control(heater_control);
    heater.heat_output(heater_output);

    // Export dependency graph for visualization
    digsim::dependency_graph.export_dot("thermostat_example.dot");

    digsim::info("Main", "=== Initializing thermostat simulation ===");

    digsim::scheduler.initialize();

    digsim::info("Main", "=== Running simulation for 20 time units ===");

    // Run simulation for 20 time units
    digsim::scheduler.run(20);

    digsim::info("Main", "=== Simulation finished ===");

    return 0;
}