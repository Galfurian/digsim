/// @file dependency_graph.hpp
/// @brief
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include <unordered_map>
#include <unordered_set>

namespace digsim
{

class module_t;  // Forward declare module base
class isignal_t; // Forward declare abstract signal

class dependency_graph_t
{
public:
    static dependency_graph_t &instance();

    void register_signal_producer(const isignal_t *signal, std::shared_ptr<process_t> process, module_t *module);
    void register_signal_consumer(const isignal_t *signal, std::shared_ptr<process_t> process, module_t *module);

    void export_dot(const std::string &filename = "dependency_graph.dot") const;

private:
    /// @brief Generates a random ID of the specified length.
    /// @param length the length of the random ID to generate (default is 8).
    /// @return a random ID as a string.
    std::string random_id(size_t length = 8) const;

    struct process_info_t {
        std::shared_ptr<process_t> process;
        module_t *module;
    };

    dependency_graph_t()                                      = default;
    ~dependency_graph_t()                                     = default;
    dependency_graph_t(const dependency_graph_t &)            = delete;
    dependency_graph_t &operator=(const dependency_graph_t &) = delete;
    dependency_graph_t(dependency_graph_t &&)                 = delete;
    dependency_graph_t &operator=(dependency_graph_t &&)      = delete;

    std::unordered_map<const isignal_t *, process_info_t> signal_producers;
    std::unordered_map<const isignal_t *, std::vector<process_info_t>> signal_consumers;
};

/// @brief A reference to the singleton instance of the scheduler, for convenience.
inline dependency_graph_t &dependency_graph = dependency_graph_t::instance();

} // namespace digsim

#include "dependency_graph.tpp"