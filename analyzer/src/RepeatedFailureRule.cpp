/**
 * @file RepeatedFailureRule.cpp
 * @brief Implementation of RepeatedFailureRule.
 * @component analyzer (sentinel-core)
 * @status Constructor/accessors implemented. evaluate() is STUBBED -- ASSESSED LOGIC.
 */

#include "sentinel/RepeatedFailureRule.hpp"

namespace sentinel {

RepeatedFailureRule::RepeatedFailureRule(std::string id,
                                          std::chrono::seconds window,
                                          int threshold,
                                          Severity severity,
                                          int score)
    : id_(std::move(id)), window_(window), threshold_(threshold), severity_(severity), score_(score) {}

std::vector<IncidentResult> RepeatedFailureRule::evaluate(
    const std::vector<SecurityEvent>& events) const {
    // TODO(student): group AUTH_FAILURE events by sourceIp, use a sliding window over each
    // group's timestamps, and emit one IncidentResult per source IP/window that reaches
    // threshold_ within window_. events is chronologically ordered on input.
    (void)events;
    // window_/threshold_ are read here only to silence -Wunused-private-field until evaluate()
    // is implemented -- they're genuinely consumed by the assessed logic once it exists.
    (void)window_;
    (void)threshold_;
    throw std::logic_error("RepeatedFailureRule::evaluate not implemented");
}

const std::string& RepeatedFailureRule::id() const noexcept { return id_; }

Severity RepeatedFailureRule::severity() const noexcept { return severity_; }

int RepeatedFailureRule::score() const noexcept { return score_; }

}  // namespace sentinel
