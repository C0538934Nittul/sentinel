/**
 * @file MultiAccountProbeRule.hpp
 * @brief Detects one source probing multiple distinct accounts with failed auth attempts.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. evaluate() in src/MultiAccountProbeRule.cpp is stubbed --
 *         ASSESSED LOGIC. See config/rules.json for MULTI_ACCOUNT_PROBE's configuration.
 */

#pragma once

#include <chrono>
#include <string>

#include "sentinel/DetectionRule.hpp"

namespace sentinel {

/**
 * @class MultiAccountProbeRule
 * @brief Fires when a single source IP produces AUTH_FAILURE events against at least
 *        `threshold_` distinct accounts within `window_`.
 */
class MultiAccountProbeRule final : public DetectionRule {
public:
    MultiAccountProbeRule(std::string id,
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
