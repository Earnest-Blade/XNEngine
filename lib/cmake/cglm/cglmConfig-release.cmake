#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cglm::cglm" for configuration "Release"
set_property(TARGET cglm::cglm APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(cglm::cglm PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/libcglm.dll.a"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/libcglm-0.dll"
  )

list(APPEND _cmake_import_check_targets cglm::cglm )
list(APPEND _cmake_import_check_files_for_cglm::cglm "${_IMPORT_PREFIX}/lib/libcglm.dll.a" "${_IMPORT_PREFIX}/bin/libcglm-0.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
