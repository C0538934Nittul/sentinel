/**
 * @file EventBuilder.hpp
 * @brief Fluent builder for constructing SecurityEvent test fixtures with relative time offsets.
 * @component analyzer/tests/support
 * @status Complete. Test infrastructure, not assessed logic -- this file constructs inputs for
 *         the rule tests you write; it does not implement or presuppose any rule's behavior.
 */

#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>

#include <nlohmann/json.hpp>

#include "sentinel/SecurityEvent.hpp"

namespace sentinel::test {

/**
 * @brief Fixed base instant every test fixture builds relative offsets from.
 *
 * Deliberately not derived from wall-clock time (no std::chrono::system_clock::now() anywhere
 * in this file) -- tests must produce identical results regardless of when or in what timezone
 * they run. 2026-01-01T00:00:00.000Z has no significance beyond being a stable, memorable
 * anchor.
 */
inline std::chrono::system_clock::time_point testBaseTime() {
    using namespace std::chrono;
    return sys_days{year{2026} / January / 1};
}

/**
 * @brief Format a time_point as the ISO 8601 UTC string SecurityEvent::fromJson expects.
 *
 * Independent, test-only reimplementation (SecurityEvent.cpp's equivalent parsing logic is
 * assessed code and this file must not depend on it) -- if you rewrite timestamp handling, this
 * function's output must still match whatever format your new fromJson accepts.
 */
inline std::string formatIso8601Utc(std::chrono::system_clock::time_point tp) {
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

/**
 * @class EventBuilder
 * @brief Fluent builder for a single SecurityEvent test fixture.
 *
 * All timestamps are offsets from testBaseTime(), specified via at(). Builds through
 * SecurityEvent::fromJson (the only way to construct a SecurityEvent), so a builder-produced
 * event is validated exactly the same way a real ingested event would be.
 *
 * Usage:
 *   auto event = EventBuilder("evt-1")
 *                    .at(std::chrono::seconds(30))
 *                    .failure()
 *                    .from("203.0.113.1")
 *                    .account("jdoe")
 *                    .host("workstation-01")
 *                    .build();
 */
class EventBuilder {
public:
    explicit EventBuilder(std::string eventId) : eventId_(std::move(eventId)) {}

    EventBuilder& at(std::chrono::milliseconds offset) {
        offset_ = offset;
        return *this;
    }

    EventBuilder& eventType(EventType type) {
        eventType_ = type;
        return *this;
    }

    EventBuilder& failure() { return eventType(EventType::AuthFailure); }
    EventBuilder& success() { return eventType(EventType::AuthSuccess); }

    EventBuilder& from(std::string sourceIp) {
        sourceIp_ = std::move(sourceIp);
        return *this;
    }

    EventBuilder& account(std::string account) {
        account_ = std::move(account);
        return *this;
    }

    EventBuilder& noAccount() {
        account_.reset();
        return *this;
    }

    EventBuilder& host(std::string host) {
        host_ = std::move(host);
        return *this;
    }

    [[nodiscard]] SecurityEvent build() const {
        nlohmann::json json{
            {"eventId", eventId_},
            {"timestamp", formatIso8601Utc(testBaseTime() + offset_)},
            {"eventType", toString(eventType_)},
            {"sourceIp", sourceIp_},
            {"host", host_},
        };
        if (account_) {
            json["account"] = *account_;
        }
        return SecurityEvent::fromJson(json);
    }

private:
    std::string eventId_;
    std::chrono::milliseconds offset_{0};
    EventType eventType_ = EventType::AuthFailure;
    std::string sourceIp_ = "203.0.113.1";
    std::optional<std::string> account_;
    std::string host_ = "test-host";
};

}  // namespace sentinel::test
