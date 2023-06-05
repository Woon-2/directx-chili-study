cmake_minimum_required(VERSION 3.18)

project(InitialTest VERSION 2023.06.05 LANGUAGES CXX)

enable_testing()

add_executable(initial_test main.cpp)

include("${CMAKE_SOURCE_DIR}/cmake/common_configuration.cmake")
COMMON_CONFIGURATION(initial_test)

target_link_libraries(initial_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(initial_test)