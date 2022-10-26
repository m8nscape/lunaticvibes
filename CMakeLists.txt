cmake_minimum_required(VERSION 3.16)

if (WIN32)
    set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "")
endif()

#########################################################################
# PROJECT CONFIG

project(LunaticVibes
    VERSION 0.9
    LANGUAGES CXX
)
set(PROJECT_TITLE "Lunatic Vibes")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BUILD_X64 TRUE CACHE BOOL "Build x64 arch")
else()
    set(BUILD_X64 FALSE CACHE BOOL "Build x64 arch")
endif()

configure_file(
    "${CMAKE_SOURCE_DIR}/src/cmake_config.h.in"
    "${PROJECT_BINARY_DIR}/cmake_config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

if (MSVC)
    add_compile_options("/utf-8")
    add_compile_options("/bigobj")
    add_definitions(-D_WIN32_WINNT=0x0601) # Target Windows 7
    include_directories("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
    link_directories("${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
endif()

if (WIN32)
    add_definitions(-DNOMINMAX)

    #set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    #set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT runtime)
    #include(InstallRequiredSystemLibraries)
endif()

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
