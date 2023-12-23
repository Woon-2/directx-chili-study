#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "imgui::imgui_win32_dx11" for configuration "Debug"
set_property(TARGET imgui::imgui_win32_dx11 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(imgui::imgui_win32_dx11 PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/imgui_win32_dx11d.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/imgui_win32_dx11d.dll"
  )

list(APPEND _cmake_import_check_targets imgui::imgui_win32_dx11 )
list(APPEND _cmake_import_check_files_for_imgui::imgui_win32_dx11 "${_IMPORT_PREFIX}/lib/imgui_win32_dx11d.lib" "${_IMPORT_PREFIX}/bin/imgui_win32_dx11d.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
