/**
 * @file ThreatAnalyzer.hpp
 * @brief Owns a collection of detection rules and runs them against a set of events.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. analyze() in src/ThreatAnalyzer.cpp is stubbed --
 *         ASSESSED LOGIC (correlation across rules). addRule()/ownership plumbing is
 *         implemented since it is not the assessed part.
 */

#pragma once

#include <memory>
#include <vector>

#include "sentinel/DetectionRule.hpp"
#include "sentinel/IncidentResult.hpp"
#include "sentinel/SecurityEvent.hpp"

namespace sentinel {

/**
 * @class ThreatAnalyzer
 * @brief Runs every registered DetectionRule against a batch of events and aggregates results.
 *
 * ## Ownership semantics
 *
 * ThreatAnalyzer holds its rules as `std::vector<std::unique_ptr<DetectionRule>>`.
 *
 * - **Who allocates:** the caller (RuleFactory, in this project) constructs each concrete rule
 *   with `std::make_unique<ConcreteRule>(...)` and passes ownership into addRule().
 * - **Who owns:** from the moment addRule() returns, ThreatAnalyzer exclusively owns the rule.
 *   No other part of the program holds a pointer to it. addRule() takes its parameter by value
 *   (`std::unique_ptr<DetectionRule>`), so the call site must `std::move` into it -- the type
 *   system makes the ownership transfer explicit at every call site, rather than relying on a
 *   comment.
 * - **When it's destroyed:** each rule is destroyed when the owning ThreatAnalyzer is destroyed
 *   (or when the vector is cleared/reassigned), via DetectionRule's virtual destructor --
 *   correct polymorphic cleanup even though the vector only stores base-class pointers.
 * - **Why unique_ptr instead of raw pointers:** a raw `DetectionRule*` would leave ownership
 *   ambiguous -- callers wouldn't know from the type alone whether they're responsible for
 *   `delete`ing it, inviting leaks or double-frees. unique_ptr encodes "exactly one owner,
 *   destroyed automatically" directly in the type.
 * - **Why unique_ptr instead of shared_ptr:** no other part of this program needs to share
 *   ownership of a rule -- rules are built once by RuleFactory and consumed solely by the
 *   analyzer that owns them. shared_ptr's reference counting would add overhead and imply a
 *   multi-owner model that doesn't reflect reality here, and would obscure (rather than
 *   document) the single-owner intent this class actually has.
 *
 * Because it holds move-only members, ThreatAnalyzer itself is move-only: copying is deleted
 * (copying would require deep-cloning each polymorphic rule, which DetectionRule does not
 * support) and moving is defaulted (transfers the vector of unique_ptrs cheaply).
 */
class ThreatAnalyzer {
public:
    ThreatAnalyzer() = default;

    ThreatAnalyzer(const ThreatAnalyzer&) = delete;
    ThreatAnalyzer& operator=(const ThreatAnalyzer&) = delete;
    ThreatAnalyzer(ThreatAnalyzer&&) = default;
    ThreatAnalyzer& operator=(ThreatAnalyzer&&) = default;
    ~ThreatAnalyzer() = default;

    /**
     * @brief Transfer ownership of a detection rule to this analyzer.
     * @param rule A non-null owning pointer to a concrete DetectionRule. Ownership moves into
     *             this ThreatAnalyzer; the caller's pointer is left null.
     */
    void addRule(std::unique_ptr<DetectionRule> rule);

    /// Number of rules currently registered.
    [[nodiscard]] std::size_t ruleCount() const noexcept;

    /**
     * @brief Run every registered (enabled) rule against `events` and aggregate the results.
     * @param events All events for this analysis run, chronologically ordered.
     * @return All incidents produced by all rules, in the order the rules were registered.
     */
    [[nodiscard]] std::vector<IncidentResult> analyze(
        const std::vector<SecurityEvent>& events) const;

private:
    std::vector<std::unique_ptr<DetectionRule>> rules_;
};

}  // namespace sentinel
