/**
 * @file test_EventReader.cpp
 * @brief Tests for EventReader file/stream loading.
 * @component analyzer/tests
 * @status Filled in (Step 3, Phase 5) against the first-draft EventReader. Self-contained --
 *         writes its own temp fixture files rather than depending on sample-data/ being
 *         reachable relative to ctest's working directory.
 */

#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "sentinel/EventReader.hpp"
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
}  // namespace

TEST_CASE("readFromFile throws FileError for a nonexistent path", "[EventReader]") {
    REQUIRE_THROWS_AS(EventReader::readFromFile("/nonexistent/path/events.json"), FileError);
}

TEST_CASE("readFromFile loads all events from a well-formed file", "[EventReader]") {
    const auto path = writeTempFile("sentinel-test-events-valid.json", R"({
        "events": [
            {"eventId":"e1","timestamp":"2026-03-01T10:00:00.000Z","eventType":"AUTH_FAILURE","sourceIp":"203.0.113.1","host":"h1"},
            {"eventId":"e2","timestamp":"2026-03-01T10:00:01.000Z","eventType":"AUTH_SUCCESS","sourceIp":"203.0.113.1","host":"h1"}
        ]
    })");
    const auto events = EventReader::readFromFile(path);
    REQUIRE(events.size() == 2);
}

TEST_CASE("readFromStream throws ParseError for invalid JSON", "[EventReader]") {
    std::istringstream input("{ this is not valid json");
    REQUIRE_THROWS_AS(EventReader::readFromStream(input), ParseError);
}

TEST_CASE("readFromStream throws ValidationError when the top-level \"events\" key is missing",
          "[EventReader]") {
    std::istringstream input(R"({"notEvents": []})");
    REQUIRE_THROWS_AS(EventReader::readFromStream(input), ValidationError);
}

TEST_CASE("readFromStream skips (does not throw for) an event with a missing timestamp",
          "[EventReader]") {
    // The file-level wrapper is valid, but the only event in it is not -- readFromStream skips
    // invalid events rather than throwing at the file level (see docs/ASSUMPTIONS.md).
    std::istringstream input(R"({"events":[{"eventId":"e1","eventType":"AUTH_FAILURE","sourceIp":"203.0.113.1","host":"h1"}]})");
    const auto events = EventReader::readFromStream(input);
    REQUIRE(events.empty());
}

TEST_CASE("readFromStream skips (does not throw for) an event with an unknown eventType",
          "[EventReader]") {
    std::istringstream input(R"({"events":[{"eventId":"e1","timestamp":"2026-03-01T10:00:00.000Z","eventType":"NOPE","sourceIp":"203.0.113.1","host":"h1"}]})");
    const auto events = EventReader::readFromStream(input);
    REQUIRE(events.empty());
}

TEST_CASE("readFromStream skips (does not throw for) an event with sourceIp as a number",
          "[EventReader]") {
    std::istringstream input(R"({"events":[{"eventId":"e1","timestamp":"2026-03-01T10:00:00.000Z","eventType":"AUTH_FAILURE","sourceIp":123,"host":"h1"}]})");
    const auto events = EventReader::readFromStream(input);
    REQUIRE(events.empty());
}

TEST_CASE("readFromStream handles an empty events array", "[EventReader]") {
    std::istringstream input(R"({"events":[]})");
    const auto events = EventReader::readFromStream(input);
    REQUIRE(events.empty());
}
