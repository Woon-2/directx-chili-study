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

project(POngoing VERSION 2023.10.29 LANGUAGES CXX)

message(STATUS "Configuring ${CMAKE_PROJECT_NAME}(v${CMAKE_PROJECT_VERSION})...")
message(STATUS "Author: @Woon2 (wkdauddns9@gmail.com)")

add_executable(chili)

target_sources(chili PRIVATE
    src/main.cpp
    include/ChiliWindow.hpp
    src/ChiliWindow.cpp
    include/MsgHandlers.hpp
    include/Keyboard.hpp
    include/Mouse.hpp
    include/App.hpp
    include/Graphics.hpp
    include/GraphicsException.hpp
    include/GraphicsNamespaces.hpp
    include/DrawComponent.hpp
    include/Bindable.hpp
    src/GraphicsException.cpp
)

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
            /fp:fast
            /JMC
            /wd5104
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

auto_configure(chili PRIVATE)

# Shader related stuffs

configure_file("${PROJECT_SOURCE_DIR}/shader/ShaderPath.h.in"
    "${PROJECT_SOURCE_DIR}/shader/ShaderPath.h"
)
set(COMPILED_SHADER_PATH "${PROJECT_BINARY_DIR}/shader" CACHE PATH
    "Write path to populate compiled shaders."
)

include(find_hlsl_compiler)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/VertexShader.hlsl"
    "${COMPILED_SHADER_PATH}/VertexShader.cso"
    "vs_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/PixelShader.hlsl"
    "${COMPILED_SHADER_PATH}/PixelShader.cso"
    "ps_4_0"
)
add_hlsl_target("${HLSL_COMPILED_SHADERS}")

add_dependencies(chili hlsl)

# link modules

target_link_libraries(chili
PRIVATE
    Win::win
    Utility::literal
    Utility::timer
    Resource::resource
    d3d11.lib
    D3DCompiler.lib
    $<$<CONFIG:DEBUG>:dxguid.lib>
)

target_include_directories(chili PRIVATE include shader)