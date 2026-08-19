/**
 * @file test_ConfigReader.cpp
 * @brief Tests for ConfigReader.
 * @component analyzer/tests
 * @status Filled in (Step 3, Phase 5) against the first-draft ConfigReader. Self-contained --
 *         writes its own temp fixture files.
 */

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

#include "sentinel/ConfigReader.hpp"
#include "sentinel/SentinelException.hpp"

using namespace sentinel;

namespace {
std::string writeTempFile(const std::string& name, const std::string& contents) {
    const auto path = std::filesystem::temp_directory_path() / name;
    std::ofstream out(path);
    out << contents;
    out.close();
    return path.string();
}

const char* kValidConfig = R"({
    "analysisWindowSeconds": 300,
    "rules": [
        {"id":"REPEATED_AUTH_FAILURE","windowSeconds":120,"threshold":10,"severity":"HIGH","score":80,"enabled":true},
        {"id":"SUCCESS_AFTER_FAILURES","windowSeconds":300,"threshold":5,"severity":"HIGH","score":90,"enabled":true},
        {"id":"MULTI_ACCOUNT_PROBE","windowSeconds":180,"threshold":5,"severity":"MEDIUM","score":65,"enabled":false}
    ]
})";
}  // namespace

TEST_CASE("readFromFile throws FileError for a nonexistent path", "[ConfigReader]") {
    REQUIRE_THROWS_AS(ConfigReader::readFromFile("/nonexistent/path/rules.json"), FileError);
}

TEST_CASE("readFromFile loads analysisWindowSeconds and all three rules from config/rules.json",
          "[ConfigReader]") {
    const auto path = writeTempFile("sentinel-test-config-valid.json", kValidConfig);
    const auto config = ConfigReader::readFromFile(path);
    REQUIRE(config.analysisWindowSeconds == std::chrono::seconds(300));
    REQUIRE(config.rules.size() == 3);
}

TEST_CASE("readFromFile parses each rule's severity string into the Severity enum",
          "[ConfigReader]") {
    const auto path = writeTempFile("sentinel-test-config-severity.json", kValidConfig);
    const auto config = ConfigReader::readFromFile(path);
    REQUIRE(config.rules.at(0).severity == Severity::High);
    REQUIRE(config.rules.at(2).severity == Severity::Medium);
}

TEST_CASE("readFromFile throws ConfigError for an unknown severity string", "[ConfigReader]") {
    const auto path = writeTempFile("sentinel-test-config-bad-severity.json", R"({
        "analysisWindowSeconds": 300,
        "rules": [{"id":"X","windowSeconds":10,"threshold":1,"severity":"CATASTROPHIC","score":1}]
    })");
    REQUIRE_THROWS_AS(ConfigReader::readFromFile(path), ConfigError);
}

TEST_CASE("readFromFile throws ConfigError when a required rule field is missing",
          "[ConfigReader]") {
    const auto path = writeTempFile("sentinel-test-config-missing-field.json", R"({
        "analysisWindowSeconds": 300,
        "rules": [{"id":"X","threshold":1,"severity":"HIGH","score":1}]
    })");
    REQUIRE_THROWS_AS(ConfigReader::readFromFile(path), ConfigError);
}

TEST_CASE("readFromFile preserves each rule's enabled flag", "[ConfigReader]") {
    const auto path = writeTempFile("sentinel-test-config-enabled.json", kValidConfig);
    const auto config = ConfigReader::readFromFile(path);
    REQUIRE(config.rules.at(0).enabled == true);
    REQUIRE(config.rules.at(2).enabled == false);
}
