cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.07.14 LANGUAGES CXX)

add_executable(Win)
add_executable(KeyboardTest)
add_executable(WinXX)

target_sources(Win PRIVATE main.cpp WindowBase.hpp WindowBase.inl WindowBase.cpp
    Window.hpp Window.cpp WindowsMessageMap.cpp WindowsMessageMap.hpp
    Woon2Exception.hpp Woon2Exception.cpp
    Keyboard.hpp Keyboard.cpp)

target_sources(KeyboardTest PRIVATE KeyboardTest.cpp WindowBase.hpp WindowBase.inl WindowBase.cpp
    Keyboard.hpp Keyboard.cpp KeyboardTestWindow.hpp KeyboardTestWindow.cpp Woon2Exception.hpp Woon2Exception.cpp)

target_sources(WinXX PRIVATE mainxx.cpp WindowBasexx.hpp WindowBasexx.inl)

target_include_directories(Win PRIVATE ${UTILITY_PATH})
target_include_directories(KeyboardTest PRIVATE ${UTILITY_PATH})
target_include_directories(WinXX PRIVATE ${UTILITY_PATH})

include(${CMAKE_SOURCE_DIR}/cmake/win32app_configuration.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/link_resource.cmake)

WIN32APP_MINIMUM_CONFIGURATION(Win PRIVATE)
WIN32APP_MINIMUM_CONFIGURATION(KeyboardTest PRIVATE)
WIN32APP_MINIMUM_CONFIGURATION(WinXX PRIVATE)

LINK_RESOURCE(Win)
LINK_RESOURCE(KeyboardTest)
LINK_RESOURCE(WinXX)