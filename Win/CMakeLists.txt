cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.06.12 LANGUAGES CXX)

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
    # disable unused window stuff, Keep Window.h light
    WIN32_LEAN_AND_MEAN
    NOGDICAPMASKS
    NOSYSMETRICS
    NOMENUS
    NOICONS
    NOSYSCOMMANDS
    NORASTEROPS
    OEMRESOURCE
    NOATOM
    NOCLIPBOARD
    NOCOLOR
    NOCTLMGR
    NODRAWTEXT
    NOKERNEL
    NONLS
    NOMEMMGR
    NOMETAFILE
    NOOPENFILE
    NOSCROLL
    NOSERVICE
    NOSOUND
    NOTEXTMETRIC
    NOWH
    NOCOMM
    NOKANJI
    NOHELP
    NOPROFILER
    NODEFERWINDOWPOS
    NOMCX
    NORPC
    NOPROXYSTUB
    NOIMAGE
    NOTAPE
    NOMINMAX
    # type safety
    # https://learn.microsoft.com/en-us/windows/win32/winprog/enabling-strict
    STRICT
)

target_link_options(Win
PRIVATE
    "/SUBSYSTEM:WINDOWS"
    "/ENTRY:WinMainCRTStartup"
)

COMMON_CONFIGURATION(KeyboardTest)

target_compile_options(KeyboardTest
PRIVATE
    /MP     # compilation speed-up via multi processor
    /sdl    # security check
    /Ot     # maximum speed optimization
    /fp:fast    # rate speed over accuracy on floating points
)

target_compile_definitions(KeyboardTest
PRIVATE
    UNICODE _UNICODE
    # disable unused window stuff, Keep Window.h light
    WIN32_LEAN_AND_MEAN
    NOGDICAPMASKS
    NOSYSMETRICS
    NOMENUS
    NOICONS
    NOSYSCOMMANDS
    NORASTEROPS
    OEMRESOURCE
    NOATOM
    NOCLIPBOARD
    NOCOLOR
    NOCTLMGR
    NODRAWTEXT
    NOKERNEL
    NONLS
    NOMEMMGR
    NOMETAFILE
    NOOPENFILE
    NOSCROLL
    NOSERVICE
    NOSOUND
    NOTEXTMETRIC
    NOWH
    NOCOMM
    NOKANJI
    NOHELP
    NOPROFILER
    NODEFERWINDOWPOS
    NOMCX
    NORPC
    NOPROXYSTUB
    NOIMAGE
    NOTAPE
    NOMINMAX
    # type safety
    # https://learn.microsoft.com/en-us/windows/win32/winprog/enabling-strict
    STRICT
)

target_link_options(KeyboardTest
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

target_compile_definitions(KeyboardTest
PRIVATE
    $<$<CONFIG:Release>:NDEBUG>     # Standard way to check release mode
)

target_compile_options(KeyboardTest
PRIVATE
    $<$<CONFIG:Release>:/MT>        # Multi-threaded & Static libraries
)


# Debug Options

target_compile_options(Win
PRIVATE
    $<$<CONFIG:Debug>:/MTd>         # Multi-threaded & Static libraries & debug options
)

target_compile_options(KeyboardTest
PRIVATE
    $<$<CONFIG:Debug>:/MTd>         # Multi-threaded & Static libraries & debug options
)