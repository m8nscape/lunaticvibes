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
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.lib
        )
    else()
        set_target_properties(SDL2 PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.lib
        )
    endif()
else()
  message(FATAL_ERROR "There is no bundled SDL2 for this platform")
endif()
add_library(SDL2::SDL2 ALIAS SDL2)

target_include_directories(SDL2 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)


add_library(SDL2main SHARED IMPORTED GLOBAL)
if (WIN32)
    if (${BUILD_X64})
        set_target_properties(SDL2main PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2main.lib
        )
    else()
        set_target_properties(SDL2main PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2main.lib
        )
    endif()
else()
  message(FATAL_ERROR "There is no bundled SDL2main for this platform")
endif()
add_library(SDL2::SDL2main ALIAS SDL2main)

target_include_directories(SDL2main INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
