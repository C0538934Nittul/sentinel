/**
 * @file EventReader.hpp
 * @brief Loads and validates a security-event file into a vector of SecurityEvent.
 * @component analyzer (sentinel-core)
 * @status Declaration complete. Implementation in src/EventReader.cpp is stubbed.
 */

#pragma once

#include <istream>
#include <string>
#include <vector>

#include "sentinel/SecurityEvent.hpp"

namespace sentinel {

/**
 * @class EventReader
 * @brief Reads the `{ "events": [...] }` file format described in docs/data-contract.md.
 *
 * Stateless utility -- all methods are static. Kept as a class (rather than free functions) so
 * it groups with ConfigReader under a consistent "reader" naming convention and leaves room for
 * future stateful readers (e.g. streaming) without changing call sites.
 */
class EventReader {
public:
    EventReader() = delete;

    /**
     * @brief Load and validate events from a file path.
     * @throws FileError if the file cannot be opened.
     * @throws ParseError if the file is not valid JSON.
     * @throws ValidationError if the JSON is valid but does not match the event schema, or if
     *         any individual event fails SecurityEvent::fromJson validation.
     */
    [[nodiscard]] static std::vector<SecurityEvent> readFromFile(const std::string& path);

    /**
     * @brief Load and validate events from an already-open input stream (used for --stdin).
     * @throws ParseError, ValidationError -- see readFromFile().
     */
    [[nodiscard]] static std::vector<SecurityEvent> readFromStream(std::istream& input);
};

}  // namespace sentinel
