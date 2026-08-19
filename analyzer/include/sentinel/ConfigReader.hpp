/**
 * @file ConfigReader.hpp
 * @brief Loads and validates config/rules.json into a strongly-typed structure.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. Implementation in src/ConfigReader.cpp is stubbed.
 */

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "sentinel/IncidentResult.hpp"

namespace sentinel {

/// Strongly-typed mirror of one entry in config/rules.json's "rules" array.
struct RuleConfig {
    std::string id;
    std::string description;
    std::chrono::seconds windowSeconds;
    int threshold;
    Severity severity;
    int score;
    bool enabled;
};

/// Strongly-typed mirror of the full config/rules.json document.
struct AnalysisConfig {
    std::chrono::seconds analysisWindowSeconds;
    std::vector<RuleConfig> rules;
};

/**
 * @class ConfigReader
 * @brief Stateless loader for the rules configuration file.
 */
class ConfigReader {
public:
    ConfigReader() = delete;

    /**
     * @brief Load and validate config/rules.json (or an equivalent path).
     * @throws FileError if the file cannot be opened.
     * @throws ParseError if the file is not valid JSON.
     * @throws ConfigError if required fields are missing or values are invalid (e.g. unknown
     *         severity string, non-positive threshold/window).
     */
    [[nodiscard]] static AnalysisConfig readFromFile(const std::string& path);
};

}  // namespace sentinel
