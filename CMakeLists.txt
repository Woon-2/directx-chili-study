# *****************************
# Copylight @Woon2
# (wkdauddns9@gmail.com)
# Simple script for building DirectX12 Project
# *****************************

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

project(DirectX_Study VERSION 2023.12.05)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

message(STATUS "configuring ${CMAKE_PROJECT_NAME}(v${CMAKE_PROJECT_VERSION})...")
message("Author: @Woon2 (wkdauddns9@gmail.com)")

include(gitlog)
add_gitlog(8)

add_subdirectory(extern)
add_subdirectory(Win)
add_subdirectory(Utility)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    add_subdirectory(Test)
endif()
add_subdirectory(Resource)
add_subdirectory(Ongoing)