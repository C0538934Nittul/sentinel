/**
 * @file EventReader.cpp
 * @brief Implementation of EventReader.
 * @component analyzer (sentinel-core)
 * @status First-draft implementation (Step 3, Phase 5). Not the final assessed version --
 *         review and rewrite as needed.
 */

#include "sentinel/EventReader.hpp"

#include <fstream>
#include <iostream>
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
    nlohmann::json parsed;
    try {
        input >> parsed;
    } catch (const nlohmann::json::parse_error& ex) {
        throw ParseError(std::string("failed to parse events JSON: ") + ex.what());
    }

    if (!parsed.is_object() || !parsed.contains("events") || !parsed["events"].is_array()) {
        throw ValidationError("events input must be a JSON object with an \"events\" array");
    }

    const auto& rawEvents = parsed["events"];

    // Design decision: one invalid event does NOT fail the whole file. Each bad event is
    // skipped and logged (to stderr, since this is a library function with no Logger
    // dependency of its own -- see docs/ASSUMPTIONS.md), with a summary count at the end. The
    // alternative (fail the whole file on the first bad event) is also documented there.
    std::vector<SecurityEvent> events;
    events.reserve(rawEvents.size());
    std::size_t skipped = 0;
    for (const auto& rawEvent : rawEvents) {
        try {
            events.push_back(SecurityEvent::fromJson(rawEvent));
        } catch (const ValidationError& ex) {
            ++skipped;
            std::cerr << "EventReader: skipping invalid event: " << ex.what() << '\n';
        }
    }

    if (skipped > 0) {
        std::cerr << "EventReader: skipped " << skipped << " of " << rawEvents.size()
                   << " event(s) due to validation errors\n";
    }

    return events;
}

}  // namespace sentinel
