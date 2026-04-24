#include "core/Log.h"
#include <iostream>

std::vector<LogEntry> Log::entries;

void Log::info (const std::string& msg) { add(LogLevel::Info,  "[INFO]  " + msg); }
void Log::warn (const std::string& msg) { add(LogLevel::Warn,  "[WARN]  " + msg); }
void Log::error(const std::string& msg) { add(LogLevel::Error, "[ERROR] " + msg); }
void Log::gl   (const std::string& msg) { add(LogLevel::GL,    "[GL]    " + msg); }

void Log::add(LogLevel level, const std::string& msg) {
    std::cerr << msg << "\n";
    entries.push_back({level, msg});
}
