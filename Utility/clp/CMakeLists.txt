cmake_minimum_required(VERSION 3.18)

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

project(p-gclp
    VERSION 2023.10.07
    LANGUAGES CXX
    DESCRIPTION "a lightweight header-only generic command line parser"
)

add_library(gclp INTERFACE gclp.hpp)

target_compile_features(gclp INTERFACE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID MATCHES msvc)
    target_compile_options(gclp
    INTERFACE
        /Zc:preprocessor    # standard behavior of preprocessor
    )

elseif(CMAKE_CXX_COMPILER_ID MATCHES gnu)
    target_compile_options(gclp
    INTERFACE
        -fconcepts
    )

elseif(CMAKE_CXX_COMPILER_ID MATCHES clang)     
     # Clang options differ by platform.
     if(WIN32)   # clang-cl
        target_compile_options(gclp
        INTERFACE
            clang:-fcoroutines-ts
            -fms-compatiability)
    else()  # AppleClang or Clang on Linux
     target_compile_options(gclp
     INTERFACE
         -std=c++2a
     )
    endif()

endif()  # compile options

target_include_directories(gclp
INTERFACE
    "${PROJECT_SOURCE_DIR}"
)

add_library(gclp::gclp ALIAS gclp)