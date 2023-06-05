function(COMMON_CONFIGURATION targetName)
    # See below link for finding GCC options.
    # https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
    if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
        target_compile_options(${targetName}
        PRIVATE
            -fconcepts
            -Wall
            -pedantic
            -Wextra
        )

    # See below link for finding Clang options.
    # https://clang.llvm.org/docs/ClangCommandLineReference.html
    elseif(CMAKE_CXX_COMPILER_ID MATCHES CLANG)
        # Clang options differ by platform.
        if(WIN32)   # clang-cl
            target_compile_options(${targetName}
            PRIVATE
                "/clang:-fcoroutines-ts"
                -fms-compatiability)
        else()  # AppleClang or Clang on Linux
            target_compile_options(${targetName}
            PRIVATE
                -std=c++2a
            )
        endif()

    # See below link for finding MSVC options.
    # https://docs.microsoft.com/en-us/cpp/build/reference/compiler-options-listed-by-category?view=msvc-160
    elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
        target_compile_options(${targetName}
        PRIVATE
            /W3
            /JMC-
            /Zc:__cplusplus
            /Zc:preprocessor
        )
    endif()

    # Platform setting
    if(WIN32)
        target_compile_definitions(${targetName}
        PRIVATE
            WIN32_LEAN_AND_MIN NOMINMAX
        )
        set_target_properties(${targetName}
        PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS OFF
        )
    endif()
endfunction()