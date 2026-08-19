/**
 * @file RuleFactory.cpp
 * @brief Implementation of RuleFactory.
 * @component analyzer (sentinel-core)
 * @status First-draft implementation (Step 3, Phase 5). Not the final assessed version --
 *         review and rewrite as needed.
 */

#include "sentinel/RuleFactory.hpp"

#include "sentinel/MultiAccountProbeRule.hpp"
#include "sentinel/RepeatedFailureRule.hpp"
#include "sentinel/SentinelException.hpp"
#include "sentinel/SuccessAfterFailuresRule.hpp"

namespace sentinel {

std::vector<std::unique_ptr<DetectionRule>> RuleFactory::buildRules(const AnalysisConfig& config) {
    std::vector<std::unique_ptr<DetectionRule>> rules;

    for (const auto& ruleConfig : config.rules) {
        if (!ruleConfig.enabled) {
            continue;
        }

        if (ruleConfig.id == "REPEATED_AUTH_FAILURE") {
            rules.push_back(std::make_unique<RepeatedFailureRule>(
                ruleConfig.id, ruleConfig.windowSeconds, ruleConfig.threshold, ruleConfig.severity,
                ruleConfig.score));
        } else if (ruleConfig.id == "SUCCESS_AFTER_FAILURES") {
            rules.push_back(std::make_unique<SuccessAfterFailuresRule>(
                ruleConfig.id, ruleConfig.windowSeconds, ruleConfig.threshold, ruleConfig.severity,
                ruleConfig.score));
        } else if (ruleConfig.id == "MULTI_ACCOUNT_PROBE") {
            rules.push_back(std::make_unique<MultiAccountProbeRule>(
                ruleConfig.id, ruleConfig.windowSeconds, ruleConfig.threshold, ruleConfig.severity,
                ruleConfig.score));
        } else {
            throw ConfigError("unrecognized rule id in config: " + ruleConfig.id);
        }
    }

    return rules;
}

}  // namespace sentinel
