set(RESOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH
    "Write path of root directory in which all resource files reside")

configure_file("${CMAKE_CURRENT_LIST_DIR}/Resource.hpp.in" "${CMAKE_CURRENT_LIST_DIR}/Resource.hpp" @ONLY)

add_library(resource INTERFACE resource.hpp)
target_compile_features(resource INTERFACE cxx_std_17)
target_include_directories(resource INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

add_library(Resource::resource ALIAS resource)