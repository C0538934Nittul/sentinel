/**
 * @file ConfigReader.cpp
 * @brief Implementation of ConfigReader.
 * @component analyzer (sentinel-core)
 * @status First-draft implementation (Step 3, Phase 5). Not the final assessed version --
 *         review and rewrite as needed.
 */

#include "sentinel/ConfigReader.hpp"

#include <fstream>

#include <nlohmann/json.hpp>

#include "sentinel/SentinelException.hpp"

namespace sentinel {

namespace {

Severity parseSeverity(const std::string& ruleId, const std::string& value) {
    if (value == "LOW") return Severity::Low;
    if (value == "MEDIUM") return Severity::Medium;
    if (value == "HIGH") return Severity::High;
    throw ConfigError("rule \"" + ruleId + "\" has an unknown severity: " + value);
}

RuleConfig parseRuleConfig(const nlohmann::json& entry) {
    if (!entry.is_object()) {
        throw ConfigError("each entry in \"rules\" must be a JSON object");
    }
    if (!entry.contains("id") || !entry["id"].is_string()) {
        throw ConfigError("a rule entry is missing a string \"id\"");
    }
    RuleConfig config;
    config.id = entry["id"].get<std::string>();

    config.description = entry.value("description", std::string{});

    if (!entry.contains("windowSeconds") || !entry["windowSeconds"].is_number_integer()) {
        throw ConfigError("rule \"" + config.id + "\" is missing an integer \"windowSeconds\"");
    }
    config.windowSeconds = std::chrono::seconds(entry["windowSeconds"].get<long long>());

    if (!entry.contains("threshold") || !entry["threshold"].is_number_integer()) {
        throw ConfigError("rule \"" + config.id + "\" is missing an integer \"threshold\"");
    }
    config.threshold = entry["threshold"].get<int>();
    if (config.threshold <= 0) {
        throw ConfigError("rule \"" + config.id + "\" has a non-positive threshold");
    }

    if (!entry.contains("severity") || !entry["severity"].is_string()) {
        throw ConfigError("rule \"" + config.id + "\" is missing a string \"severity\"");
    }
    config.severity = parseSeverity(config.id, entry["severity"].get<std::string>());

    if (!entry.contains("score") || !entry["score"].is_number_integer()) {
        throw ConfigError("rule \"" + config.id + "\" is missing an integer \"score\"");
    }
    config.score = entry["score"].get<int>();

    config.enabled = entry.value("enabled", true);

    return config;
}

}  // namespace

AnalysisConfig ConfigReader::readFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw FileError("could not open config file: " + path);
    }

    nlohmann::json parsed;
    try {
        file >> parsed;
    } catch (const nlohmann::json::parse_error& ex) {
        throw ParseError(std::string("failed to parse config JSON: ") + ex.what());
    }

    if (!parsed.is_object()) {
        throw ConfigError("rules config must be a JSON object");
    }
    if (!parsed.contains("analysisWindowSeconds") || !parsed["analysisWindowSeconds"].is_number_integer()) {
        throw ConfigError("rules config is missing an integer \"analysisWindowSeconds\"");
    }
    if (!parsed.contains("rules") || !parsed["rules"].is_array()) {
        throw ConfigError("rules config is missing a \"rules\" array");
    }

    AnalysisConfig config;
    config.analysisWindowSeconds = std::chrono::seconds(parsed["analysisWindowSeconds"].get<long long>());
    for (const auto& entry : parsed["rules"]) {
        config.rules.push_back(parseRuleConfig(entry));
    }

    return config;
}

}  // namespace sentinel
