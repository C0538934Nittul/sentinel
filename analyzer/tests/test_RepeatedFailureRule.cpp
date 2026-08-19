/**
 * @file test_RepeatedFailureRule.cpp
 * @brief Test checklist for RepeatedFailureRule::evaluate.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies -- this is a checklist for the assessed logic in
 *         RepeatedFailureRule::evaluate. Use sample-data/authentication-attack.json and
 *         sample-data/normal-events.json as fixtures once file-based fixtures are wired up.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/RepeatedFailureRule.hpp"

TEST_CASE("evaluate fires when failures from one source reach the threshold within the window",
          "[RepeatedFailureRule]") {}

TEST_CASE("evaluate does not fire when failures are below the threshold", "[RepeatedFailureRule]") {}

TEST_CASE("evaluate does not fire when failures are spread across more than the window",
          "[RepeatedFailureRule]") {}

TEST_CASE("evaluate does not count AUTH_SUCCESS or other event types toward the threshold",
          "[RepeatedFailureRule]") {}

TEST_CASE("evaluate treats different source IPs independently", "[RepeatedFailureRule]") {}

TEST_CASE("evaluate produced incidents carry the correct ruleId, severity, and score",
          "[RepeatedFailureRule]") {}

TEST_CASE("evaluate produced incidents reference the correct supportingEventIds",
          "[RepeatedFailureRule]") {}

TEST_CASE("evaluate on an empty event list produces no incidents", "[RepeatedFailureRule]") {}
