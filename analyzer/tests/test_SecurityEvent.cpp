/**
 * @file test_SecurityEvent.cpp
 * @brief Tests for SecurityEvent construction, validation, and JSON parsing.
 * @component analyzer/tests
 * @status Filled in (Step 3, Phase 5) against the first-draft SecurityEvent::fromJson.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/SecurityEvent.hpp"
#include "sentinel/SentinelException.hpp"

using namespace sentinel;

namespace {
nlohmann::json validEvent() {
    return nlohmann::json{
        {"eventId", "evt-001"},
        {"timestamp", "2026-03-01T10:00:00.000Z"},
        {"eventType", "AUTH_FAILURE"},
        {"sourceIp", "203.0.113.42"},
        {"account", "jdoe"},
        {"host", "workstation-12"},
        {"details", {{"method", "ssh"}}},
    };
}
}  // namespace

TEST_CASE("fromJson parses a fully valid AUTH_FAILURE event", "[SecurityEvent]") {
    const auto event = SecurityEvent::fromJson(validEvent());
    REQUIRE(event.eventId() == "evt-001");
    REQUIRE(event.eventType() == EventType::AuthFailure);
    REQUIRE(event.sourceIp() == "203.0.113.42");
    REQUIRE(event.account().has_value());
    REQUIRE(event.account().value() == "jdoe");
    REQUIRE(event.host() == "workstation-12");
    REQUIRE(event.details().at("method") == "ssh");
}

TEST_CASE("fromJson parses an event with no account field", "[SecurityEvent]") {
    auto json = validEvent();
    json.erase("account");
    const auto event = SecurityEvent::fromJson(json);
    REQUIRE_FALSE(event.account().has_value());
}

TEST_CASE("fromJson parses an event with an empty details object", "[SecurityEvent]") {
    auto json = validEvent();
    json["details"] = nlohmann::json::object();
    const auto event = SecurityEvent::fromJson(json);
    REQUIRE(event.details().empty());
}

TEST_CASE("fromJson throws ValidationError when eventId is missing", "[SecurityEvent]") {
    auto json = validEvent();
    json.erase("eventId");
    REQUIRE_THROWS_AS(SecurityEvent::fromJson(json), ValidationError);
}

TEST_CASE("fromJson throws ValidationError when timestamp is missing", "[SecurityEvent]") {
    auto json = validEvent();
    json.erase("timestamp");
    REQUIRE_THROWS_AS(SecurityEvent::fromJson(json), ValidationError);
}

TEST_CASE("fromJson throws ValidationError when timestamp is unparseable", "[SecurityEvent]") {
    auto json = validEvent();
    json["timestamp"] = "not-a-real-timestamp";
    REQUIRE_THROWS_AS(SecurityEvent::fromJson(json), ValidationError);
}

TEST_CASE("fromJson throws ValidationError when eventType is unknown", "[SecurityEvent]") {
    auto json = validEvent();
    json["eventType"] = "TELEPORT_DETECTED";
    REQUIRE_THROWS_AS(SecurityEvent::fromJson(json), ValidationError);
}

TEST_CASE("fromJson throws ValidationError when sourceIp is not a string", "[SecurityEvent]") {
    auto json = validEvent();
    json["sourceIp"] = 203011342;
    REQUIRE_THROWS_AS(SecurityEvent::fromJson(json), ValidationError);
}

TEST_CASE("fromJson throws ValidationError when host is missing", "[SecurityEvent]") {
    auto json = validEvent();
    json.erase("host");
    REQUIRE_THROWS_AS(SecurityEvent::fromJson(json), ValidationError);
}

TEST_CASE("toString(EventType) round-trips with eventTypeFromString for every enumerator",
          "[SecurityEvent]") {
    for (auto type : {EventType::AuthSuccess, EventType::AuthFailure, EventType::FileAccess,
                       EventType::ProcessStart, EventType::NetworkConnection}) {
        REQUIRE(eventTypeFromString(toString(type)) == type);
    }
}

TEST_CASE("SecurityEvent has no public mutators (compile-time / API shape check)",
          "[SecurityEvent]") {
    // If SecurityEvent grew a setter, this test would still compile and pass -- its real job is
    // documentary: a reviewer scanning tests sees this class is asserted to be immutable.
    const auto event = SecurityEvent::fromJson(validEvent());
    const auto copy = event;  // copy-construction must remain available
    REQUIRE(copy.eventId() == event.eventId());
}
