/**
 * @file RuleFactory.hpp
 * @brief Builds concrete DetectionRule instances from parsed configuration.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. Implementation in src/RuleFactory.cpp is stubbed.
 */

#pragma once

#include <memory>
#include <vector>

#include "sentinel/ConfigReader.hpp"
#include "sentinel/DetectionRule.hpp"

namespace sentinel {

/**
 * @class RuleFactory
 * @brief Translates an AnalysisConfig into a set of owned, ready-to-run DetectionRule
 *        instances (the Factory pattern, isolating "which rule id maps to which concrete
 *        class" from ThreatAnalyzer and from main.cpp).
 */
class RuleFactory {
public:
    RuleFactory() = delete;

    /**
     * @brief Construct one concrete DetectionRule per enabled entry in `config.rules`.
     * @param config Parsed rules configuration (see ConfigReader).
     * @return Newly allocated rules, ready to be moved into a ThreatAnalyzer via addRule().
     * @throws ConfigError if a rule id in the config does not map to a known concrete rule
     *         class.
     */
    [[nodiscard]] static std::vector<std::unique_ptr<DetectionRule>> buildRules(
        const AnalysisConfig& config);
};

}  // namespace sentinel
