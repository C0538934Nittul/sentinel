/**
 * @file RuleFactory.cpp
 * @brief Implementation of RuleFactory.
 * @component analyzer (sentinel-core)
 * @status STUBBED -- TODO(student).
 */

#include "sentinel/RuleFactory.hpp"

#include "sentinel/MultiAccountProbeRule.hpp"
#include "sentinel/RepeatedFailureRule.hpp"
#include "sentinel/SentinelException.hpp"
#include "sentinel/SuccessAfterFailuresRule.hpp"

namespace sentinel {

std::vector<std::unique_ptr<DetectionRule>> RuleFactory::buildRules(const AnalysisConfig& config) {
    // TODO(student): for each enabled RuleConfig in config.rules, construct the matching
    // concrete rule (RepeatedFailureRule for "REPEATED_AUTH_FAILURE",
    // SuccessAfterFailuresRule for "SUCCESS_AFTER_FAILURES",
    // MultiAccountProbeRule for "MULTI_ACCOUNT_PROBE") via std::make_unique, passing its
    // threshold/window/severity/score from config. Throw ConfigError for an unrecognized id.
    // Stubbed to a safe empty result rather than throwing -- see docs/ASSUMPTIONS.md. With
    // ConfigReader also stubbed to return zero rules, this is also literally correct today.
    (void)config;
    return {};
}

}  // namespace sentinel
