/**
 * @file RepeatedFailureRule.hpp
 * @brief Detects a high volume of AUTH_FAILURE events from one source within a window.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. evaluate() in src/RepeatedFailureRule.cpp is stubbed --
 *         ASSESSED LOGIC, implement this yourself. See config/rules.json for
 *         REPEATED_AUTH_FAILURE's configured threshold/window/severity/score.
 */

#pragma once

#include <chrono>
#include <string>

#include "sentinel/DetectionRule.hpp"

namespace sentinel {

/**
 * @class RepeatedFailureRule
 * @brief Fires when a single source IP produces at least `threshold_` AUTH_FAILURE events
 *        within `window_`.
 */
class RepeatedFailureRule final : public DetectionRule {
public:
    /**
     * @param id Rule identifier, injected from config rather than hard-coded.
     * @param window Sliding time window within which failures are counted.
     * @param threshold Minimum failure count within the window to produce an incident.
     * @param severity Severity assigned to produced incidents.
     * @param score Risk score assigned to produced incidents.
     */
    RepeatedFailureRule(std::string id,
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
