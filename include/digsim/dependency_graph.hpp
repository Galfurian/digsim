/// @file dependency_graph.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include "digsim/common.hpp"

#include <unordered_map>
#include <unordered_set>

namespace digsim
{

class module_t;  // Forward declare module base.
class isignal_t; // Forward declare abstract signal.

using path_t = std::vector<const isignal_t *>;

/// @brief Process information structure that contains details about the process
/// that produces or consumes a signal.
class dependency_graph_t
{
public:
    /// @brief Singleton instance of the dependency graph.
    /// @return A reference to the singleton instance of the dependency graph.
    static dependency_graph_t &instance();

    /// @brief Registers a signal producer in the dependency graph.
    /// @param signal The signal interface that is being produced.
    /// @param proc_info Information about the process producing the signal.
    void register_signal_producer(const isignal_t *signal, const process_info_t &proc_info);

    /// @brief Registers a signal consumer in the dependency graph.
    /// @param signal The signal interface that is being consumed.
    /// @param proc_info Information about the process consuming the signal.
    void register_signal_consumer(const isignal_t *signal, const process_info_t &proc_info);

    /// @brief Exports the dependency graph to a DOT file for visualization.
    /// @param filename The name of the file to export the graph to (default is "dependency_graph.dot").
    void export_dot(const std::string &filename = "dependency_graph.dot") const;

    /// @brief Checks if the dependency graph has a cycle.
    /// @return if there is a cycle in the dependency graph, false otherwise.
    bool has_cycle() const;

    /// @brief Returns the cycles detected in the dependency graph.
    /// @return A vector of cycles, where each cycle is represented as a vector of signal interfaces.
    const std::vector<path_t> &get_cycles() const;

    /// @brief Computes cycles in the dependency graph.
    void compute_cycles();

    /// @brief Prints a report of the cycles detected in the dependency graph.
    void print_cycle_report(const path_t &cycle) const;

    /// @brief Checks if a given cycle is a bad cycle.
    /// @param cycle The cycle to check.
    /// @return true if the cycle is a bad cycle, false otherwise.
    bool is_bad_cycle(const path_t &cycle) const;

private:
    dependency_graph_t()                                      = default;
    ~dependency_graph_t()                                     = default;
    dependency_graph_t(const dependency_graph_t &)            = delete;
    dependency_graph_t &operator=(const dependency_graph_t &) = delete;
    dependency_graph_t(dependency_graph_t &&)                 = delete;
    dependency_graph_t &operator=(dependency_graph_t &&)      = delete;

    /// @brief Generates a random ID of the specified length.
    /// @param length the length of the random ID to generate (default is 8).
    /// @return a random ID as a string.
    std::string random_id(size_t length = 8) const;

    /// @brief Gets the list of inputs for a module.
    /// @param module The module to query.
    /// @return A const reference to a vector of inputs.
    const std::unordered_set<const isignal_t *> &get_inputs(const module_t *module) const;

    /// @brief Gets the list of outputs for a module.
    /// @param module The module to query.
    /// @return A const reference to a vector of outputs.
    const std::unordered_set<const isignal_t *> &get_outputs(const module_t *module) const;

    /// @brief Gets the list of producers for a given signal.
    /// @param sig The signal interface to query.
    /// @return A vector of process_info_t representing the producers of the signal.
    std::vector<process_info_t> get_consumers(const isignal_t *sig) const;

    /// @brief Gets the producer of a signal interface.
    /// @param sig The signal interface to query.
    /// @return A process_info_t representing the producer of the signal.
    const process_info_t *get_producer(const isignal_t *sig) const;

    /// @brief Gets all modules in the dependency graph.
    /// @return A vector of pointers to all modules in the graph.
    std::vector<const module_t *> get_all_modules() const;

    /// @brief Gets all signal interfaces in the dependency graph.
    /// @return A vector of pointers to all signal interfaces in the graph.
    std::vector<const isignal_t *> get_all_signals() const;

    /// @brief Updates the signal graph based on the current state of the dependency graph.
    void update_signal_graph();

    void dfs_cycle(
        const isignal_t *current,
        std::unordered_set<const isignal_t *> &visited,
        std::unordered_set<const isignal_t *> &stack,
        path_t &path);

    /// @brief Maps signal interfaces to their producers.
    std::unordered_map<const isignal_t *, process_info_t> signal_producers;
    /// @brief Maps signal interfaces to their consumers.
    std::unordered_map<const isignal_t *, std::vector<process_info_t>> signal_consumers;
    /// @brief Keep track of all the inputs of modules.
    std::unordered_map<const module_t *, std::unordered_set<const isignal_t *>> module_inputs;
    /// @brief Keep track of all the outputs of modules.
    std::unordered_map<const module_t *, std::unordered_set<const isignal_t *>> module_outputs;
    /// @brief Maps modules to their inputs.
    std::unordered_map<const isignal_t *, std::vector<const isignal_t *>> signal_graph;
    /// @brief A vector of cycles detected in the dependency graph.
    std::vector<path_t> cycles;
};

/// @brief A reference to the singleton instance of the scheduler, for convenience.
inline dependency_graph_t &dependency_graph = dependency_graph_t::instance();

} // namespace digsim
