/**
 * @file SuccessAfterFailuresRule.hpp
 * @brief Detects an AUTH_SUCCESS immediately following repeated AUTH_FAILURE events.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. evaluate() in src/SuccessAfterFailuresRule.cpp is stubbed --
 *         ASSESSED LOGIC. See config/rules.json for SUCCESS_AFTER_FAILURES's configuration.
 */

#pragma once

#include <chrono>
#include <string>

#include "sentinel/DetectionRule.hpp"

namespace sentinel {

/**
 * @class SuccessAfterFailuresRule
 * @brief Fires when an AUTH_SUCCESS follows at least `threshold_` AUTH_FAILURE events against
 *        the same account (or, absent an account, the same source IP) within `window_`.
 */
class SuccessAfterFailuresRule final : public DetectionRule {
public:
    SuccessAfterFailuresRule(std::string id,
                              std::chrono::seconds window,
                              int threshold,
                              Severity severity,
                              int score);

    [[nodiscard]] std::vector<IncidentResult> evaluate(
        const std::vector<SecurityEvent>& events) const override;

    [[nodiscard]] const std::string& id() const noexcept override;
    [[nodiscard]] Severity severity() const noexcept override;
    [[nodiscard]] int score() const noexcept override;

private:
    std::string id_;
    std::chrono::seconds window_;
    int threshold_;
    Severity severity_;
    int score_;
};

}  // namespace sentinel
