/**
 * @file IncidentResult.cpp
 * @brief Implementation of IncidentResult.
 * @component analyzer (sentinel-core)
 * @status Constructor and accessors implemented (trivial). toJson() first-draft implemented
 *         (Step 3, Phase 5) -- not the final assessed version, review and rewrite as needed.
 */

#include "sentinel/IncidentResult.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace sentinel {

namespace {

// Mirrors Logger::timestampNow's approach -- manual formatting avoids relying on libc++'s
// <chrono> streaming/format extensions, whose availability varies by platform/toolchain
// version. Produces the same "YYYY-MM-DDTHH:MM:SS.mmmZ" shape SecurityEvent parses.
std::string formatIso8601Utc(std::chrono::system_clock::time_point tp) {
    using namespace std::chrono;
    const auto ms = duration_cast<milliseconds>(tp.time_since_epoch()) % 1000;
    const std::time_t t = system_clock::to_time_t(tp);
    std::tm utcTm{};
#if defined(_WIN32)
    gmtime_s(&utcTm, &t);
#else
    gmtime_r(&t, &utcTm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&utcTm, "%Y-%m-%dT%H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count() << 'Z';
    return oss.str();
}

}  // namespace

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
    // Matches docs/data-contract.md's incident-result shape, excluding incidentId/status
    // (assigned by the API at persistence time).
    return nlohmann::json{
        {"ruleId", ruleId_},
        {"severity", toString(severity_)},
        {"riskScore", riskScore_},
        {"detectedAt", formatIso8601Utc(detectedAt_)},
        {"supportingEventIds", supportingEventIds_},
        {"summary", summary_},
    };
}

}  // namespace sentinel
