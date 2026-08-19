/**
 * @file test_MultiAccountProbeRule.cpp
 * @brief Test checklist for MultiAccountProbeRule::evaluate.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies -- checklist for assessed logic. Use
 *         sample-data/multi-account-probe.json as a fixture once wired up.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/MultiAccountProbeRule.hpp"

TEST_CASE("evaluate fires when one source fails against enough distinct accounts",
          "[MultiAccountProbeRule]") {}

TEST_CASE("evaluate does not fire when distinct account count is below the threshold",
          "[MultiAccountProbeRule]") {}

TEST_CASE("evaluate does not double-count repeated failures against the same account",
          "[MultiAccountProbeRule]") {}

TEST_CASE("evaluate does not fire when the probe spans more than the window",
          "[MultiAccountProbeRule]") {}

TEST_CASE("evaluate treats different source IPs independently", "[MultiAccountProbeRule]") {}

TEST_CASE("evaluate on an empty event list produces no incidents", "[MultiAccountProbeRule]") {}
