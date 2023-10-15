cmake_minimum_required(VERSION 3.12)

function(find_hlsl_compiler)
    # Detect compiler and set HLSL compiler accordingly

    # For Microsoft Visual Studio (MSVC)
    if(MSVC)
        find_program(FXC_COMPILER "fxc.exe")
        if(FXC_COMPILER-NOTFOUND)
            message(FATAL_ERROR "cannot find fxc compiler.")
        endif()

        set(HLSL_COMPILER "fxc.exe" CACHE PATH
            "Write path to hlsl compiler"
        )
    # For other compilers/platforms (assuming dxc.exe for Windows other than MSVC,
    # and wine+dxc.exe for non-Windows platforms)
    else()
        find_program(DXC_COMPILER "dxc.exe")
        if(DXC_COMPILER-NOTFOUND)
            message(FATAL_ERROR "cannot find dxc compiler.")
        endif()

        set(HLSL_COMPILER "dxc.exe" CACHE PATH
            "Write path to hlsl compiler"
        )
    endif()
endfunction()

function(compile_hlsl INPUT_HLSL_FILE OUTPUT_CSO_FILE SHADER_TYPE)
    find_hlsl_compiler()

    add_custom_command(
        OUTPUT "${OUTPUT_CSO_FILE}"
        COMMAND "${HLSL_COMPILER}" /T "${SHADER_TYPE}" /Fo "${OUTPUT_CSO_FILE}" "${INPUT_HLSL_FILE}"
        DEPENDS "${INPUT_HLSL_FILE}"
    )
    set(HLSL_COMPILED_SHADERS "${HLSL_COMPILED_SHADERS};${OUTPUT_CSO_FILE}"
        CACHE STRING "compiled shaders(.hlsl)" FORCE
    )
    mark_as_advanced(HLSL_COMPILED_SHADERS)
endfunction()

function(add_hlsl_target CSO_DEPENDENCIES)
    find_hlsl_compiler()
    # Create a custom target for shader compilation
    add_custom_target(hlsl DEPENDS "${CSO_DEPENDENCIES}")
endfunction()