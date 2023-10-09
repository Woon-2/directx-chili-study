cmake_minimum_required(VERSION 3.18)

# top level configuration
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

    if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
        set(DEFAULT_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")

        message(STATUS "Setting configuration types to ${DEFAULT_CONFIGURATION_TYPES}
            as none was specified."
        )
        set(CMAKE_CONFIGURATION_TYPES ${DEFAULT_CONFIGURATION_TYPES} CACHE STRING
            "Write the possible configuration types."
        )
    endif()

    if(NOT DEFINED CMAKE_BUILD_TYPE)
        set(DEFAULT_BUILD_TYPE "Release")
        
        message(STATUS "Settting build type to ${DEFAULT_BUILD_TYPE}
            as none was specified."
        )
        set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING
            "Choose the type of build."
        )
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
            "${CMAKE_CONFIGURATION_TYPES}"
        )
    endif()

    if (NOT DEFINED CMAKE_INSTALL_PREFIX)
        set(DEFAULT_INSTALL_PATH "${CMAKE_SOURCE_DIR}/install")

        message(STATUS "Setting install path to ${DEFAULT_INSTALL_PATH}
            as none was specified."
        )
        set (CMAKE_INSTALL_PREFIX "${DEFAULT_INSTALL_PATH}" CACHE STRING
            "Set the install path."
        )
    endif()

endif()  # top level configuration

project(Utility VERSION 2023.10.09 LANGUAGES CXX)

add_subdirectory(clp)

function(auto_configure TARGET_NAME ACCESS_MODIFIER)
    target_compile_features("${TARGET_NAME}" PRIVATE cxx_std_20)

    if(CMAKE_CXX_COMPILER_ID MATCHES msvc)
        target_compile_options("${TARGET_NAME}"
        ${ACCESS_MODIFIER}
            /MP     # compilation speed-up via multi processor
            $<if:$<CONFIG:DEBUG>,MTd,MT>    # use static multi-threaded crt library
            /sdl    # security check
            $<if:$<CONFIG:DEBUG>,/Od,/O2>     # maximum speed optimization
            /fp:strict    # rate correctness over speed on floating points
            /Zc:preprocessor    # standard behavior of preprocessor
            /Zc:__cplusplus   # updated version macro
            /W3     # warning level
        )

    elseif(CMAKE_CXX_COMPILER_ID MATCHES gnu)
        target_compile_options("${TARGET_NAME}"
        ${ACCESS_MODIFIER}
            -fconcepts
            -Wall
            -pedantic
            -Wextra
        )

    elseif(CMAKE_CXX_COMPILER_ID MATCHES clang)     
        # Clang options differ by platform.
        if(WIN32)   # clang-cl
            target_compile_options("${TARGET_NAME}"
            ${ACCESS_MODIFIER}
                clang:-fcoroutines-ts
                -fms-compatiability)
        else()  # AppleClang or Clang on Linux
        target_compile_options("${TARGET_NAME}"
        ${ACCESS_MODIFIER}
            -std=c++2a
        )
        endif()

    endif() # compile options

    # Platform setting
    if(WIN32)
        set_target_properties(${TARGET_NAME}
        PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS OFF
        )
    endif()

    # Link command-line parser
    target_link_libraries(${TARGET_NAME}
        PRIVATE gclp::gclp
    )
endfunction()

add_executable(GenIterateCall
    GenIterateCall.cpp GenIterateCall.hpp
)
add_executable(GenNumArgs
    GenNumArgs.cpp GenNumArgs.hpp
)
add_executable(GenOneHotEncode
    GenOneHotEncode.cpp GenOneHotEncode.hpp
)

auto_configure(GenIterateCall PRIVATE)
auto_configure(GenNumArgs PRIVATE)
auto_configure(GenOneHotEncode PRIVATE)