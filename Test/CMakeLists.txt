enable_testing()

add_executable(mocktest)

target_sources(mocktest PRIVATE main.cpp)

find_package(GTest)

message(STATUS "Finding GTest...")
if(Gtest_FOUND)
    message(STATUS "GTest - Found.")
else()
    message(STATUS "GTest - Not found.")
    message(STATUS "Downloading GTest from github...")

    set(GTEST_COMMIT_HASH "2dd1c131950043a8ad5ab0d2dda0e0970596586a" CACHE
        STRING "set git commit hash for specific version of google test." FORCE)
    
    # for consistent behavior of FetchContent
    if (POLICY CMP0135)
        cmake_policy(SET CMP0135 OLD)
    endif()

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL "https://github.com/google/googletest/archive/${GTEST_COMMIT_HASH}.zip"
    )

    message(STATUS "GTest (${GTEST_COMMIT_HASH}) Downloaded.")

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

target_link_libraries(mocktest PRIVATE GTest::gtest_main)
# TODO: separate paths for build interface/install interface
target_include_directories(mocktest PRIVATE "${gtest_SOURCE_DIR}/include")

gtest_discover_tests(mocktest)

set(TEST_CONFIG Release CACHE STRING "configuration for testing")
set_property(CACHE TEST_CONFIG PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})

# TODO: separate paths for build interface/install interface
add_custom_target(
    test
    COMMENT "test features"
    COMMAND ctest -C ${TEST_CONFIG} --test-dir "${CMAKE_CURRENT_BINARY_DIR}" --output-on-failure
)

option(BUILD_TESTING "enable if you want to test each time you build the project" OFF)
if(BUILD_TESTING)
    add_custom_command(TARGET mocktest POST_BUILD
        COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_CURRENT_BINARY_DIR}" --target test
        COMMENT "Building test target after all gclp_test target is built."
    )
endif()