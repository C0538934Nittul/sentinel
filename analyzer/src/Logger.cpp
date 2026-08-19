/**
 * @file Logger.cpp
 * @brief Implementation of Logger.
 * @component analyzer (sentinel-core)
 * @status Complete.
 */

#include "sentinel/Logger.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

#include "sentinel/SentinelException.hpp"

namespace sentinel {

Logger::Logger(const std::string& logFilePath, LogLevel minLevel)
    : file_(logFilePath, std::ios::app), minLevel_(minLevel) {
    if (!file_.is_open()) {
        throw FileError("Logger: could not open log file: " + logFilePath);
    }
}

void Logger::debug(const std::string& message) { write(LogLevel::Debug, message); }
void Logger::info(const std::string& message) { write(LogLevel::Info, message); }
void Logger::warn(const std::string& message) { write(LogLevel::Warn, message); }
void Logger::error(const std::string& message) { write(LogLevel::Error, message); }

void Logger::write(LogLevel level, const std::string& message) {
    if (static_cast<int>(level) < static_cast<int>(minLevel_)) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    file_ << "[" << timestampNow() << "] [" << levelName(level) << "] " << message << '\n';
    file_.flush();
}

std::string Logger::levelName(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "DEBUG";
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warn:
            return "WARN";
        case LogLevel::Error:
            return "ERROR";
    }
    return "UNKNOWN";
}

std::string Logger::timestampNow() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t nowTimeT = std::chrono::system_clock::to_time_t(now);
    std::tm utcTm{};
#if defined(_WIN32)
    gmtime_s(&utcTm, &nowTimeT);
#else
    gmtime_r(&nowTimeT, &utcTm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&utcTm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

}  // namespace sentinel
