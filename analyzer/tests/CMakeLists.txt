# Test build configuration.
# Component: analyzer/tests
# Status: complete. TEST_CASE bodies within the .cpp files below are empty checklists --
#         see each file's header comment.

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
)

target_link_libraries(sentinel-tests PRIVATE sentinel-core Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(sentinel-tests)
