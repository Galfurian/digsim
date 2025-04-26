/// @file dependency_graph.cpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "digsim/dependency_graph.hpp"

#include "digsim/module.hpp"

#include <fstream>
#include <iostream>
#include <random>

namespace digsim
{

dependency_graph_t &dependency_graph_t::instance()
{
    static dependency_graph_t instance;
    return instance;
}

void dependency_graph_t::register_signal_producer(const isignal_t *signal, const process_info_t &proc_info)
{
    // Check if the signal is already registered.
    if (signal_producers.count(signal) > 0) {
        return;
    }
    signal_producers[signal] = proc_info;
    // Track which signal is produced by which module.
    if (auto module = dynamic_cast<const module_t *>(proc_info.owner.ptr)) {
        // Add the signal to the module's outputs.
        if (module_outputs[module].count(signal) == 0) {
            module_outputs[module].insert(signal);
            digsim::trace(
                "dependency_graph_t", "Module `{}` produces signal `{}`", module->get_name(), signal->get_name());
        }
    }
}

void dependency_graph_t::register_signal_consumer(const isignal_t *signal, const process_info_t &proc_info)
{
    // Check if the signal is already registered.
    if (signal_consumers.count(signal) > 0) {
        return;
    }
    signal_consumers[signal].push_back(proc_info);
    // Track which module consumes this signal.
    if (auto module = dynamic_cast<const module_t *>(proc_info.owner.ptr)) {
        // Add the signal to the module's inputs.
        if (module_inputs[module].count(signal) == 0) {
            module_inputs[module].insert(signal);
            digsim::trace(
                "dependency_graph_t", "Module `{}` consumes signal `{}`", module->get_name(), signal->get_name());
        }
    }
}

void dependency_graph_t::export_dot(const std::string &filename) const
{
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    std::unordered_map<const isignal_t *, std::string> signal_ids;
    std::unordered_map<const module_t *, std::string> module_ids;
    auto all_signals = get_all_signals();
    auto all_modules = get_all_modules();

    // Generate unique IDs.
    for (auto signal : all_signals) {
        signal_ids[signal] = "sig_" + this->random_id();
    }
    for (auto module : all_modules) {
        module_ids[module] = "mod_" + this->random_id();
    }

    // ========================================================================
    // Write the DOT file header.

    ofs << "digraph DependencyGraph {\n";
    ofs << "    rankdir=LR;      // Left to right layout\n";
    ofs << "    nodesep=0.50;    // Space between nodes\n";
    ofs << "    ranksep=0.75;    // Space between ranks\n";
    ofs << "    splines=ortho;   // Use orthogonal edges for clarity\n";
    ofs << "    node [fontname=\"Courier New\"];\n";

    // ========================================================================
    // Set the style for the nodes.
    ofs << "    node [shape=ellipse, fontsize=10, fillcolor=white, style=filled, width=1.0, height=1.0];\n";
    // Emit the nodes for signals.
    for (auto signal : all_signals) {
        ofs << "    \"" << signal_ids[signal] << "\" [label=\"" << signal->get_name() << "\\n("
            << signal->get_type_name();
        // If the signal has a delay, include it in the label.
        if (signal->get_delay() > 0) {
            ofs << ", " << signal->get_delay();
        }
        ofs << ")\"];\n";
    }

    // ========================================================================
    // Set the style for the modules.
    ofs << "    node [shape=box, fontsize=10, fillcolor=\"#D0E7FF\", style=\"filled,rounded\", width=1.0, "
           "height=0.5];\n";
    // Emit the nodes for modules.
    for (auto module : all_modules) {
        const auto &inputs  = get_inputs(module);
        const auto &outputs = get_outputs(module);
        // Use the maximum of inputs and outputs to determine the height. This ensures that the module node is tall
        // enough to accommodate its inputs and outputs.
        auto max_signals    = std::max(inputs.size(), outputs.size());
        // Set the height based on the number of signals.
        float height        = 1.00f;
        if (max_signals > 0) {
            height = 1.00f * static_cast<float>(max_signals);
        }
        ofs << "    \"" << module_ids[module] << "\" [label=\"" << module->get_name() << "\", height=" << height
            << "];\n";
    }

    // ========================================================================
    // Emit producer edges.
    for (const auto &[signal_if, producer_info] : signal_producers) {
        auto signal = signal_if->get_bound_signal();
        auto module = reinterpret_cast<const module_t *>(producer_info.owner.ptr);
        if (signal && module) {
            ofs << "    \"" << module_ids[module] << "\" -> \"" << signal_ids[signal] << "\";\n";
        }
    }

    // ========================================================================
    // Emit consumer edges.
    for (const auto &[signal_if, consumer_list] : signal_consumers) {
        auto signal = signal_if->get_bound_signal();
        if (signal) {
            for (const auto &consumer_info : consumer_list) {
                auto module = reinterpret_cast<const module_t *>(consumer_info.owner.ptr);
                if (module) {
                    ofs << "    \"" << signal_ids[signal] << "\" -> \"" << module_ids[module] << "\";\n";
                }
            }
        }
    }
    ofs << "}\n";
}

bool dependency_graph_t::has_cycle() const { return !cycles.empty(); }

const std::vector<std::vector<const isignal_t *>> &dependency_graph_t::get_cycles() const { return cycles; }

void dependency_graph_t::compute_cycles()
{
    this->update_signal_graph();
    cycles.clear();
    std::unordered_set<const isignal_t *> visited;
    std::unordered_set<const isignal_t *> stack;
    std::vector<const isignal_t *> path;
    for (const auto &[sig, _] : signal_graph) {
        if (visited.count(sig) == 0) {
            dfs_cycle(sig, visited, stack, path);
        }
    }
}

void dependency_graph_t::print_cycle_report(const path_t &cycle) const
{
    digsim::info("dependency_graph_t", "Cycle:");
    for (const auto *signal : cycle) {
        const isignal_t *output_port    = nullptr;
        const module_t *signal_producer = nullptr;
        // Get producer (if any).
        for (const auto &[port, proc_info] : signal_producers) {
            // Get the producer module.
            const module_t *producer_module = dynamic_cast<const module_t *>(proc_info.owner.ptr);
            if (!producer_module) {
                continue;
            }
            // Get the signal interface.
            const isignal_t *bound_signal = port->get_bound_signal();
            if (!bound_signal) {
                continue;
            }
            if (bound_signal == signal) {
                // Found the producer for this signal.
                signal_producer = producer_module;
                output_port     = port;
                break;
            }
        }
        if (signal_producer && output_port) {
            // If a producer is found, print the signal name and its producer.
            digsim::info(
                "dependency_graph_t", "  - {} [{}.{}, delay: {}]", signal->get_name(), signal_producer->get_name(),
                output_port->get_name(), signal->get_delay());
        } else {
            // If no producer is found, just print the signal name.
            digsim::info("dependency_graph_t", "  - {} [delay: {}]", signal->get_name(), signal->get_delay());
        }
    }
    // Close the loop
    if (!cycle.empty()) {
        digsim::info("dependency_graph_t", "  - Back to {}.", cycle.front()->get_name());
    }
}

bool dependency_graph_t::is_bad_cycle(const path_t &cycle) const
{
    // Check if the cycle contains any bad signals.
    for (const auto *signal : cycle) {
        // Not a bad cycle: at least one signal has a delay.
        if (signal->get_delay() > 0) {
            return false;
        }
    }
    // All signals have zero delay: this is a problematic cycle.
    return true;
}

inline std::string dependency_graph_t::random_id(size_t length) const
{
    static const char charset[] = "0123456789abcdef";
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string result;
    for (size_t i = 0; i < length; ++i)
        result += charset[dist(rng)];
    return result;
}

const std::unordered_set<const isignal_t *> &dependency_graph_t::get_inputs(const module_t *module) const
{
    static const std::unordered_set<const isignal_t *> empty;
    auto it = module_inputs.find(module);
    return (it != module_inputs.end()) ? it->second : empty;
}

const std::unordered_set<const isignal_t *> &dependency_graph_t::get_outputs(const module_t *module) const
{
    static const std::unordered_set<const isignal_t *> empty;
    auto it = module_outputs.find(module);
    return (it != module_outputs.end()) ? it->second : empty;
}

std::vector<process_info_t> dependency_graph_t::get_consumers(const isignal_t *sig) const
{
    auto it = signal_consumers.find(sig);
    if (it != signal_consumers.end()) {
        return it->second;
    }
    return {};
}

const process_info_t *dependency_graph_t::get_producer(const isignal_t *sig) const
{
    auto it = signal_producers.find(sig);
    if (it != signal_producers.end()) {
        return &it->second;
    }
    // Return an empty process_info_t if not found.
    return nullptr;
}

std::vector<const module_t *> dependency_graph_t::get_all_modules() const
{
    std::vector<const module_t *> result;
    result.reserve(module_inputs.size());
    for (const auto &pair : module_inputs) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<const isignal_t *> dependency_graph_t::get_all_signals() const
{
    std::unordered_set<const isignal_t *> all;
    for (const auto &pair : signal_producers) {
        // Get the bound signal.
        if (const auto *bound = pair.first->get_bound_signal()) {
            all.insert(bound);
        }
    }
    for (const auto &pair : signal_consumers) {
        // Get the bound signal.
        if (const auto *bound = pair.first->get_bound_signal()) {
            all.insert(bound);
        }
    }
    return std::vector<const isignal_t *>(all.begin(), all.end());
}

void dependency_graph_t::update_signal_graph()
{
    signal_graph.clear();

    // For each signal in the system.
    for (const auto &[iface_signal, _] : signal_producers) {
        // Get the signal interface and check if it's bound.
        const isignal_t *signal = iface_signal->get_bound_signal();
        if (!signal) {
            continue;
        }
        digsim::trace(
            "dependency_graph_t", "Output signal `{}` is bound to `{}`...", iface_signal->get_name(),
            signal->get_name());
        // For each signal interface in consumers...
        for (const auto &[consumer_iface, consumer_list] : signal_consumers) {
            // Check if the consumer signal is bound to the same signal.
            const isignal_t *consumer_signal = consumer_iface->get_bound_signal();
            if (!consumer_signal || consumer_signal != signal) {
                continue;
            }
            digsim::trace(
                "dependency_graph_t", "    Signal `{}` is in turn bound to input signal `{}`...", signal->get_name(),
                consumer_iface->get_name());
            // Now we know signal is bound to both an output and input — it connects them.
            // Link signal to each output of the consumer's module.
            for (const auto &proc : consumer_list) {
                // Get the module that consumes this signal.
                const module_t *consumer_module = dynamic_cast<const module_t *>(proc.owner.ptr);
                if (!consumer_module) {
                    continue;
                }
                // Add the signal to the graph.
                for (const auto *out_port : get_outputs(consumer_module)) {
                    if (auto *bound = out_port->get_bound_signal()) {
                        signal_graph[signal].push_back(bound);
                        digsim::trace(
                            "dependency_graph_t", "        Link resolved signal `{}` -> `{}`...", signal->get_name(),
                            bound->get_name());
                    }
                }
            }
        }
    }
}

void dependency_graph_t::dfs_cycle(
    const isignal_t *current,
    std::unordered_set<const isignal_t *> &visited,
    std::unordered_set<const isignal_t *> &stack,
    std::vector<const isignal_t *> &path)
{
    visited.insert(current);
    stack.insert(current);
    path.push_back(current);
    for (auto *neighbor : signal_graph[current]) {
        if (stack.count(neighbor)) {
            auto it = std::find(path.begin(), path.end(), neighbor);
            if (it != path.end()) {
                cycles.emplace_back(it, path.end());
            }
        } else if (visited.count(neighbor) == 0) {
            dfs_cycle(neighbor, visited, stack, path);
        }
    }
    stack.erase(current);
    path.pop_back();
}

} // namespace digsim
