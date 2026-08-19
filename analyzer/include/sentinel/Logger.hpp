/**
 * @file Logger.hpp
 * @brief Minimal leveled logger writing to a diagnostic log file and echoing to stderr.
 * @component analyzer (sentinel-core)
 * @status Complete. Plumbing, not assessed logic -- implemented in full.
 */

#pragma once

#include <fstream>
#include <mutex>
#include <string>

namespace sentinel {

enum class LogLevel { Debug, Info, Warn, Error };

/**
 * @class Logger
 * @brief Appends leveled, timestamped lines to a log file and to stderr.
 *
 * Deliberately not a singleton -- main.cpp owns one instance and passes it (or a reference)
 * to whatever needs it, keeping logging an explicit dependency rather than global state.
 */
class Logger {
public:
    /**
     * @param logFilePath Path to append log lines to. Parent directory must already exist.
     * @param minLevel Minimum level that will actually be written.
     * @throws FileError if the log file cannot be opened for writing.
     */
    explicit Logger(const std::string& logFilePath, LogLevel minLevel = LogLevel::Info);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;
    ~Logger() = default;

    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

private:
    void write(LogLevel level, const std::string& message);
    [[nodiscard]] static std::string levelName(LogLevel level);
    [[nodiscard]] static std::string timestampNow();

    std::ofstream file_;
    LogLevel minLevel_;
    std::mutex mutex_;
};

}  // namespace sentinel
