cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.06.05 LANGUAGES CXX)

add_executable(Win main.cpp)

# Common Options

include("${CMAKE_SOURCE_DIR}/cmake/common_configuration.cmake")
COMMON_CONFIGURATION(Win)

target_compile_options(Win
PRIVATE
    /MP     # compilation speed-up via multi processor
    /sdl    # security check
    /Ot     # maximum speed optimization
    /fp:fast    # rate speed over accuracy on floating points
)

target_compile_definitions(Win
PRIVATE
    UNICODE _UNICODE
)

target_link_options(Win
PRIVATE
    "/SUBSYSTEM:WINDOWS"
    "/ENTRY:WinMainCRTStartup"
)


# Release Options

target_compile_definitions(Win
PRIVATE
    $<$<CONFIG:Release>:NDEBUG>     # Standard way to check release mode
)

target_compile_options(Win
PRIVATE
    $<$<CONFIG:Release>:/MT>        # Multi-threaded & Static libraries
)


# Debug Options

target_compile_options(Win
PRIVATE
    $<$<CONFIG:Debug>:/MTd>         # Multi-threaded & Static libraries & debug options
)