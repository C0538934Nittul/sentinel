/**
 * @file SecurityEvent.cpp
 * @brief Implementation of SecurityEvent construction, validation, and JSON parsing.
 * @component analyzer (sentinel-core)
 * @status STUBBED. This is assessed logic -- fill in validation and parsing per the contract
 *         in docs/data-contract.md and the declarations in include/sentinel/SecurityEvent.hpp.
 */

#include "sentinel/SecurityEvent.hpp"

#include "sentinel/SentinelException.hpp"

namespace sentinel {

std::string toString(EventType type) {
    // TODO(student): map each EventType enumerator to its wire string
    // ("AUTH_SUCCESS", "AUTH_FAILURE", "FILE_ACCESS", "PROCESS_START", "NETWORK_CONNECTION").
    (void)type;
    throw std::logic_error("toString(EventType) not implemented");
}

EventType eventTypeFromString(const std::string& value) {
    // TODO(student): inverse of toString(). Throw ValidationError for unknown strings.
    (void)value;
    throw std::logic_error("eventTypeFromString(std::string) not implemented");
}

SecurityEvent SecurityEvent::fromJson(const nlohmann::json& json) {
    // TODO(student):
    //  - Validate presence and type of required fields: eventId, timestamp, eventType,
    //    sourceIp, host.
    //  - Parse `timestamp` (ISO 8601 UTC, millisecond precision) into a
    //    std::chrono::system_clock::time_point. Throw ValidationError on an unparseable string.
    //  - Validate `eventType` against the known enum values via eventTypeFromString().
    //  - Validate `sourceIp` looks like an IPv4 dotted-quad (format check is sufficient --
    //    no need to resolve or reach the address).
    //  - `account` is optional; `details` defaults to an empty object if absent.
    //  - Construct and return a SecurityEvent via the private constructor.
    (void)json;
    throw std::logic_error("SecurityEvent::fromJson not implemented");
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

}  // namespace sentinel
