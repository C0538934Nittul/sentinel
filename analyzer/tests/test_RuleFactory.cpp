/**
 * @file test_RuleFactory.cpp
 * @brief Test checklist for RuleFactory.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/RuleFactory.hpp"

TEST_CASE("buildRules constructs one rule per enabled entry in the config", "[RuleFactory]") {}

TEST_CASE("buildRules skips entries with enabled = false", "[RuleFactory]") {}

TEST_CASE("buildRules maps REPEATED_AUTH_FAILURE to RepeatedFailureRule", "[RuleFactory]") {}

TEST_CASE("buildRules maps SUCCESS_AFTER_FAILURES to SuccessAfterFailuresRule", "[RuleFactory]") {}

TEST_CASE("buildRules maps MULTI_ACCOUNT_PROBE to MultiAccountProbeRule", "[RuleFactory]") {}

TEST_CASE("buildRules throws ConfigError for an unrecognized rule id", "[RuleFactory]") {}

TEST_CASE("buildRules propagates each rule's configured threshold/window/severity/score",
          "[RuleFactory]") {}
