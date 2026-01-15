/// @file isignal.cpp
/// @brief

#include "simcore/isignal.hpp"

#include "simcore/module.hpp"

namespace simcore
{

std::string get_signal_location_string(const isignal_t *signal)
{
    if (!signal) {
        return "null";
    }
    std::stringstream ss;
    if (!signal) {
        return "<null signal>";
    }
    // Print module hierarchy if owner is known.
    module_t *module = signal->get_owner();
    std::vector<std::string> hierarchy;

    while (module) {
        hierarchy.push_back(module->get_name());
        module = module->get_parent();
    }

    // Build hierarchy string in reverse order (top-down)
    for (auto it = hierarchy.rbegin(); it != hierarchy.rend(); ++it) {
        ss << *it << "::";
    }

    ss << signal->get_name();
    return ss.str();
}

} // namespace simcore
