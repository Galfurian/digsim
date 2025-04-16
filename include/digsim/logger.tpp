/// @file logger.hpp
/// @brief Implementation of the logger class.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "logger.hpp"
#include "scheduler.hpp"

#pragma once

namespace digsim
{

logger_t::logger_t()
    : global_level(digsim::log_level_t::info)
{
    // Nothing to do here.
}

void logger_t::set_level(log_level_t level) noexcept { global_level = level; }

log_level_t logger_t::get_level() const noexcept { return global_level; }

void logger_t::log(log_level_t level, const std::string &source, const std::string &msg) noexcept
{
    if (level <= global_level) {
        std::cout << "[" << level_to_str(level) << "]";
        std::cout << " [" << std::right << std::setw(4) << scheduler.time() << "]";
        std::cout << " [" << std::left << std::setw(12) << source << "]";
        std::cout << " " << msg << "\n";
    }
}

} // namespace digsim
