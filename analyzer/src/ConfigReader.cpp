/**
 * @file ConfigReader.cpp
 * @brief Implementation of ConfigReader.
 * @component analyzer (sentinel-core)
 * @status STUBBED -- TODO(student).
 */

#include "sentinel/ConfigReader.hpp"

#include <fstream>

#include <nlohmann/json.hpp>

#include "sentinel/SentinelException.hpp"

namespace sentinel {

AnalysisConfig ConfigReader::readFromFile(const std::string& path) {
    // File-open check is real (infra, not assessed logic).
    std::ifstream file(path);
    if (!file.is_open()) {
        throw FileError("could not open config file: " + path);
    }

    // TODO(student): parse the file as JSON (ParseError on malformed JSON), then validate and
    // map each entry in "rules" to a RuleConfig -- including parsing the "severity" string into
    // the Severity enum and rejecting unknown values with ConfigError.
    //
    // Stubbed to a safe default (the top-level analysisWindowSeconds from config/rules.json,
    // zero rules) for now rather than throwing, so RuleFactory/ThreatAnalyzer are exercisable
    // end-to-end before this is implemented -- see docs/ASSUMPTIONS.md.
    return AnalysisConfig{std::chrono::seconds(300), {}};
}

}  // namespace sentinel
