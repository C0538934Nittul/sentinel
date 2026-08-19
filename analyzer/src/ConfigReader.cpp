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
    // TODO(student): open and parse the file (FileError / ParseError as appropriate), then
    // validate and map each entry in "rules" to a RuleConfig -- including parsing the
    // "severity" string into the Severity enum and rejecting unknown values with ConfigError.
    (void)path;
    throw std::logic_error("ConfigReader::readFromFile not implemented");
}

}  // namespace sentinel
