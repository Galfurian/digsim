/// @file logger.cpp
/// @brief Implementation of the logger class.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#include "simcore/logger.hpp"

#include "simcore/common.hpp"
#include "simcore/scheduler.hpp"

namespace simcore
{

logger_t::logger_t()
    : global_level(simcore::log_level_t::info)
{
    // Nothing to do here.
}

logger_t &logger_t::instance()
{
    static logger_t _logger;
    return _logger;
}

void logger_t::set_level(log_level_t level) noexcept { global_level = level; }

log_level_t logger_t::get_level() const noexcept { return global_level; }

void logger_t::log(log_level_t level, const std::string &source, const std::string &msg) noexcept
{
    if (level <= global_level) {
        std::cout << "[" << level_to_str(level) << "]";
        std::cout << " [" << std::left << std::setw(8) << format_time(scheduler.time()) << "]";
        std::cout << " [" << std::left << std::setw(12) << source << "]";
        std::cout << " " << msg << "\n";
    }
}

std::string logger_t::level_to_str(log_level_t level) const noexcept
{
    switch (level) {
    case log_level_t::error:
        return "ERR";
    case log_level_t::info:
        return "INF";
    case log_level_t::debug:
        return "DBG";
    case log_level_t::trace:
        return "TRC";
    default:
        return "LOG";
    }
}

void log(log_level_t level, const std::string &src, const std::string &msg) { simcore::logger.log(level, src, msg); }

void error(const std::string &src, const std::string &msg) { simcore::logger.log(log_level_t::error, src, msg); }

void info(const std::string &src, const std::string &msg) { simcore::logger.log(log_level_t::info, src, msg); }

void debug(const std::string &src, const std::string &msg) { simcore::logger.log(log_level_t::debug, src, msg); }

void trace(const std::string &src, const std::string &msg) { simcore::logger.log(log_level_t::trace, src, msg); }

} // namespace simcore
