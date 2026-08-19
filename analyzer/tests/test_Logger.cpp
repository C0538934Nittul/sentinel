/**
 * @file test_Logger.cpp
 * @brief Tests for Logger.
 * @component analyzer/tests
 * @status Filled in (Step 3, Phase 5). Logger itself was already fully implemented (Step 2),
 *         not new this phase, but its test bodies were still empty until now.
 */

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "sentinel/Logger.hpp"
#include "sentinel/SentinelException.hpp"

using namespace sentinel;

namespace {
std::string tempLogPath(const std::string& name) {
    return (std::filesystem::temp_directory_path() / name).string();
}

std::string readFile(const std::string& path) {
    std::ifstream in(path);
    std::ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}
}  // namespace

TEST_CASE("constructing a Logger with an unwritable path throws FileError", "[Logger]") {
    REQUIRE_THROWS_AS(Logger("/nonexistent-directory-xyz/log.txt"), FileError);
}

TEST_CASE("info/warn/error write a line containing the level and message", "[Logger]") {
    const auto path = tempLogPath("sentinel-test-logger-levels.log");
    {
        Logger logger(path, LogLevel::Debug);
        logger.info("hello info");
        logger.warn("hello warn");
        logger.error("hello error");
    }
    const auto contents = readFile(path);
    REQUIRE(contents.find("[INFO] hello info") != std::string::npos);
    REQUIRE(contents.find("[WARN] hello warn") != std::string::npos);
    REQUIRE(contents.find("[ERROR] hello error") != std::string::npos);
}

TEST_CASE("messages below minLevel are not written", "[Logger]") {
    const auto path = tempLogPath("sentinel-test-logger-minlevel.log");
    {
        Logger logger(path, LogLevel::Warn);
        logger.debug("should not appear");
        logger.info("should not appear either");
        logger.warn("should appear");
    }
    const auto contents = readFile(path);
    REQUIRE(contents.find("should not appear") == std::string::npos);
    REQUIRE(contents.find("should appear") != std::string::npos);
}

TEST_CASE("each log line is prefixed with an ISO 8601 UTC timestamp", "[Logger]") {
    const auto path = tempLogPath("sentinel-test-logger-timestamp.log");
    {
        Logger logger(path);
        logger.info("timestamped line");
    }
    const auto contents = readFile(path);
    // Format: [YYYY-MM-DDTHH:MM:SSZ] [INFO] timestamped line
    REQUIRE(contents.size() >= 22);
    REQUIRE(contents[0] == '[');
    REQUIRE(contents[5] == '-');
    REQUIRE(contents[8] == '-');
    REQUIRE(contents[11] == 'T');
    REQUIRE(contents.find('Z') != std::string::npos);
}
