if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the SDL2 source code and call cmake from there")
endif()

##### general #####
cmake_minimum_required(VERSION 3.0)

set(SDL2_MAJOR_VERSION 2)
set(SDL2_MINOR_VERSION 0)
set(SDL2_MICRO_VERSION 14)
set(SDL2_INTERFACE_AGE 0)
set(SDL2_VERSION "${SDL2_MAJOR_VERSION}.${SDL2_MINOR_VERSION}.${SDL2_MICRO_VERSION}")



##### library generation #####
add_library(SDL2 SHARED IMPORTED GLOBAL)
if (WIN32)
    if (${BUILD_X64})
        set_target_properties(SDL2 PROPERTIES
            IMPORTED_LOCATION_SDL2    ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.dll
            IMPORTED_IMPLIB_SDL2      ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib
        )
    else()
        set_target_properties(SDL2 PROPERTIES
            IMPORTED_LOCATION_SDL2    ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.dll
            IMPORTED_IMPLIB_SDL2      ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib
        )
    endif()
elseif (LINUX)
endif()

target_include_directories(SDL2 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
