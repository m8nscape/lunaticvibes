if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the SDL_ttf source code and call cmake from there")
endif()

##### general #####
cmake_minimum_required(VERSION 3.0)

find_package(SDL2 REQUIRED)

set(SDL_TTF_MAJOR_VERSION 2)
set(SDL_TTF_MINOR_VERSION 0)
set(SDL_TTF_MICRO_VERSION 15)
set(SDL_TTF_INTERFACE_AGE 0)
set(SDL_TTF_VERSION "${SDL_TTF_MAJOR_VERSION}.${SDL_TTF_MINOR_VERSION}.${SDL_TTF_MICRO_VERSION}")



##### library generation #####
add_library(SDL2_ttf::libfreetype UNKNOWN IMPORTED GLOBAL)
add_library(SDL2_ttf::zlib UNKNOWN IMPORTED GLOBAL)

add_library(SDL2_ttf SHARED IMPORTED GLOBAL)
if (WIN32)
    if (${BUILD_X64})
        set_target_properties(SDL2_ttf::libfreetype PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/libfreetype-6.dll
        )
        set_target_properties(SDL2_ttf::zlib PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/zlib1.dll
        )
        set_target_properties(SDL2_ttf PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_ttf.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_ttf.lib  
        )
    else()
        set_target_properties(SDL2_ttf::libfreetype PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x86/libfreetype-6.dll
        )
        set_target_properties(SDL2_ttf::zlib PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/zlib1.dll
        )
        set_target_properties(SDL2_ttf PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_ttf.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_ttf.lib  
        )
    endif()
else()
  message(FATAL_ERROR "There is no bundled SDL2_ttf for this platform")
endif()

target_include_directories(SDL2_ttf INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
