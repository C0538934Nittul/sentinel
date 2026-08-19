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
    // TODO(student): open the file (throw FileError if it can't be opened), then delegate to
    // readFromStream().
    (void)path;
    throw std::logic_error("EventReader::readFromFile not implemented");
}

std::vector<SecurityEvent> EventReader::readFromStream(std::istream& input) {
    // TODO(student): parse the stream as JSON (throw ParseError on malformed JSON), validate
    // the top-level `{ "events": [...] }` wrapper is present (throw ValidationError if not),
    // then call SecurityEvent::fromJson() on each element, collecting results. Decide (and
    // document) whether one invalid event fails the whole file or is skipped/reported.
    (void)input;
    throw std::logic_error("EventReader::readFromStream not implemented");
}

}  // namespace sentinel
