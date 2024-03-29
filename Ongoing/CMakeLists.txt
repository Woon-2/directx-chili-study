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

project(POngoing VERSION 2024.03.19 LANGUAGES CXX)

message(STATUS "Configuring ${CMAKE_PROJECT_NAME}(v${CMAKE_PROJECT_VERSION})...")
message(STATUS "Author: @Woon2 (wkdauddns9@gmail.com)")

add_executable(chili)

# Application Layer sources
target_sources(chili PRIVATE
    src/main.cpp
    src/App/App.cpp
    src/App/ChiliWindow.cpp
    src/App/Keyboard.cpp
    src/App/MsgHandlers.cpp
    include/App/App.hpp
    include/App/ChiliWindow.hpp
    include/App/MsgHandlers.hpp
    include/App/Keyboard.hpp
    include/App/Mouse.hpp
)

# Game Layer sources
target_sources(chili PRIVATE
    src/Game/PrimitiveEntity.cpp
    src/Game/Game.cpp
    src/Game/InputSystem.cpp
    src/Game/GTransformComponent.cpp
    src/Game/SimulationUI.cpp
    src/Game/CoordSystem.cpp
    src/Game/CameraControl.cpp
    src/Game/PointLightControl.cpp

    include/Game/PrimitiveEntity.hpp
    include/Game/Game.hpp
    include/Game/Entity.hpp
    include/Game/InputSystem.hpp
    include/Game/InputComponent.hpp
    include/Game/Chrono.hpp
    include/Game/GTransformComponent.hpp
    include/Game/SimulationUI.hpp
    include/Game/CoordSystem.hpp
    include/Game/CameraControl.hpp
    include/Game/PointLightControl.hpp
    include/Game/IlluminatedBox.hpp
)

# GFX sources
target_sources(chili PRIVATE
    src/GFX/Core/Storage.cpp
    src/GFX/Core/Exception.cpp
    src/GFX/Core/SwapChain.cpp
    src/GFX/Core/Graphics.cpp
    src/GFX/Core/CMDLogger.cpp

    include/GFX/Core/Graphics.hpp
    include/GFX/Core/Factory.hpp
    include/GFX/Core/SwapChain.hpp
    include/GFX/Core/Storage.hpp
    include/GFX/Core/Pipeline.hpp
    include/GFX/Core/Exception.hpp
    include/GFX/Core/Namespaces.hpp
    include/GFX/Core/CMDLogger.hpp
    include/GFX/Core/Transform.hpp

    src/GFX/Scenery/Renderer.cpp
    src/GFX/Scenery/Camera.cpp
    src/GFX/Scenery/PointLight.cpp
    src/GFX/Scenery/SolidMaterial.cpp
    src/GFX/Scenery/TransformCBuffer.cpp
    src/GFX/Scenery/TransformDrawContexts.cpp
    src/GFX/Scenery/CMDSummarizer.cpp
    src/GFX/Scenery/CMDLogFileView.cpp
    src/GFX/Scenery/CMDLogGUIView.cpp

    include/GFX/Scenery/Renderer.hpp
    include/GFX/Scenery/RendererSystem.hpp
    include/GFX/Scenery/Camera.hpp
    include/GFX/Scenery/Scene.hpp
    include/GFX/Scenery/DrawComponent.hpp
    include/GFX/Scenery/RCDrawComponent.hpp
    include/GFX/Scenery/RenderObjectDesc.hpp
    include/GFX/Scenery/RendererDesc.hpp
    include/GFX/Scenery/PointLight.hpp
    include/GFX/Scenery/SolidMaterial.hpp
    include/GFX/Scenery/TransformDrawContexts.hpp
    include/GFX/Scenery/TransformCBuffer.hpp
    include/GFX/Scenery/CMDSummarizer.hpp
    include/GFX/Scenery/CMDLogFileView.hpp
    include/GFX/Scenery/CMDLogGUIView.hpp

    src/GFX/PipelineObjects/Buffer.cpp
    src/GFX/PipelineObjects/DrawCaller.cpp
    src/GFX/PipelineObjects/RenderTarget.cpp
    src/GFX/PipelineObjects/Shader.cpp
    src/GFX/PipelineObjects/Texture.cpp
    src/GFX/PipelineObjects/Sampler.cpp

    include/GFX/PipelineObjects/PipelineObject.hpp
    include/GFX/PipelineObjects/Buffer.hpp
    include/GFX/PipelineObjects/DrawCaller.hpp
    include/GFX/PipelineObjects/DrawContext.hpp
    include/GFX/PipelineObjects/IA.hpp
    include/GFX/PipelineObjects/RenderTarget.hpp
    include/GFX/PipelineObjects/Shader.hpp
    include/GFX/PipelineObjects/Topology.hpp
    include/GFX/PipelineObjects/Viewport.hpp
    include/GFX/PipelineObjects/Texture.hpp
    include/GFX/PipelineObjects/Sampler.hpp

    include/GFX/Primitives/Cube.hpp
    include/GFX/Primitives/Plane.hpp
    include/GFX/Primitives/Prism.hpp
    include/GFX/Primitives/Cone.hpp
    include/GFX/Primitives/Sphere.hpp
)

# Image processing sources
target_sources(chili PRIVATE
    src/Image/GDIPlusMgr.cpp
    src/Image/Surface.cpp

    include/Image/GDIPlusMgr.hpp
    include/Image/Surface.hpp
)

# UI sources
target_sources(chili PRIVATE
    src/UI/ImguiMgr.cpp
    include/UI/ImguiMgr.hpp
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
            /wd4996
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
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/VertexShaderIndexed.hlsl"
    "${COMPILED_SHADER_PATH}/VertexShaderIndexed.cso"
    "vs_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/PixelShaderIndexed.hlsl"
    "${COMPILED_SHADER_PATH}/PixelShaderIndexed.cso"
    "ps_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/VertexShaderBlended.hlsl"
    "${COMPILED_SHADER_PATH}/VertexShaderBlended.cso"
    "vs_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/PixelShaderBlended.hlsl"
    "${COMPILED_SHADER_PATH}/PixelShaderBlended.cso"
    "ps_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/VertexShaderTextured.hlsl"
    "${COMPILED_SHADER_PATH}/VertexShaderTextured.cso"
    "vs_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/PixelShaderTextured.hlsl"
    "${COMPILED_SHADER_PATH}/PixelShaderTextured.cso"
    "ps_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/VertexShaderBPhong.hlsl"
    "${COMPILED_SHADER_PATH}/VertexShaderBPhong.cso"
    "vs_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/PixelShaderBPhong.hlsl"
    "${COMPILED_SHADER_PATH}/PixelShaderBPhong.cso"
    "ps_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/VertexShaderSolid.hlsl"
    "${COMPILED_SHADER_PATH}/VertexShaderSolid.cso"
    "vs_4_0"
)
compile_hlsl("${PROJECT_SOURCE_DIR}/shader/PixelShaderSolid.hlsl"
    "${COMPILED_SHADER_PATH}/PixelShaderSolid.cso"
    "ps_4_0"
)
add_hlsl_target("${HLSL_COMPILED_SHADERS}")

add_dependencies(chili hlsl)

# link modules
find_package(imgui REQUIRED PATHS "${CMAKE_MODULE_PATH}")

target_link_libraries(chili
PRIVATE
    Win::win
    Utility::literal
    Utility::timer
    Utility::aconcepts
    Utility::aranges
    Utility::onehot_encode
    Utility::enum_util
    Utility::generator
    Resource::resource
    woon2cache::LRUCache
    d3d11.lib
    D3DCompiler.lib
    gdiplus.lib
    $<$<CONFIG:DEBUG>:dxguid.lib>
    imgui::imgui_win32_dx11
)

target_include_directories(chili PRIVATE include shader)