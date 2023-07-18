cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.07.18 LANGUAGES CXX)

add_executable(Win)

target_sources(Win PRIVATE main.cpp Window.cpp Window.hpp Window.inl
    ${UTILITY_PATH}/Woon2Exception.cpp ${UTILITY_PATH}/Woon2Exception.hpp)


target_include_directories(Win PRIVATE ${UTILITY_PATH})

include(${CMAKE_SOURCE_DIR}/cmake/win32app_configuration.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/link_resource.cmake)

WIN32APP_MINIMUM_CONFIGURATION(Win PRIVATE)