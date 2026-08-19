/**
 * @file test_SecurityEvent.cpp
 * @brief Test checklist for SecurityEvent construction, validation, and JSON parsing.
 * @component analyzer/tests
 * @status Empty TEST_CASE bodies -- fill each in as you implement SecurityEvent::fromJson.
 */

#include <catch2/catch_test_macros.hpp>

#include "sentinel/SecurityEvent.hpp"

TEST_CASE("fromJson parses a fully valid AUTH_FAILURE event", "[SecurityEvent]") {}

TEST_CASE("fromJson parses an event with no account field", "[SecurityEvent]") {}

TEST_CASE("fromJson parses an event with an empty details object", "[SecurityEvent]") {}

TEST_CASE("fromJson throws ValidationError when eventId is missing", "[SecurityEvent]") {}

TEST_CASE("fromJson throws ValidationError when timestamp is missing", "[SecurityEvent]") {}

TEST_CASE("fromJson throws ValidationError when timestamp is unparseable", "[SecurityEvent]") {}

TEST_CASE("fromJson throws ValidationError when eventType is unknown", "[SecurityEvent]") {}

TEST_CASE("fromJson throws ValidationError when sourceIp is not a string", "[SecurityEvent]") {}

TEST_CASE("fromJson throws ValidationError when host is missing", "[SecurityEvent]") {}

TEST_CASE("toString(EventType) round-trips with eventTypeFromString for every enumerator",
          "[SecurityEvent]") {}

TEST_CASE("SecurityEvent has no public mutators (compile-time / API shape check)",
          "[SecurityEvent]") {}
