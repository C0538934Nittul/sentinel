/**
 * @file IncidentResult.cpp
 * @brief Implementation of IncidentResult.
 * @component analyzer (sentinel-core)
 * @status Constructor and accessors implemented (trivial). toJson() is stubbed -- TODO(student).
 */

#include "sentinel/IncidentResult.hpp"

namespace sentinel {

std::string toString(Severity severity) {
    switch (severity) {
        case Severity::Low:
            return "LOW";
        case Severity::Medium:
            return "MEDIUM";
        case Severity::High:
            return "HIGH";
    }
    return "UNKNOWN";
}

IncidentResult::IncidentResult(std::string ruleId,
                                Severity severity,
                                int riskScore,
                                std::chrono::system_clock::time_point detectedAt,
                                std::vector<std::string> supportingEventIds,
                                std::string summary)
    : ruleId_(std::move(ruleId)),
      severity_(severity),
      riskScore_(riskScore),
      detectedAt_(detectedAt),
      supportingEventIds_(std::move(supportingEventIds)),
      summary_(std::move(summary)) {}

const std::string& IncidentResult::ruleId() const noexcept { return ruleId_; }

Severity IncidentResult::severity() const noexcept { return severity_; }

int IncidentResult::riskScore() const noexcept { return riskScore_; }

std::chrono::system_clock::time_point IncidentResult::detectedAt() const noexcept {
    return detectedAt_;
}

const std::vector<std::string>& IncidentResult::supportingEventIds() const noexcept {
    return supportingEventIds_;
}

const std::string& IncidentResult::summary() const noexcept { return summary_; }

nlohmann::json IncidentResult::toJson() const {
    // TODO(student): serialize to the incident-result shape in docs/data-contract.md,
    // formatting detectedAt_ as an ISO 8601 UTC string with millisecond precision and
    // severity_ via toString(Severity). Omit incidentId/status -- the API assigns those.
    throw std::logic_error("IncidentResult::toJson not implemented");
}

}  // namespace sentinel
