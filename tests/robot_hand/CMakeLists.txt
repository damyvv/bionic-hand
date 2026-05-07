file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(runRobotHandTests
    ${TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/robot_hand/Finger.cpp
    ${CMAKE_SOURCE_DIR}/src/robot_hand/drivers/Servo.cpp
    ${CMAKE_SOURCE_DIR}/src/robot_hand/drivers/Pca9685.cpp
    ${CMAKE_SOURCE_DIR}/src/robot_hand/drivers/Pca9685Channel.cpp
    ${CMAKE_SOURCE_DIR}/src/robot_hand/demo/DemoStates.cpp
    ${CMAKE_SOURCE_DIR}/src/robot_hand/demo/HandDemo.cpp
    ${CMAKE_SOURCE_DIR}/libs/amp-embedded-infra-lib/hal/interfaces/test_doubles/I2cMock.cpp
)

target_include_directories(runRobotHandTests PRIVATE
    ${CMAKE_SOURCE_DIR}/src/robot_hand
    ${CMAKE_SOURCE_DIR}/libs/amp-embedded-infra-lib
)

target_compile_definitions(runRobotHandTests PRIVATE
    FINGER_COUNT=5
)

target_link_libraries(runRobotHandTests PRIVATE
    gmock
    gtest
    gtest_main
    hal.interfaces
    infra.timer
)

gtest_discover_tests(runRobotHandTests)