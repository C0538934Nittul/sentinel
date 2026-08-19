/**
 * @file test_IncidentResult.cpp
 * @brief Tests for IncidentResult serialization.
 * @component analyzer/tests
 * @status Filled in (Step 3, Phase 5) against the first-draft IncidentResult::toJson.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/IncidentResult.hpp"

using namespace sentinel;
using namespace std::chrono;

namespace {
IncidentResult sample() {
    const auto detectedAt = sys_days{year{2026} / March / 1} + hours(10) + minutes(2) + milliseconds(500);
    return IncidentResult("REPEATED_AUTH_FAILURE", Severity::High, 80, detectedAt,
                           {"evt-010", "evt-011", "evt-012"}, "10 failures from one source");
}
}  // namespace

TEST_CASE("toJson produces all fields from docs/data-contract.md except incidentId/status",
          "[IncidentResult]") {
    const auto json = sample().toJson();
    REQUIRE(json.contains("ruleId"));
    REQUIRE(json.contains("severity"));
    REQUIRE(json.contains("riskScore"));
    REQUIRE(json.contains("detectedAt"));
    REQUIRE(json.contains("supportingEventIds"));
    REQUIRE(json.contains("summary"));
    REQUIRE_FALSE(json.contains("incidentId"));
    REQUIRE_FALSE(json.contains("status"));
    REQUIRE(json.at("ruleId") == "REPEATED_AUTH_FAILURE");
    REQUIRE(json.at("riskScore") == 80);
}

TEST_CASE("toJson formats detectedAt as ISO 8601 UTC with millisecond precision",
          "[IncidentResult]") {
    const auto json = sample().toJson();
    REQUIRE(json.at("detectedAt").get<std::string>() == "2026-03-01T10:02:00.500Z");
}

TEST_CASE("toJson serializes severity using toString(Severity)", "[IncidentResult]") {
    REQUIRE(sample().toJson().at("severity") == "HIGH");
}

TEST_CASE("toJson preserves the order of supportingEventIds", "[IncidentResult]") {
    const auto json = sample().toJson();
    const std::vector<std::string> ids = json.at("supportingEventIds").get<std::vector<std::string>>();
    REQUIRE(ids == std::vector<std::string>{"evt-010", "evt-011", "evt-012"});
}

TEST_CASE("toString(Severity) covers Low, Medium, and High", "[IncidentResult]") {
    REQUIRE(toString(Severity::Low) == "LOW");
    REQUIRE(toString(Severity::Medium) == "MEDIUM");
    REQUIRE(toString(Severity::High) == "HIGH");
}
