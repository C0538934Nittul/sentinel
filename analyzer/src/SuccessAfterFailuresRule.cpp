/**
 * @file SuccessAfterFailuresRule.cpp
 * @brief Implementation of SuccessAfterFailuresRule.
 * @component analyzer (sentinel-core)
 * @status Constructor/accessors implemented. evaluate() is STUBBED -- ASSESSED LOGIC.
 */

#include "sentinel/SuccessAfterFailuresRule.hpp"

namespace sentinel {

SuccessAfterFailuresRule::SuccessAfterFailuresRule(std::string id,
                                                     std::chrono::seconds window,
                                                     int threshold,
                                                     Severity severity,
                                                     int score)
    : id_(std::move(id)), window_(window), threshold_(threshold), severity_(severity), score_(score) {}

std::vector<IncidentResult> SuccessAfterFailuresRule::evaluate(
    const std::vector<SecurityEvent>& events) const {
    // TODO(student): for each AUTH_SUCCESS event, look back within window_ for AUTH_FAILURE
    // events against the same account (fall back to sourceIp if account is absent). If the
    // count reaches threshold_, emit an IncidentResult referencing the success event plus the
    // qualifying failures as supporting events.
    (void)events;
    // window_/threshold_ are read here only to silence -Wunused-private-field until evaluate()
    // is implemented -- they're genuinely consumed by the assessed logic once it exists.
    (void)window_;
    (void)threshold_;
    throw std::logic_error("SuccessAfterFailuresRule::evaluate not implemented");
}

const std::string& SuccessAfterFailuresRule::id() const noexcept { return id_; }

Severity SuccessAfterFailuresRule::severity() const noexcept { return severity_; }

int SuccessAfterFailuresRule::score() const noexcept { return score_; }

}  // namespace sentinel
