/// @file dependency_graph.tpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "digsim/dependency_graph.hpp"

#include "digsim/module.hpp"

#include <fstream>
#include <iostream>

namespace digsim
{

dependency_graph_t &dependency_graph_t::instance()
{
    static dependency_graph_t instance;
    return instance;
}

void dependency_graph_t::register_signal_producer(
    const isignal_t *signal,
    std::shared_ptr<process_t> process,
    module_t *module)
{
    signal_producers[signal] = {process, module};
}

void dependency_graph_t::register_signal_consumer(
    const isignal_t *signal,
    std::shared_ptr<process_t> process,
    module_t *module)
{
    signal_consumers[signal].push_back({process, module});
}

void dependency_graph_t::export_dot(const std::string &filename) const
{
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return;
    }

    ofs << "digraph DependencyGraph {\n";
    ofs << "    rankdir=LR;\n";
    ofs << "    node [shape=box, style=filled, fillcolor=lightgray];\n";

    std::unordered_set<const isignal_t *> all_signals;
    for (const auto &[signal_if, _] : signal_producers) {
        if (auto signal = signal_if->get_bound_signal()) {
            all_signals.insert(signal);
        }
    }
    for (const auto &[signal_if, _] : signal_consumers) {
        if (auto signal = signal_if->get_bound_signal()) {
            all_signals.insert(signal);
        }
    }
    for (const isignal_t *signal : all_signals) {
        ofs << "    \"" << signal->get_name() << "\" [shape=ellipse, style=filled, fillcolor=lightblue";
        ofs << ", label=\"" << signal->get_name() << "\\n(type=" << signal->get_type_name() << ")";
        if (signal->get_delay() > 0) {
            ofs << "\\n(delay=" << signal->get_delay() << ")";
        }
        ofs << "\"];\n";
    }

    // Producer edges: module -> module.signal
    for (const auto &[signal_if, producer_info] : signal_producers) {
        if (auto signal = signal_if->get_bound_signal()) {
            ofs << "    \"" << producer_info.module->get_name() << "\" -> \"" << signal->get_name()
                << "\" [label=\"produces\"];\n";
        }
    }

    // Consumer edges: module.signal -> consuming module
    for (const auto &[signal_if, consumer_list] : signal_consumers) {
        if (auto signal = signal_if->get_bound_signal()) {
            for (const auto &consumer_info : consumer_list) {
                ofs << "    \"" << signal->get_name() << "\" -> \"" << consumer_info.module->get_name()
                    << "\" [label=\"consumed by\"];\n";
            }
        }
    }

    ofs << "}\n";
}

} // namespace digsim
