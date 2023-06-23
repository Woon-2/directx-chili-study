cmake_minimum_required(VERSION 3.18)

project(Utility VERSION 2023.06.23 LANGUAGES CXX)

add_executable(GenNumArgs)
add_executable(GenOneHotEncode)
add_executable(GenIterateCall)

add_executable(SampleNumArgs)
add_executable(SampleOneHotEncode)
add_executable(SampleIterateCall)

target_sources(GenNumArgs PRIVATE GenNumArgsMain.cpp GenNumArgs.hpp)
target_sources(GenOneHotEncode PRIVATE GenOneHotEncodeMain.cpp GenOneHotEncode.hpp)
target_sources(GenIterateCall PRIVATE GenIterateCallMain.cpp GenIterateCall.hpp)

target_sources(SampleNumArgs PRIVATE SampleNumArgs.cpp NumArgs.hpp)
target_sources(SampleOneHotEncode PRIVATE SampleOneHotEncode.cpp OneHotEncode.hpp NumArgs.hpp)
target_sources(SampleIterateCall PRIVATE SampleIterateCall.cpp IterateCall.hpp NumArgs.hpp)

include(${CMAKE_SOURCE_DIR}/cmake/common_configuration.cmake)

COMMON_CONFIGURATION(GenNumArgs)
COMMON_CONFIGURATION(GenOneHotEncode)
COMMON_CONFIGURATION(GenIterateCall)

COMMON_CONFIGURATION(SampleNumArgs)
COMMON_CONFIGURATION(SampleOneHotEncode)
COMMON_CONFIGURATION(SampleIterateCall)