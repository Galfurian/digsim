/// @file logger.hpp
/// @brief Logger class and logging functions.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include <format>
#include <string>

namespace digsim
{

/// @brief Enumeration for log levels.
enum class log_level_t {
    none  = 0, ///< No logging.
    error = 1, ///< Errors that should be reported to the user.
    info  = 2, ///< Informational messages that are useful for understanding the program flow.
    debug = 3, ///< Debug messages that are useful for developers to trace the program execution.
    trace = 4, ///< Very detailed messages for tracing the program execution, usually only needed during development.
};

/// @brief The logger class for handling logging in the application.
class logger_t
{
public:
    /// @brief Get the singleton instance of the logger.
    /// @return a reference to the logger instance.
    static logger_t &instance()
    {
        static logger_t logger;
        return logger;
    }

    /// @brief Set the global log level.
    /// @param level the log level to set.
    void set_level(log_level_t level) noexcept;

    /// @brief Get the current global log level.
    /// @return the current global log level.
    log_level_t get_level() const noexcept;

    /// @brief Logs a message with the specified log level and source.
    /// @param level the log level of the message.
    /// @param source the source of the log message, typically the name of the module or component.
    /// @param msg the message to log.
    void log(log_level_t level, const std::string &source, const std::string &msg) noexcept;

private:
    /// @brief Private constructor to enforce singleton pattern.
    logger_t();

    /// @brief Turns a log level into a string representation.
    /// @param level the log level to convert to a string.
    /// @return a string representation of the log level.
    inline std::string level_to_str(log_level_t level) const noexcept
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

    /// @brief The current global log level.
    log_level_t global_level;
};

/// @brief Global logger instance for easy access.
inline digsim::logger_t &logger = digsim::logger_t::instance();

/// @brief Logs a message with the specified log level and source.
/// @param level the log level of the message.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
inline void log(log_level_t level, const std::string &src, const std::string &msg)
{
    digsim::logger.log(level, src, msg);
}

/// @brief Logs an error message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
inline void error(const std::string &src, const std::string &msg) { digsim::logger.log(log_level_t::error, src, msg); }

/// @brief Logs an informational message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
inline void info(const std::string &src, const std::string &msg) { digsim::logger.log(log_level_t::info, src, msg); }

/// @brief Logs a debug message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
inline void debug(const std::string &src, const std::string &msg) { digsim::logger.log(log_level_t::debug, src, msg); }

/// @brief Logs a trace message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
inline void trace(const std::string &src, const std::string &msg) { digsim::logger.log(log_level_t::trace, src, msg); }

/// @brief Logs a message with the specified log level and source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param level the log level of the message.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void log(log_level_t level, const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    digsim::log(level, source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs an error message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void error(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    digsim::error(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs an informational message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args> inline void info(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    digsim::info(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs a debug message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void debug(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    digsim::debug(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs an error message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void trace(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    digsim::trace(source, std::format(fmt, std::forward<Args>(args)...));
}

} // namespace digsim

#include "logger.tpp"
