/**
 * @file SecurityEvent.cpp
 * @brief Implementation of SecurityEvent construction, validation, and JSON parsing.
 * @component analyzer (sentinel-core)
 * @status First-draft implementation (Step 3, Phase 5). Not the final assessed version --
 *         review and rewrite as needed.
 */

#include "sentinel/SecurityEvent.hpp"

#include <array>
#include <ctime>
#include <regex>
#include <utility>

#include "sentinel/SentinelException.hpp"

namespace sentinel {

namespace {

// Order matches the EventType enum declaration.
constexpr std::array<std::pair<EventType, const char*>, 5> kEventTypeNames{{
    {EventType::AuthSuccess, "AUTH_SUCCESS"},
    {EventType::AuthFailure, "AUTH_FAILURE"},
    {EventType::FileAccess, "FILE_ACCESS"},
    {EventType::ProcessStart, "PROCESS_START"},
    {EventType::NetworkConnection, "NETWORK_CONNECTION"},
}};

// Matches docs/data-contract.md: IPv4 dotted-quad format check only (no octet-range or
// reachability validation) -- see docs/ASSUMPTIONS.md.
const std::regex kSourceIpPattern(R"(^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$)");

// Strict ISO 8601 UTC, millisecond precision: YYYY-MM-DDTHH:MM:SS.mmmZ
const std::regex kTimestampPattern(
    R"(^(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})\.(\d{3})Z$)");

std::chrono::system_clock::time_point parseTimestamp(const std::string& value) {
    std::smatch match;
    if (!std::regex_match(value, match, kTimestampPattern)) {
        throw ValidationError("timestamp is not a valid ISO 8601 UTC timestamp: " + value);
    }

    const int year = std::stoi(match[1]);
    const unsigned month = static_cast<unsigned>(std::stoi(match[2]));
    const unsigned day = static_cast<unsigned>(std::stoi(match[3]));
    const int hour = std::stoi(match[4]);
    const int minute = std::stoi(match[5]);
    const int second = std::stoi(match[6]);
    const int millis = std::stoi(match[7]);

    const std::chrono::year_month_day ymd{std::chrono::year{year}, std::chrono::month{month},
                                           std::chrono::day{day}};
    if (!ymd.ok()) {
        throw ValidationError("timestamp has an invalid calendar date: " + value);
    }
    if (hour > 23 || minute > 59 || second > 59) {
        throw ValidationError("timestamp has an out-of-range time component: " + value);
    }

    const std::chrono::sys_days days{ymd};
    auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(days);
    tp += std::chrono::hours(hour) + std::chrono::minutes(minute) + std::chrono::seconds(second) +
          std::chrono::milliseconds(millis);
    return tp;
}

}  // namespace

std::string toString(EventType type) {
    for (const auto& [value, name] : kEventTypeNames) {
        if (value == type) {
            return name;
        }
    }
    // Unreachable for any valid EventType enumerator; a new enumerator without a matching
    // table entry is a programming error, not a runtime/input error.
    throw std::logic_error("toString(EventType): unhandled EventType enumerator");
}

EventType eventTypeFromString(const std::string& value) {
    for (const auto& [type, name] : kEventTypeNames) {
        if (value == name) {
            return type;
        }
    }
    throw ValidationError("unknown eventType: " + value);
}

SecurityEvent SecurityEvent::fromJson(const nlohmann::json& json) {
    if (!json.is_object()) {
        throw ValidationError("event must be a JSON object");
    }

    if (!json.contains("eventId") || !json["eventId"].is_string() ||
        json["eventId"].get<std::string>().empty()) {
        throw ValidationError("event is missing a non-empty string \"eventId\"");
    }
    std::string eventId = json["eventId"].get<std::string>();

    if (!json.contains("timestamp") || !json["timestamp"].is_string()) {
        throw ValidationError("event \"" + eventId + "\" is missing a string \"timestamp\"");
    }
    std::chrono::system_clock::time_point timestamp;
    try {
        timestamp = parseTimestamp(json["timestamp"].get<std::string>());
    } catch (const ValidationError& ex) {
        throw ValidationError("event \"" + eventId + "\": " + ex.what());
    }

    if (!json.contains("eventType") || !json["eventType"].is_string()) {
        throw ValidationError("event \"" + eventId + "\" is missing a string \"eventType\"");
    }
    EventType eventType;
    try {
        eventType = eventTypeFromString(json["eventType"].get<std::string>());
    } catch (const ValidationError& ex) {
        throw ValidationError("event \"" + eventId + "\": " + ex.what());
    }

    if (!json.contains("sourceIp") || !json["sourceIp"].is_string()) {
        throw ValidationError("event \"" + eventId + "\" has a missing or non-string \"sourceIp\"");
    }
    std::string sourceIp = json["sourceIp"].get<std::string>();
    if (!std::regex_match(sourceIp, kSourceIpPattern)) {
        throw ValidationError("event \"" + eventId + "\" has a malformed sourceIp: " + sourceIp);
    }

    std::optional<std::string> account;
    if (json.contains("account")) {
        if (!json["account"].is_string()) {
            throw ValidationError("event \"" + eventId + "\" has a non-string \"account\"");
        }
        account = json["account"].get<std::string>();
    }

    if (!json.contains("host") || !json["host"].is_string() || json["host"].get<std::string>().empty()) {
        throw ValidationError("event \"" + eventId + "\" is missing a non-empty string \"host\"");
    }
    std::string host = json["host"].get<std::string>();

    nlohmann::json details = nlohmann::json::object();
    if (json.contains("details")) {
        if (!json["details"].is_object()) {
            throw ValidationError("event \"" + eventId + "\" has a non-object \"details\"");
        }
        details = json["details"];
    }

    return SecurityEvent(std::move(eventId), timestamp, eventType, std::move(sourceIp),
                          std::move(account), std::move(host), std::move(details));
}

SecurityEvent::SecurityEvent(std::string eventId,
                              std::chrono::system_clock::time_point timestamp,
                              EventType eventType,
                              std::string sourceIp,
                              std::optional<std::string> account,
                              std::string host,
                              nlohmann::json details)
    : eventId_(std::move(eventId)),
      timestamp_(timestamp),
      eventType_(eventType),
      sourceIp_(std::move(sourceIp)),
      account_(std::move(account)),
      host_(std::move(host)),
      details_(std::move(details)) {}

const std::string& SecurityEvent::eventId() const noexcept { return eventId_; }

std::chrono::system_clock::time_point SecurityEvent::timestamp() const noexcept {
    return timestamp_;
}

EventType SecurityEvent::eventType() const noexcept { return eventType_; }

const std::string& SecurityEvent::sourceIp() const noexcept { return sourceIp_; }

const std::optional<std::string>& SecurityEvent::account() const noexcept { return account_; }

const std::string& SecurityEvent::host() const noexcept { return host_; }

const nlohmann::json& SecurityEvent::details() const noexcept { return details_; }

bool SecurityEvent::operator<(const SecurityEvent& other) const noexcept {
    if (timestamp_ != other.timestamp_) {
        return timestamp_ < other.timestamp_;
    }
    return eventId_ < other.eventId_;
}

}  // namespace sentinel
