cmake_minimum_required(VERSION 3.18)

project(PWin VERSION 2023.07.23 LANGUAGES CXX)

add_library(Win)
add_library(WinMsgMap)

target_sources(Win PUBLIC Window.cpp Window.hpp Window.inl)
target_sources(WinMsgMap PUBLIC WindowsMessageMap.hpp WindowsMessageMap.cpp)

function(auto_configure TARGET_NAME ACCESS_MODIFIER)
    target_compile_features(${TARGET_NAME} ${ACCESS_MODIFIER} cxx_std_20)

    # See below link for msvc options
    # https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options?view=msvc-170
    if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        target_compile_options(${TARGET_NAME}
        ${ACCESS_MODIFIER}
            /MP
            $<IF:$<CONFIG:Debug>,/MDd,/MD>
            $<IF:$<CONFIG:Debug>,/Od,/O2>
            /W3
            /Zc:preprocessor
            /Zc:__cplusplus
            /sdl-
        )

    # See below link for gcc options
    # https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
    elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        target_compile_options(${TARGET_NAME}
        ${ACCESS_MODIFIER}
            -Wall
            -pedantic
            -Wextra
            -fconcepts
            $<IF:$<CONFIG:Debug>,-O0, O2>
        )   

    # See below link for clang options
    # https://clang.llvm.org/docs/ClangCommandLineReference.html
    elseif(CMAKE_CXX_COMPILER_ID MATCHES CLANG)
        # Clang options differ by platform.
        if(WIN32)   # clang-cl
            target_compile_options(${TARGET_NAME}
            ${ACCESS_MODIFIER}
                "/clang:-fcoroutines-ts"
                -fms-compatiability)
        else()  # AppleClang or Clang on Linux
            target_compile_options(${TARGET_NAME}
            ${ACCESS_MODIFIER}
                -std=c++2a
            )
        endif()

    else()
        message(WARNING "bulding with unknown compiler,
            the build might not be successful."
        )
    endif()

    target_compile_definitions(${TARGET_NAME}
    ${ACCESS_MODIFIER}
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

    target_link_options(${TARGET_NAME}
    ${ACCESS_MODIFIER}
        /subsystem:WINDOWS
        /ENTRY:WinMainCRTStartup
    )

    # Platform setting
    if(WIN32)
        set_target_properties(${TARGET_NAME}
        PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS OFF
        )
    endif()

endfunction()

auto_configure(Win PUBLIC)
auto_configure(WinMsgMap PUBLIC)

target_link_libraries(Win
PUBLIC
    Utility::woon2_exception
    Utility::aconcepts
    Utility::string_like
    WinMsgMap
)

add_library(Win::Win ALIAS Win)