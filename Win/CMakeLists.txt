cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.06.19 LANGUAGES CXX)

add_executable(Win)
add_executable(KeyboardTest)

target_sources(Win PRIVATE main.cpp WindowBase.hpp WindowBase.inl WindowBase.cpp
    Window.hpp Window.cpp WindowsMessageMap.cpp WindowsMessageMap.hpp
    Woon2Exception.hpp Woon2Exception.cpp macros.hpp
    Keyboard.hpp Keyboard.cpp)

target_sources(KeyboardTest PRIVATE KeyboardTest.cpp WindowBase.hpp WindowBase.inl WindowBase.cpp
    Keyboard.hpp Keyboard.cpp KeyboardTestWindow.hpp KeyboardTestWindow.cpp Woon2Exception.hpp Woon2Exception.cpp
    macros.hpp)

# Common Options

include(${CMAKE_SOURCE_DIR}/cmake/win32app_configuration.cmake)

WIN32APP_MINIMUM_CONFIGURATION(Win)
WIN32APP_MINIMUM_CONFIGURATION(KeyboardTest)