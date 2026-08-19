/**
 * @file EventReader.cpp
 * @brief Implementation of EventReader.
 * @component analyzer (sentinel-core)
 * @status STUBBED -- TODO(student).
 */

#include "sentinel/EventReader.hpp"

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "sentinel/SentinelException.hpp"

namespace sentinel {

std::vector<SecurityEvent> EventReader::readFromFile(const std::string& path) {
    // File-open check is real (infra, not assessed logic) -- this is what lets a missing
    // --events path surface as exit code 3 (FileError) even before parsing is implemented.
    std::ifstream file(path);
    if (!file.is_open()) {
        throw FileError("could not open events file: " + path);
    }
    return readFromStream(file);
}

std::vector<SecurityEvent> EventReader::readFromStream(std::istream& input) {
    // TODO(student): parse the stream as JSON (throw ParseError on malformed JSON), validate
    // the top-level `{ "events": [...] }` wrapper is present (throw ValidationError if not),
    // then call SecurityEvent::fromJson() on each element, collecting results. Decide (and
    // document) whether one invalid event fails the whole file or is skipped/reported.
    //
    // Stubbed to a safe empty result for now rather than throwing, so the rest of the pipeline
    // (ConfigReader, RuleFactory, ThreatAnalyzer) is exercisable end-to-end before this is
    // implemented -- see docs/ASSUMPTIONS.md.
    (void)input;
    return {};
}

}  // namespace sentinel
