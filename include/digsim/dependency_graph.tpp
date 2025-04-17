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

    // Optional: make signal nodes visually distinct
    for (const auto &[signal, _] : signal_producers) {
        ofs << "    \"" << signal->get_name() << "\" [shape=ellipse, style=filled, fillcolor=lightblue";
        if (signal->get_delay()) {
            ofs << ", label=\"" << signal->get_name() << "\\n(delay=" << signal->get_delay() << ")\"";
        }
        ofs << "];\n";
    }

    // Draw producer edges with delay
    for (const auto &[signal, info] : signal_producers) {
        ofs << "    \"" << info.module->get_name() << "\" -> \"" << signal->get_name() << "\" [label=\"produces\"];\n";
    }

    // Draw consumer edges
    for (const auto &[signal, consumers] : signal_consumers) {
        for (const auto &info : consumers) {
            ofs << "    \"" << signal->get_name() << "\" -> \"" << info.module->get_name()
                << "\" [label=\"consumed by\"];\n";
        }
    }

    ofs << "}\n";
}

} // namespace digsim
