cmake_minimum_required(VERSION 3.18)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

    if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
        set(DEFAULT_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")

        message(STATUS "Setting configuration types to ${DEFAULT_CONFIGURATION_TYPES}
            as none was specified."
        )
        set(CMAKE_CONFIGURATION_TYPES ${DEFAULT_CONFIGURATION_TYPES} CACHE STRING
            "Specify available build types."
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
        set (CMAKE_INSTALL_PREFIX "${DEFAULT_INSTALL_PATH}" CACHE PATH
            "Specify the path to install this project"
        )
    endif()

endif()

project (PUtility VERSION 2023.10.09
    DESCRIPTION "Project for devloper-utility functionalities"
    LANGUAGES CXX
)

message(STATUS "configuring PUtility(v${CMAKE_PROJECT_VERSION})...")
message("Author: @Woon2 (wkdauddns9@gmail.com)")

# header only libraries

add_library(enum_util INTERFACE EnumUtil.hpp)
add_library(iterate_call INTERFACE IterateCall.hpp)
add_library(num_args INTERFACE NumArgs.hpp)
add_library(onehot_encode INTERFACE OneHotEncode.hpp)
add_library(literal INTERFACE Literal.hpp)
add_library(string_like INTERFACE StringLike.hpp)

target_compile_features(enum_util INTERFACE cxx_std_20)
target_compile_features(literal INTERFACE cxx_std_17)
target_compile_features(string_like INTERFACE cxx_std_20)

target_link_libraries(iterate_call INTERFACE num_args)
target_link_libraries(onehot_encode INTERFACE num_args)

# concrete libraries

add_library(woon2_exception Woon2Exception.cpp Woon2Exception.hpp)

# See below link for msvc options
# https://learn.microsoft.com/en-us/cpp/build/reference/compiler-options?view=msvc-170
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    target_compile_options(woon2_exception
    PRIVATE
        /MP
        $<IF:$<CONFIG:Debug>,/MDd,/MD>
        /sdl-
        /Zc:preprocessor
        /Zc:__cplusplus
        /W3
        $<IF:$<CONFIG:Debug>,/Od,/O2>
    )

# See below link for gcc options
# https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
elseif(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    target_compile_options(woon2_exception
    PRIVATE
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
        target_compile_options(woon2_exception
        PRIVATE
            "/clang:-fcoroutines-ts"
            -fms-compatiability)
    else()  # AppleClang or Clang on Linux
        target_compile_options(woon2_exception
        PRIVATE
            -std=c++14
        )
    endif()

else()
    message(WARNING "building with unknown compiler,
        the build might not be successful."
    )
endif()

# Platform setting
if(WIN32)
    set_target_properties(woon2_exception
    PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS OFF
    )
endif()