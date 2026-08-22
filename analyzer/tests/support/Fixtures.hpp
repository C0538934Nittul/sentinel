/**
 * @file Fixtures.hpp
 * @brief Resolves paths under sample-data/ to absolute paths, independent of ctest's working
 *        directory.
 * @component analyzer/tests/support
 * @status Complete. Test infrastructure, not assessed logic.
 */

#pragma once

#include <filesystem>
#include <string>

#ifndef SENTINEL_SAMPLE_DATA_DIR
#error "SENTINEL_SAMPLE_DATA_DIR must be defined by the build -- see tests/CMakeLists.txt's " \
       "target_compile_definitions(sentinel-tests ...) line."
#endif

namespace sentinel::test {

/**
 * @brief Absolute path to a file under sample-data/.
 *
 * SENTINEL_SAMPLE_DATA_DIR is injected by CMake as CMAKE_CURRENT_SOURCE_DIR/../../sample-data
 * (see tests/CMakeLists.txt), i.e. resolved at configure time relative to the repository
 * layout -- not a path relative to the process's current working directory, which ctest does
 * not guarantee to be the repo root (or any particular directory at all).
 */
inline std::string sampleDataPath(const std::string& filename) {
    return (std::filesystem::path(SENTINEL_SAMPLE_DATA_DIR) / filename).string();
}

}  // namespace sentinel::test
