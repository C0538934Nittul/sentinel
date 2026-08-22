/**
 * @file Matchers.hpp
 * @brief Catch2 matchers for asserting on std::vector<IncidentResult> / IncidentResult so test
 *        intent reads directly: FiresRule(...), FiresNothing(), HasSeverity(...),
 *        HasEvidenceCount(...).
 * @component analyzer/tests/support
 * @status Complete. Test infrastructure, not assessed logic -- these matchers only inspect
 *         IncidentResult objects you already have; they don't produce or presuppose them.
 */

#pragma once

#include <string>
#include <vector>

#include <catch2/matchers/catch_matchers_templated.hpp>

#include "sentinel/IncidentResult.hpp"

namespace sentinel::test {

/// REQUIRE_THAT(incidents, FiresRule("REPEATED_AUTH_FAILURE")) -- true if any incident in the
/// vector has that ruleId. Combine with `!FiresRule(...)` to assert a rule did NOT fire.
class FiresRuleMatcher : public Catch::Matchers::MatcherGenericBase {
public:
    explicit FiresRuleMatcher(std::string ruleId) : ruleId_(std::move(ruleId)) {}

    bool match(const std::vector<IncidentResult>& incidents) const {
        for (const auto& incident : incidents) {
            if (incident.ruleId() == ruleId_) {
                return true;
            }
        }
        return false;
    }

    std::string describe() const override { return "fires rule \"" + ruleId_ + "\""; }

private:
    std::string ruleId_;
};

inline FiresRuleMatcher FiresRule(std::string ruleId) { return FiresRuleMatcher(std::move(ruleId)); }

/// REQUIRE_THAT(incidents, FiresNothing()) -- true only for an empty result vector. More
/// readable at the call site than REQUIRE(incidents.empty()) when read alongside FiresRule(...).
class FiresNothingMatcher : public Catch::Matchers::MatcherGenericBase {
public:
    bool match(const std::vector<IncidentResult>& incidents) const { return incidents.empty(); }
    std::string describe() const override { return "fires nothing (produces zero incidents)"; }
};

inline FiresNothingMatcher FiresNothing() { return FiresNothingMatcher(); }

/// REQUIRE_THAT(incidents, HasSeverity("HIGH")) -- true if every incident in the vector has the
/// given severity (vacuously false for an empty vector -- pair with FiresRule, not FiresNothing).
class HasSeverityMatcher : public Catch::Matchers::MatcherGenericBase {
public:
    explicit HasSeverityMatcher(std::string severity) : severity_(std::move(severity)) {}

    bool match(const std::vector<IncidentResult>& incidents) const {
        if (incidents.empty()) {
            return false;
        }
        for (const auto& incident : incidents) {
            // Qualified call: MatcherGenericBase declares its own 0-arg toString() member,
            // which would otherwise shadow sentinel::toString(Severity) via member lookup.
            if (sentinel::toString(incident.severity()) != severity_) {
                return false;
            }
        }
        return true;
    }

    std::string describe() const override { return "has severity \"" + severity_ + "\""; }

private:
    std::string severity_;
};

inline HasSeverityMatcher HasSeverity(std::string severity) {
    return HasSeverityMatcher(std::move(severity));
}

/// REQUIRE_THAT(incidents.at(0), HasEvidenceCount(12)) -- true if that single incident's
/// supportingEventIds has exactly `count` entries. Operates on one IncidentResult, not a
/// vector -- index into the result vector at the call site.
class HasEvidenceCountMatcher : public Catch::Matchers::MatcherGenericBase {
public:
    explicit HasEvidenceCountMatcher(std::size_t count) : count_(count) {}

    bool match(const IncidentResult& incident) const {
        return incident.supportingEventIds().size() == count_;
    }

    std::string describe() const override { return "has evidence count " + std::to_string(count_); }

private:
    std::size_t count_;
};

inline HasEvidenceCountMatcher HasEvidenceCount(std::size_t count) {
    return HasEvidenceCountMatcher(count);
}

}  // namespace sentinel::test
