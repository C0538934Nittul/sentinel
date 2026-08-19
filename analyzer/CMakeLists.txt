# Sentinel analysis engine.
# Component: analyzer/
# Status: build configuration is complete; rule logic in src/*.cpp is stubbed (see TODO(student)).

cmake_minimum_required(VERSION 3.21)
project(sentinel-analyzer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

include(FetchContent)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")

set(SENTINEL_SOURCES
  src/SecurityEvent.cpp
  src/RepeatedFailureRule.cpp
  src/SuccessAfterFailuresRule.cpp
  src/MultiAccountProbeRule.cpp
  src/ThreatAnalyzer.cpp
  src/IncidentResult.cpp
  src/EventReader.cpp
  src/ConfigReader.cpp
  src/RuleFactory.cpp
  src/Logger.cpp
)

add_library(sentinel-core ${SENTINEL_SOURCES})
target_include_directories(sentinel-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(sentinel-core PUBLIC nlohmann_json::nlohmann_json)

add_executable(sentinel-analyzer src/main.cpp)
target_link_libraries(sentinel-analyzer PRIVATE sentinel-core)

enable_testing()
add_subdirectory(tests)
