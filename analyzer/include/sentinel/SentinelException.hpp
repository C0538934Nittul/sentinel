/**
 * @file SentinelException.hpp
 * @brief Exception hierarchy for the analyzer.
 * @component analyzer (sentinel-core)
 * @status Complete. This is a thin, fully header-only hierarchy -- there is no meaningful
 *         logic to stub, so it is provided in full. main.cpp's top-level catch clauses map
 *         each of these to a specific process exit code.
 */

#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace sentinel {

/**
 * @brief Base class for all exceptions raised by sentinel-core.
 *
 * Never thrown directly -- always throw one of the derived types below so callers (main.cpp,
 * tests) can distinguish failure categories.
 */
class SentinelException : public std::runtime_error {
public:
    explicit SentinelException(std::string message) : std::runtime_error(std::move(message)) {}
};

/// Raised when a file cannot be opened, read, or written.
class FileError : public SentinelException {
public:
    explicit FileError(std::string message) : SentinelException(std::move(message)) {}
};

/// Raised when input text cannot be parsed as valid JSON.
class ParseError : public SentinelException {
public:
    explicit ParseError(std::string message) : SentinelException(std::move(message)) {}
};

/// Raised when parsed JSON does not satisfy the data contract (missing/invalid fields).
class ValidationError : public SentinelException {
public:
    explicit ValidationError(std::string message) : SentinelException(std::move(message)) {}
};

/// Raised when config/rules.json is missing required fields or has invalid values.
class ConfigError : public SentinelException {
public:
    explicit ConfigError(std::string message) : SentinelException(std::move(message)) {}
};

}  // namespace sentinel
