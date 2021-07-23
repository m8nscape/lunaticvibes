cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME dexceed)

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

add_executable(${PROJECT_NAME} 
    src/main.cpp
)

if (UNIX OR MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE -lpthread)
    target_link_libraries(${PROJECT_NAME} PRIVATE -lstdc++fs)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17)

#########################################################################
# GRAPHICS BACKEND

# SDL2, 
set(GRAPHICS_BACKEND SDL2 CACHE STRING "Graphics backend" )

#########################################################################
# DEPENDENCIES
add_subdirectory(ext)

#########################################################################
# SOURCE

set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src CACHE INTERNAL "include directory")
add_subdirectory(src)


#########################################################################
# SOURCE
target_link_libraries(${PROJECT_NAME} PRIVATE
    common
    config
    db
    game
)