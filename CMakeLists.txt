cmake_minimum_required(VERSION 3.16.3)

project(xne LANGUAGES C CXX)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(MAIN "${SOURCE_DIR}/main.c")

include_directories(${INCLUDE_DIR})

set(GLEW_LIB ${LIB_DIR}/libglew32.dll.a)
if(NOT EXISTS ${GLEW_LIB})
    message(FATAL_ERROR "Cannot find GLEW '${GLEW_LIB}'")
endif()

set(GLFW_LIB ${LIB_DIR}/libglfw3.a)
if(NOT EXISTS ${GLFW_LIB})
    message(FATAL_ERROR "Cannot find GLFW '${GLFW_LIB}'")
endif()

set(Z_LIB ${LIB_DIR}/libzlib.dll.a)
if(NOT EXISTS ${Z_LIB})
    message(FATAL_ERROR "Cannot find ZLIB '${Z_LIB}'")
endif()

set(PNG_LIB ${LIB_DIR}/libpng16.dll.a)
if( NOT EXISTS ${PNG_LIB})
    message(FATAL_ERROR "Cannot find LIBPNG '${PNG_LIB}'")
endif()

set(JSON_LIB ${LIB_DIR}/libjson-c.dll.a)
if( NOT EXISTS ${JSON_LIB})
    message(FATAL_ERROR "Cannot find JSON-C '${JSON_LIB}'")
endif()

add_executable(xne)
find_package(OpenGL REQUIRED)

target_sources(xne PRIVATE ${MAIN})

target_link_libraries(xne 
    ${OPENGL_LIBRARIES} 
    ${GLEW_LIB} 
    ${GLFW_LIB}
    ${Z_LIB}
    ${PNG_LIB}
    ${JSON_LIB}
)

target_include_directories(xne PRIVATE ${SOURCE_DIR})

add_subdirectory("${SOURCE_DIR}/core")
add_subdirectory("${SOURCE_DIR}/graphics")
add_subdirectory("${SOURCE_DIR}/graphics/ui")
add_subdirectory("${SOURCE_DIR}/engine")

set_target_properties(xne PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}"
    ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}"
)