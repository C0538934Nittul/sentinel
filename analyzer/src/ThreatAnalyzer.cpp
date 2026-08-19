/**
 * @file ThreatAnalyzer.cpp
 * @brief Implementation of ThreatAnalyzer.
 * @component analyzer (sentinel-core)
 * @status addRule()/ruleCount() implemented (ownership plumbing, not assessed).
 *         analyze() is STUBBED -- ASSESSED LOGIC.
 */

#include "sentinel/ThreatAnalyzer.hpp"

#include <utility>

namespace sentinel {

void ThreatAnalyzer::addRule(std::unique_ptr<DetectionRule> rule) {
    rules_.push_back(std::move(rule));
}

std::size_t ThreatAnalyzer::ruleCount() const noexcept { return rules_.size(); }

std::vector<IncidentResult> ThreatAnalyzer::analyze(
    const std::vector<SecurityEvent>& events) const {
    // TODO(student): invoke evaluate(events) on each registered rule and concatenate the
    // results. Decide (and document) whether rules run independently over the full event set
    // or whether some cross-rule correlation/deduplication happens here.
    (void)events;
    throw std::logic_error("ThreatAnalyzer::analyze not implemented");
}

}  // namespace sentinel
