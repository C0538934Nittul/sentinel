/**
 * @file EventSeq.hpp
 * @brief Generators for runs of events (repeated failures, multi-account probes, trailing
 *        successes) built on top of EventBuilder.
 * @component analyzer/tests/support
 * @status Complete. Test infrastructure, not assessed logic -- these generators produce inputs
 *         with a documented, inspectable shape (count/spacing/accounts); they don't encode or
 *         presuppose what any DetectionRule should do with that input.
 */

#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <vector>

#include "EventBuilder.hpp"

namespace sentinel::test {

/// Options for failureSeries(). Defaults produce 10 AUTH_FAILURE events, 10s apart, from one
/// source against one account -- adjust whichever fields matter for the case under test.
struct FailureSeriesOptions {
    int count = 10;
    std::chrono::milliseconds spacing = std::chrono::seconds(10);
    std::chrono::milliseconds startOffset = std::chrono::seconds(0);
    std::string sourceIp = "203.0.113.1";
    std::string accountPrefix = "acct";
    /// 1 = every event uses the same account ("<accountPrefix>0"). >1 = accounts cycle
    /// round-robin through "<accountPrefix>0" .. "<accountPrefix>(distinctAccounts-1)" --
    /// useful for MULTI_ACCOUNT_PROBE fixtures where distinct-account count is what matters.
    int distinctAccounts = 1;
    std::string host = "test-host";
    std::string idPrefix = "evt";
};

/**
 * @brief Generate `options.count` AUTH_FAILURE events at `options.spacing` intervals starting
 *        at `options.startOffset`, from `options.sourceIp`.
 *
 * Events are always returned in chronological order (ascending timestamp) -- if you need to
 * test out-of-order input handling, shuffle the returned vector yourself (see
 * <algorithm>'s std::shuffle) so that intent stays visible at the call site.
 */
inline std::vector<SecurityEvent> failureSeries(const FailureSeriesOptions& options) {
    std::vector<SecurityEvent> events;
    events.reserve(static_cast<std::size_t>(options.count));
    for (int i = 0; i < options.count; ++i) {
        const int accountIndex = options.distinctAccounts > 1 ? i % options.distinctAccounts : 0;
        events.push_back(EventBuilder(options.idPrefix + "-" + std::to_string(i + 1))
                              .at(options.startOffset + options.spacing * i)
                              .failure()
                              .from(options.sourceIp)
                              .account(options.accountPrefix + std::to_string(accountIndex))
                              .host(options.host)
                              .build());
    }
    return events;
}

/**
 * @brief Build a trailing AUTH_SUCCESS event `delay` after the last event in `series` (or at
 *        testBaseTime() if `series` is empty).
 * @param series The events this success should follow -- used only to anchor the timestamp and
 *               default the sourceIp/account; not otherwise inspected.
 * @param delay How long after `series.back()`'s timestamp the success occurs. Pass a negative
 *              duration to place the success *before* the series (e.g. for a
 *              "success-before-failures should not fire" test case).
 * @param sourceIp Overrides the success event's source; defaults to `series.front()`'s source.
 * @param account Overrides the success event's account; defaults to `series.front()`'s account
 *                (if it has one).
 */
inline SecurityEvent trailingSuccess(const std::vector<SecurityEvent>& series,
                                      std::chrono::milliseconds delay,
                                      std::optional<std::string> sourceIp = std::nullopt,
                                      std::optional<std::string> account = std::nullopt,
                                      std::string eventId = "evt-success") {
    const auto anchor = series.empty() ? testBaseTime() : series.back().timestamp();
    const auto offsetFromBase =
        std::chrono::duration_cast<std::chrono::milliseconds>(anchor - testBaseTime()) + delay;

    EventBuilder builder(std::move(eventId));
    builder.at(offsetFromBase).success();

    if (sourceIp) {
        builder.from(*sourceIp);
    } else if (!series.empty()) {
        builder.from(series.front().sourceIp());
    }

    if (account) {
        builder.account(*account);
    } else if (!series.empty() && series.front().account().has_value()) {
        builder.account(*series.front().account());
    }

    return builder.build();
}

}  // namespace sentinel::test
