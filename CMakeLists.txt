cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME nullptr CACHE STRING "The project name")

#########################################################################
# PROJECT CONFIG

project(${PROJECT_NAME}
    VERSION 1.0
    LANGUAGES CXX)
    
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
