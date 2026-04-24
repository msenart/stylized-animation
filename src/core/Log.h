/**
 * @file Log.h
 * @brief Static logging system that writes to stderr and buffers entries for the UI console.
 */
#pragma once
#include <string>
#include <vector>

/**
 * @brief Severity level of a log entry.
 */
enum class LogLevel { Info, Warn, Error, GL };

/**
 * @brief A single timestamped log entry.
 */
struct LogEntry {
    LogLevel    level;
    std::string message;
};

/**
 * @brief Global logger: mirrors every message to stderr and stores it for the ImGui console.
 *
 * All methods are static; no instance is needed.
 */
struct Log {
    static std::vector<LogEntry> entries; ///< All recorded entries (readable by the UI).

    /// @brief Logs an informational message. @param msg The message text.
    static void info (const std::string& msg);
    /// @brief Logs a warning message.       @param msg The message text.
    static void warn (const std::string& msg);
    /// @brief Logs an error message.        @param msg The message text.
    static void error(const std::string& msg);
    /// @brief Logs an OpenGL debug message. @param msg The raw GL debug string.
    static void gl   (const std::string& msg);

private:
    static void add(LogLevel level, const std::string& msg);
};
