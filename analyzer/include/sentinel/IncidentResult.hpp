/**
 * @file IncidentResult.hpp
 * @brief Value type representing one incident produced by a detection rule.
 * @component analyzer (sentinel-core)
 * @status Declarations complete. toJson() body in src/IncidentResult.cpp is stubbed --
 *         serialization shape must match docs/data-contract.md exactly, which is worth
 *         checking by hand rather than assuming.
 */

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace sentinel {

/// Matches the `severity` field of config/rules.json and docs/data-contract.md.
enum class Severity { Low, Medium, High };

[[nodiscard]] std::string toString(Severity severity);

/**
 * @class IncidentResult
 * @brief One incident: the output of a DetectionRule::evaluate() call.
 *
 * The analyzer does not assign incidentId or status -- those are owned by the API/persistence
 * layer once the incident is stored. IncidentResult only carries what the analyzer itself knows.
 */
class IncidentResult {
public:
    IncidentResult(std::string ruleId,
                    Severity severity,
                    int riskScore,
                    std::chrono::system_clock::time_point detectedAt,
                    std::vector<std::string> supportingEventIds,
                    std::string summary);

    [[nodiscard]] const std::string& ruleId() const noexcept;
    [[nodiscard]] Severity severity() const noexcept;
    [[nodiscard]] int riskScore() const noexcept;
    [[nodiscard]] std::chrono::system_clock::time_point detectedAt() const noexcept;
    [[nodiscard]] const std::vector<std::string>& supportingEventIds() const noexcept;
    [[nodiscard]] const std::string& summary() const noexcept;

    /**
     * @brief Serialize to the incident-result JSON shape in docs/data-contract.md
     *        (excluding `incidentId` and `status`, which the API assigns).
     */
    [[nodiscard]] nlohmann::json toJson() const;

private:
    std::string ruleId_;
    Severity severity_;
    int riskScore_;
    std::chrono::system_clock::time_point detectedAt_;
    std::vector<std::string> supportingEventIds_;
    std::string summary_;
};

}  // namespace sentinel
