/**
 * @file test_ConfigReader.cpp
 * @brief Test checklist for ConfigReader.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies. Use config/rules.json as the primary fixture.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/ConfigReader.hpp"

TEST_CASE("readFromFile throws FileError for a nonexistent path", "[ConfigReader]") {}

TEST_CASE("readFromFile loads analysisWindowSeconds and all three rules from config/rules.json",
          "[ConfigReader]") {}

TEST_CASE("readFromFile parses each rule's severity string into the Severity enum",
          "[ConfigReader]") {}

TEST_CASE("readFromFile throws ConfigError for an unknown severity string", "[ConfigReader]") {}

TEST_CASE("readFromFile throws ConfigError when a required rule field is missing",
          "[ConfigReader]") {}

TEST_CASE("readFromFile preserves each rule's enabled flag", "[ConfigReader]") {}
