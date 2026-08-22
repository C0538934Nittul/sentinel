/**
 * @file test_support_harness.cpp
 * @brief Tests for the test support headers themselves (EventBuilder, EventSeq, Fixtures,
 *        Matchers) -- proves the harness works before you rely on it for rule tests.
 * @component analyzer/tests
 * @status Complete. Exercises only test infrastructure; constructs IncidentResult objects
 *         directly (not via any DetectionRule) so nothing here touches assessed logic.
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>

#include <filesystem>

#include "sentinel/IncidentResult.hpp"
#include "support/EventBuilder.hpp"
#include "support/EventSeq.hpp"
#include "support/Fixtures.hpp"
#include "support/Matchers.hpp"

using namespace sentinel;
using namespace sentinel::test;

TEST_CASE("EventBuilder produces a valid, parseable SecurityEvent", "[support]") {
    auto event = EventBuilder("evt-1")
                     .at(std::chrono::seconds(5))
                     .failure()
                     .from("203.0.113.9")
                     .account("jdoe")
                     .host("h1")
                     .build();
    REQUIRE(event.eventId() == "evt-1");
    REQUIRE(event.eventType() == EventType::AuthFailure);
    REQUIRE(event.sourceIp() == "203.0.113.9");
    REQUIRE(event.account().has_value());
    REQUIRE(event.account().value() == "jdoe");
    REQUIRE(event.host() == "h1");
}

TEST_CASE("EventBuilder omits account when noAccount()/unset", "[support]") {
    auto event = EventBuilder("evt-2").at(std::chrono::seconds(0)).success().build();
    REQUIRE_FALSE(event.account().has_value());
}

TEST_CASE("EventBuilder offsets are relative to a fixed base time, not wall-clock", "[support]") {
    auto e0 = EventBuilder("evt-a").at(std::chrono::seconds(0)).build();
    auto e1 = EventBuilder("evt-b").at(std::chrono::seconds(30)).build();
    REQUIRE(e0.timestamp() == testBaseTime());
    REQUIRE(e1.timestamp() == testBaseTime() + std::chrono::seconds(30));
}

TEST_CASE("failureSeries generates the requested count, spacing, and source", "[support]") {
    FailureSeriesOptions options;
    options.count = 5;
    options.spacing = std::chrono::seconds(10);
    options.sourceIp = "203.0.113.5";
    const auto events = failureSeries(options);

    REQUIRE(events.size() == 5);
    for (const auto& e : events) {
        REQUIRE(e.sourceIp() == "203.0.113.5");
        REQUIRE(e.eventType() == EventType::AuthFailure);
    }
    REQUIRE(events.front().timestamp() == testBaseTime());
    REQUIRE(events.back().timestamp() == testBaseTime() + std::chrono::seconds(40));
}

TEST_CASE("failureSeries with distinctAccounts cycles through accounts round-robin", "[support]") {
    FailureSeriesOptions options;
    options.count = 6;
    options.distinctAccounts = 3;
    const auto events = failureSeries(options);

    REQUIRE(events.at(0).account().value() == "acct0");
    REQUIRE(events.at(1).account().value() == "acct1");
    REQUIRE(events.at(2).account().value() == "acct2");
    REQUIRE(events.at(3).account().value() == "acct0");
    REQUIRE(events.at(4).account().value() == "acct1");
    REQUIRE(events.at(5).account().value() == "acct2");
}

TEST_CASE("failureSeries with distinctAccounts=1 uses the same account throughout", "[support]") {
    FailureSeriesOptions options;
    options.count = 4;
    options.distinctAccounts = 1;
    const auto events = failureSeries(options);
    for (const auto& e : events) {
        REQUIRE(e.account().value() == "acct0");
    }
}

TEST_CASE("trailingSuccess lands the given delay after the last event in the series", "[support]") {
    FailureSeriesOptions options;
    options.count = 3;
    options.spacing = std::chrono::seconds(5);
    const auto series = failureSeries(options);
    const auto success = trailingSuccess(series, std::chrono::seconds(60));

    REQUIRE(success.eventType() == EventType::AuthSuccess);
    REQUIRE(success.timestamp() == series.back().timestamp() + std::chrono::seconds(60));
    REQUIRE(success.sourceIp() == series.front().sourceIp());
    REQUIRE(success.account().value() == series.front().account().value());
}

TEST_CASE("trailingSuccess accepts a negative delay to place the success before the series",
          "[support]") {
    FailureSeriesOptions options;
    options.count = 2;
    options.startOffset = std::chrono::seconds(100);
    options.spacing = std::chrono::seconds(5);
    const auto series = failureSeries(options);
    const auto success = trailingSuccess(series, std::chrono::seconds(-200));
    REQUIRE(success.timestamp() < series.front().timestamp());
}

TEST_CASE("sampleDataPath resolves to an existing file under sample-data/", "[support]") {
    const auto path = sampleDataPath("normal-events.json");
    REQUIRE(std::filesystem::exists(path));
}

TEST_CASE("FiresRule/FiresNothing/HasSeverity/HasEvidenceCount match constructed IncidentResults",
          "[support]") {
    const std::vector<IncidentResult> none;
    REQUIRE_THAT(none, FiresNothing());
    REQUIRE_THAT(none, !FiresRule("REPEATED_AUTH_FAILURE"));

    std::vector<IncidentResult> one;
    one.emplace_back("REPEATED_AUTH_FAILURE", Severity::High, 80, testBaseTime(),
                      std::vector<std::string>{"e1", "e2", "e3"}, "test summary");

    REQUIRE_THAT(one, FiresRule("REPEATED_AUTH_FAILURE"));
    REQUIRE_THAT(one, !FiresRule("MULTI_ACCOUNT_PROBE"));
    REQUIRE_THAT(one, HasSeverity("HIGH"));
    REQUIRE_THAT(one, !FiresNothing());
    REQUIRE_THAT(one.at(0), HasEvidenceCount(3));
}
