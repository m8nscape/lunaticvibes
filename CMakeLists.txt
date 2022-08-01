cmake_minimum_required(VERSION 3.16)

#########################################################################
# PROJECT CONFIG

project(LunaticVibes
    VERSION 0.6.2
    LANGUAGES CXX
)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/cmake_config.h.in"
    "${PROJECT_BINARY_DIR}/cmake_config.h"
)
include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    
if (MSVC)
    add_compile_options("/utf-8")
endif()

if (WIN32)
    add_definitions(-DNOMINMAX)
endif()

if (CMAKE_BUILD_TYPE)
    if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
        set(BUILD_DEBUG TRUE CACHE BOOL "Build with debug libraries")
    endif()
endif()

set(BUILD_X64 TRUE CACHE BOOL "Build x64 arch")

#########################################################################
# GRAPHICS BACKEND

# SDL2, 
set(GRAPHICS_BACKEND SDL2 CACHE STRING "Graphics backend" )
set(VIDEO_ENABLED TRUE CACHE BOOL "Build the app with video playback support")

if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    add_definitions(-DRENDER_SDL2)
endif()

#########################################################################
# DEPENDENCIES

set(EXT_DEPEND_LIBFILES "" CACHE INTERNAL "dependencies")
set(EXT_DEPEND_LIBFILES_SET FALSE CACHE INTERNAL "dependencies")
add_subdirectory(ext)

#########################################################################
# SOURCE

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "include directory")
add_subdirectory(src)

#########################################################################
# LIBRARY

#########################################################################
# GTEST
option(BUILD_TEST "Enable building gtest targets" YES)
if (${BUILD_TEST})
    add_subdirectory(test)
endif()