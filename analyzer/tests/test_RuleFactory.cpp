/**
 * @file test_RuleFactory.cpp
 * @brief Tests for RuleFactory.
 * @component analyzer/tests
 * @status Filled in (Step 3, Phase 5) against the first-draft RuleFactory. Tests only that the
 *         right concrete type is constructed with the right id/severity/score -- evaluate()'s
 *         behavior is out of scope here (see test_RepeatedFailureRule.cpp etc., left empty).
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/RuleFactory.hpp"
#include "sentinel/SentinelException.hpp"

using namespace sentinel;

namespace {
RuleConfig makeRuleConfig(std::string id, bool enabled = true) {
    RuleConfig config;
    config.id = std::move(id);
    config.description = "test rule";
    config.windowSeconds = std::chrono::seconds(120);
    config.threshold = 10;
    config.severity = Severity::High;
    config.score = 80;
    config.enabled = enabled;
    return config;
}
}  // namespace

TEST_CASE("buildRules constructs one rule per enabled entry in the config", "[RuleFactory]") {
    AnalysisConfig config;
    config.analysisWindowSeconds = std::chrono::seconds(300);
    config.rules = {makeRuleConfig("REPEATED_AUTH_FAILURE"), makeRuleConfig("MULTI_ACCOUNT_PROBE")};
    const auto rules = RuleFactory::buildRules(config);
    REQUIRE(rules.size() == 2);
}

TEST_CASE("buildRules skips entries with enabled = false", "[RuleFactory]") {
    AnalysisConfig config;
    config.analysisWindowSeconds = std::chrono::seconds(300);
    config.rules = {makeRuleConfig("REPEATED_AUTH_FAILURE", true),
                     makeRuleConfig("MULTI_ACCOUNT_PROBE", false)};
    const auto rules = RuleFactory::buildRules(config);
    REQUIRE(rules.size() == 1);
    REQUIRE(rules.at(0)->id() == "REPEATED_AUTH_FAILURE");
}

TEST_CASE("buildRules maps REPEATED_AUTH_FAILURE to RepeatedFailureRule", "[RuleFactory]") {
    AnalysisConfig config;
    config.rules = {makeRuleConfig("REPEATED_AUTH_FAILURE")};
    const auto rules = RuleFactory::buildRules(config);
    REQUIRE(rules.size() == 1);
    REQUIRE(rules.at(0)->id() == "REPEATED_AUTH_FAILURE");
}

TEST_CASE("buildRules maps SUCCESS_AFTER_FAILURES to SuccessAfterFailuresRule", "[RuleFactory]") {
    AnalysisConfig config;
    config.rules = {makeRuleConfig("SUCCESS_AFTER_FAILURES")};
    const auto rules = RuleFactory::buildRules(config);
    REQUIRE(rules.size() == 1);
    REQUIRE(rules.at(0)->id() == "SUCCESS_AFTER_FAILURES");
}

TEST_CASE("buildRules maps MULTI_ACCOUNT_PROBE to MultiAccountProbeRule", "[RuleFactory]") {
    AnalysisConfig config;
    config.rules = {makeRuleConfig("MULTI_ACCOUNT_PROBE")};
    const auto rules = RuleFactory::buildRules(config);
    REQUIRE(rules.size() == 1);
    REQUIRE(rules.at(0)->id() == "MULTI_ACCOUNT_PROBE");
}

TEST_CASE("buildRules throws ConfigError for an unrecognized rule id", "[RuleFactory]") {
    AnalysisConfig config;
    config.rules = {makeRuleConfig("NOT_A_REAL_RULE")};
    REQUIRE_THROWS_AS(RuleFactory::buildRules(config), ConfigError);
}

TEST_CASE("buildRules propagates each rule's configured threshold/window/severity/score",
          "[RuleFactory]") {
    AnalysisConfig config;
    auto ruleConfig = makeRuleConfig("REPEATED_AUTH_FAILURE");
    ruleConfig.severity = Severity::Medium;
    ruleConfig.score = 65;
    config.rules = {ruleConfig};
    const auto rules = RuleFactory::buildRules(config);
    REQUIRE(rules.at(0)->severity() == Severity::Medium);
    REQUIRE(rules.at(0)->score() == 65);
}
