cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME modelLR CACHE STRING "The project name")

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
# SOURCE
target_link_libraries(${PROJECT_NAME} 
    PUBLIC game
)

if (${EXT_DEPEND_LIBFILES_SET})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXT_DEPEND_LIBFILES}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()