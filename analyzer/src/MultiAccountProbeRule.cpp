/**
 * @file MultiAccountProbeRule.cpp
 * @brief Implementation of MultiAccountProbeRule.
 * @component analyzer (sentinel-core)
 * @status Constructor/accessors implemented. evaluate() is STUBBED -- ASSESSED LOGIC.
 */

#include "sentinel/MultiAccountProbeRule.hpp"

namespace sentinel {

MultiAccountProbeRule::MultiAccountProbeRule(std::string id,
                                              std::chrono::seconds window,
                                              int threshold,
                                              Severity severity,
                                              int score)
    : id_(std::move(id)), window_(window), threshold_(threshold), severity_(severity), score_(score) {}

std::vector<IncidentResult> MultiAccountProbeRule::evaluate(
    const std::vector<SecurityEvent>& events) const {
    // TODO(student): group AUTH_FAILURE events by sourceIp, track the set of distinct accounts
    // targeted within a sliding window_, and emit an IncidentResult when the distinct-account
    // count reaches threshold_.
    (void)events;
    // window_/threshold_ are read here only to silence -Wunused-private-field until evaluate()
    // is implemented -- they're genuinely consumed by the assessed logic once it exists.
    (void)window_;
    (void)threshold_;
    throw std::logic_error("MultiAccountProbeRule::evaluate not implemented");
}

const std::string& MultiAccountProbeRule::id() const noexcept { return id_; }

Severity MultiAccountProbeRule::severity() const noexcept { return severity_; }

int MultiAccountProbeRule::score() const noexcept { return score_; }

}  // namespace sentinel
