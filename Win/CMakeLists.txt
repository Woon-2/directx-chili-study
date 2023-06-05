cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.06.25 LANGUAGES CXX)

add_executable(Win main.cpp)

include("${CMAKE_SOURCE_DIR}/cmake/common_configuration.cmake")
COMMON_CONFIGURATION(Win)

target_compile_definitions(Win
PRIVATE
    UNICODE _UNICODE
)

target_link_options(Win
PRIVATE
    "/SUBSYSTEM:WINDOWS"
    "/ENTRY:WinMainCRTStartup"
)
