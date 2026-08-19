/**
 * @file test_EventReader.cpp
 * @brief Test checklist for EventReader file/stream loading.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies. Use the JSON files under sample-data/ (especially
 *         malformed-events.json) as fixtures once file-based fixtures are wired up.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/EventReader.hpp"

TEST_CASE("readFromFile throws FileError for a nonexistent path", "[EventReader]") {}

TEST_CASE("readFromFile loads all events from a well-formed file", "[EventReader]") {}

TEST_CASE("readFromStream throws ParseError for invalid JSON", "[EventReader]") {}

TEST_CASE("readFromStream throws ValidationError when the top-level \"events\" key is missing",
          "[EventReader]") {}

TEST_CASE("readFromStream throws ValidationError for an event with a missing timestamp",
          "[EventReader]") {}

TEST_CASE("readFromStream throws ValidationError for an event with an unknown eventType",
          "[EventReader]") {}

TEST_CASE("readFromStream throws ValidationError for an event with sourceIp as a number",
          "[EventReader]") {}

TEST_CASE("readFromStream handles an empty events array", "[EventReader]") {}
