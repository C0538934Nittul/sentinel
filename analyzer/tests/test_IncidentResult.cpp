/**
 * @file test_IncidentResult.cpp
 * @brief Test checklist for IncidentResult serialization.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies -- fill each in as you implement IncidentResult::toJson.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/IncidentResult.hpp"

TEST_CASE("toJson produces all fields from docs/data-contract.md except incidentId/status",
          "[IncidentResult]") {}

TEST_CASE("toJson formats detectedAt as ISO 8601 UTC with millisecond precision",
          "[IncidentResult]") {}

TEST_CASE("toJson serializes severity using toString(Severity)", "[IncidentResult]") {}

TEST_CASE("toJson preserves the order of supportingEventIds", "[IncidentResult]") {}

TEST_CASE("toString(Severity) covers Low, Medium, and High", "[IncidentResult]") {}
