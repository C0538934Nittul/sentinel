/**
 * @file test_Logger.cpp
 * @brief Test checklist for Logger.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies. Logger itself is fully implemented (not assessed), but its
 *         file-writing behaviour still deserves coverage.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/Logger.hpp"

TEST_CASE("constructing a Logger with an unwritable path throws FileError", "[Logger]") {}

TEST_CASE("info/warn/error write a line containing the level and message", "[Logger]") {}

TEST_CASE("messages below minLevel are not written", "[Logger]") {}

TEST_CASE("each log line is prefixed with an ISO 8601 UTC timestamp", "[Logger]") {}
