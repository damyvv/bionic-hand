file(GLOB TEST_SOURCES "*.cpp")

add_executable(runBlinkTests ${TEST_SOURCES})

target_link_libraries(runBlinkTests gtest gtest_main)

gtest_discover_tests(runBlinkTests)