/**
 * @file SecurityEvent.hpp
 * @brief Immutable, validated value type representing one security event.
 * @component analyzer (sentinel-core)
 * @status Declarations complete. Implementation in src/SecurityEvent.cpp is stubbed
 *         (TODO(student)) -- construction/validation/JSON parsing logic is assessed work.
 */

#pragma once

#include <chrono>
#include <optional>
#include <string>

#include <nlohmann/json.hpp>

namespace sentinel {

/**
 * @brief The category of a security event, matching docs/data-contract.md.
 */
enum class EventType {
    AuthSuccess,
    AuthFailure,
    FileAccess,
    ProcessStart,
    NetworkConnection
};

/**
 * @brief Convert an EventType to its wire string form (e.g. "AUTH_FAILURE").
 */
[[nodiscard]] std::string toString(EventType type);

/**
 * @brief Parse an EventType from its wire string form.
 * @throws ValidationError if the string does not match a known event type.
 */
[[nodiscard]] EventType eventTypeFromString(const std::string& value);

/**
 * @class SecurityEvent
 * @brief An immutable, already-validated security event.
 *
 * Instances can only be constructed through fromJson(), which validates all required fields
 * and throws on malformed input. Once constructed, a SecurityEvent cannot be mutated -- there
 * are no setters, only accessors. This makes SecurityEvent safe to pass by const-reference
 * throughout the analysis pipeline without defensive copying.
 */
class SecurityEvent {
public:
    /**
     * @brief Parse and validate a single event from a JSON object.
     * @param json A JSON object matching the security-event shape in docs/data-contract.md.
     * @return A fully validated SecurityEvent.
     * @throws ValidationError if a required field is missing, the wrong type, or otherwise
     *         invalid (e.g. unparseable timestamp, unknown eventType, malformed sourceIp).
     */
    [[nodiscard]] static SecurityEvent fromJson(const nlohmann::json& json);

    // Accessors only -- no setters. See class-level comment.
    [[nodiscard]] const std::string& eventId() const noexcept;
    [[nodiscard]] std::chrono::system_clock::time_point timestamp() const noexcept;
    [[nodiscard]] EventType eventType() const noexcept;
    [[nodiscard]] const std::string& sourceIp() const noexcept;
    [[nodiscard]] const std::optional<std::string>& account() const noexcept;
    [[nodiscard]] const std::string& host() const noexcept;
    [[nodiscard]] const nlohmann::json& details() const noexcept;

    /**
     * @brief Orders events by timestamp (ties broken by eventId for a stable, deterministic
     *        order). Used to sort events chronologically before rule evaluation.
     */
    [[nodiscard]] bool operator<(const SecurityEvent& other) const noexcept;

    SecurityEvent(const SecurityEvent&) = default;
    SecurityEvent(SecurityEvent&&) = default;
    SecurityEvent& operator=(const SecurityEvent&) = default;
    SecurityEvent& operator=(SecurityEvent&&) = default;
    ~SecurityEvent() = default;

private:
    // Private constructor -- only fromJson() may build a SecurityEvent, guaranteeing every
    // live instance has already passed validation.
    SecurityEvent(std::string eventId,
                  std::chrono::system_clock::time_point timestamp,
                  EventType eventType,
                  std::string sourceIp,
                  std::optional<std::string> account,
                  std::string host,
                  nlohmann::json details);

    std::string eventId_;
    std::chrono::system_clock::time_point timestamp_;
    EventType eventType_;
    std::string sourceIp_;
    std::optional<std::string> account_;
    std::string host_;
    nlohmann::json details_;
};

}  // namespace sentinel
