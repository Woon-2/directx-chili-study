cmake_minimum_required(VERSION 3.18)

project(Utility VERSION 2023.06.23 LANGUAGES CXX)

add_executable(SampleGenNumArgs)
add_executable(SampleGenOneHotEncode)
add_executable(SampleGenIterateCall)

add_executable(SampleNumArgs)
add_executable(SampleOneHotEncode)
add_executable(SampleIterateCall)

target_sources(SampleGenNumArgs PRIVATE GenNumArgsMain.cpp GenNumArgs.hpp)
target_sources(SampleGenOneHotEncode PRIVATE GenOneHotEncodeMain.cpp GenOneHotEncode.hpp)
target_sources(SampleGenIterateCall PRIVATE GenIterateCallMain.cpp GenIterateCall.hpp)

target_sources(SampleNumArgs PRIVATE SampleNumArgs.cpp NumArgs.hpp)
target_sources(SampleOneHotEncode PRIVATE SampleOneHotEncode.cpp OneHotEncode.hpp NumArgs.hpp)
target_sources(SampleIterateCall PRIVATE SampleIterateCall.cpp IterateCall.hpp NumArgs.hpp)

include(${CMAKE_SOURCE_DIR}/cmake/common_configuration.cmake)

add_library(Quote INTERFACE Quote.hpp)
add_library(Bit INTERFACE Bit.hpp)

COMMON_CONFIGURATION(SampleGenNumArgs PRIVATE)
COMMON_CONFIGURATION(SampleGenOneHotEncode PRIVATE)
COMMON_CONFIGURATION(SampleGenIterateCall PRIVATE)

COMMON_CONFIGURATION(SampleNumArgs PRIVATE)
COMMON_CONFIGURATION(SampleOneHotEncode PRIVATE)
COMMON_CONFIGURATION(SampleIterateCall PRIVATE)

COMMON_CONFIGURATION(Quote INTERFACE)
COMMON_CONFIGURATION(Bit INTERFACE)