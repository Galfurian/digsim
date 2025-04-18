/// @file dependency_graph.tpp
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
    signal_producers[signal] = proc_info;
}

void dependency_graph_t::register_signal_consumer(const isignal_t *signal, const process_info_t &proc_info)
{
    signal_consumers[signal].push_back(proc_info);
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
    std::unordered_set<const isignal_t *> all_signals;
    std::unordered_set<const module_t *> all_modules;

    ofs << "digraph DependencyGraph {\n";
    ofs << "    rankdir=LR;\n";
    // Collect all signals and modules.
    for (const auto &[signal_if, producer_info] : signal_producers) {
        auto signal = signal_if->get_bound_signal();
        if (signal) {
            all_signals.insert(signal);
        }
        auto module = reinterpret_cast<const module_t *>(producer_info.owner.ptr);
        if (module) {
            all_modules.insert(module);
        }
    }
    for (const auto &[signal_if, consumer_list] : signal_consumers) {
        auto signal = signal_if->get_bound_signal();
        if (signal) {
            all_signals.insert(signal);
            for (const auto &consumer_info : consumer_list) {
                auto module = reinterpret_cast<const module_t *>(consumer_info.owner.ptr);
                if (module) {
                    all_modules.insert(module);
                }
            }
        }
    }
    // Generate unique IDs.
    for (auto signal : all_signals) {
        signal_ids[signal] = "sig_" + this->random_id();
    }
    for (auto module : all_modules) {
        module_ids[module] = "mod_" + this->random_id();
    }
    // Emit signal nodes.
    for (auto signal : all_signals) {
        const std::string &id = signal_ids[signal];
        ofs << "    \"" << id << "\" [shape=ellipse, style=filled, fillcolor=lightblue, label=\"" << signal->get_name()
            << "\\n(type=" << signal->get_type_name() << ")";
        if (signal->get_delay() > 0)
            ofs << "\\n(delay=" << signal->get_delay() << ")";
        ofs << "\"];\n";
    }
    // Emit module nodes.
    for (auto module : all_modules) {
        const std::string &id = module_ids[module];
        ofs << "    \"" << id << "\" [shape=box, style=filled, fillcolor=lightgray, label=\"" << module->get_name()
            << "\"];\n";
    }
    // Emit producer edges.
    for (const auto &[signal_if, producer_info] : signal_producers) {
        auto signal = signal_if->get_bound_signal();
        auto module = reinterpret_cast<const module_t *>(producer_info.owner.ptr);
        if (signal && module) {
            ofs << "    \"" << module_ids[module] << "\" -> \"" << signal_ids[signal] << "\" [label=\"produces\"];\n";
        }
    }
    // Emit consumer edges.
    for (const auto &[signal_if, consumer_list] : signal_consumers) {
        auto signal = signal_if->get_bound_signal();
        if (signal) {
            for (const auto &consumer_info : consumer_list) {
                auto module = reinterpret_cast<const module_t *>(consumer_info.owner.ptr);
                if (module) {
                    ofs << "    \"" << signal_ids[signal] << "\" -> \"" << module_ids[module]
                        << "\" [label=\"consumes\"];\n";
                }
            }
        }
    }
    ofs << "}\n";
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

} // namespace digsim
