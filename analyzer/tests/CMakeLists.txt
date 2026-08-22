# Test build configuration.
# Component: analyzer/tests
# Status: complete. TEST_CASE bodies within most .cpp files below are empty checklists -- see
#         each file's header comment. support/*.hpp and test_support_harness.cpp are complete,
#         real test infrastructure (Step 5) for you to build the rule/analyzer tests on top of.

add_executable(sentinel-tests
  test_SecurityEvent.cpp
  test_IncidentResult.cpp
  test_RepeatedFailureRule.cpp
  test_SuccessAfterFailuresRule.cpp
  test_MultiAccountProbeRule.cpp
  test_ThreatAnalyzer.cpp
  test_EventReader.cpp
  test_ConfigReader.cpp
  test_RuleFactory.cpp
  test_Logger.cpp
  test_support_harness.cpp
)

target_link_libraries(sentinel-tests PRIVATE sentinel-core Catch2::Catch2WithMain)

# Lets test files #include "support/EventBuilder.hpp" etc.
target_include_directories(sentinel-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Absolute path to sample-data/, injected at configure time -- see support/Fixtures.hpp for why
# this isn't just a path relative to the test binary's working directory.
target_compile_definitions(sentinel-tests PRIVATE
  SENTINEL_SAMPLE_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../sample-data"
)

include(CTest)
include(Catch)
catch_discover_tests(sentinel-tests)
