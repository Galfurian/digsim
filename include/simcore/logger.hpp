/// @file logger.hpp
/// @brief Logger class and logging functions.
/// @copyright
/// This file is distributed under the terms of the MIT License.
/// See the full license in the root directory at LICENSE.md.

#pragma once

#include <bitset>
#include <format>
#include <string>

namespace std
{
/// @brief Custom formatter for std::bitset to allow formatting with std::format.
/// @tparam N the size of the bitset.
template <std::size_t N>
struct formatter<std::bitset<N>, char> {
    /// @brief Parses the format string for std::bitset.
    /// @param ctx the format parse context.
    /// @return the beginning of the format string.
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    /// @brief Formats the std::bitset as a string.
    /// @param bits the std::bitset to format.
    /// @param ctx the format context.
    /// @return the formatted string.
    template <typename FormatContext>
    auto format(const std::bitset<N> &bits, FormatContext &ctx) const
    {
        return std::format_to(ctx.out(), "{}", bits.to_string());
    }
};
} // namespace std

namespace simcore
{

/// @brief Enumeration for log levels.
enum class log_level_t {
    none    = 0, ///< No logging.
    error   = 1, ///< Errors that should be reported to the user.
    warning = 2, ///< Warnings that indicate potential issues but do not stop execution.
    info    = 3, ///< Informational messages that are useful for understanding the program flow.
    debug   = 4, ///< Debug messages that are useful for developers to trace the program execution.
    trace   = 5, ///< Very detailed messages for tracing the program execution, usually only needed during development.
};

/// @brief The logger class for handling logging in the application.
class logger_t
{
public:
    /// @brief Get the singleton instance of the logger.
    /// @return a reference to the logger instance.
    static logger_t &instance();

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
    std::string level_to_str(log_level_t level) const noexcept;

    /// @brief The current global log level.
    log_level_t global_level;
};

/// @brief Global logger instance for easy access.
inline simcore::logger_t &logger = simcore::logger_t::instance();

/// @brief Sets the global log level.
/// @param level the log level to set.
inline void set_log_level(simcore::log_level_t level) noexcept
{
    simcore::logger.set_level(level);
}

/// @brief Gets the current global log level.
/// @return the current global log level.
inline simcore::log_level_t get_log_level() noexcept
{
    return simcore::logger.get_level();
}

/// @brief Logs a message with the specified log level and source.
/// @param level the log level of the message.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
void log(simcore::log_level_t level, const std::string &src, const std::string &msg);

/// @brief Logs an error message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
void error(const std::string &src, const std::string &msg);

/// @brief Logs a warning message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
void warning(const std::string &src, const std::string &msg);

/// @brief Logs an informational message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
void info(const std::string &src, const std::string &msg);

/// @brief Logs a debug message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
void debug(const std::string &src, const std::string &msg);

/// @brief Logs a trace message with the specified source.
/// @param src the source of the log message, typically the name of the module or component.
/// @param msg the message to log.
void trace(const std::string &src, const std::string &msg);

/// @brief Logs a message with the specified log level and source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param level the log level of the message.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void log(simcore::log_level_t level, const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    simcore::log(level, source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs an error message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void error(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    simcore::error(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs a warning message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void warning(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    simcore::warning(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs an informational message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void info(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    simcore::info(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs a debug message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void debug(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    simcore::debug(source, std::format(fmt, std::forward<Args>(args)...));
}

/// @brief Logs an error message with the specified source using a format string.
/// @tparam ...Args Variadic template arguments for the format string.
/// @param source the source of the log message, typically the name of the module or component.
/// @param fmt the format string for the message.
/// @param ...args the arguments to format the message.
template <typename... Args>
inline void trace(const std::string &source, std::format_string<Args...> fmt, Args &&...args)
{
    simcore::trace(source, std::format(fmt, std::forward<Args>(args)...));
}

} // namespace simcore
