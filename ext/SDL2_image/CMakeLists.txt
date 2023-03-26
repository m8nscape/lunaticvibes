if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the SDL_image source code and call cmake from there")
endif()

##### general #####
cmake_minimum_required(VERSION 3.0)

find_package(SDL2 REQUIRED)

set(SDL_IMAGE_MAJOR_VERSION 2)
set(SDL_IMAGE_MINOR_VERSION 0)
set(SDL_IMAGE_MICRO_VERSION 5)
set(SDL_IMAGE_INTERFACE_AGE 0)
set(SDL_IMAGE_VERSION "${SDL_IMAGE_MAJOR_VERSION}.${SDL_IMAGE_MINOR_VERSION}.${SDL_IMAGE_MICRO_VERSION}")


if (${BUILD_X64})
    set(LIB_DIR x64)
else()
    set(LIB_DIR x86)
endif()

##### library generation #####
add_library(SDL2_image::libjpeg UNKNOWN IMPORTED GLOBAL)
add_library(SDL2_image::libpng UNKNOWN IMPORTED GLOBAL)
add_library(SDL2_image::libtiff UNKNOWN IMPORTED GLOBAL)
add_library(SDL2_image::libwebp UNKNOWN IMPORTED GLOBAL)

add_library(SDL2_image SHARED IMPORTED GLOBAL)
if (WIN32)
    set_target_properties(SDL2_image::libjpeg PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/${LIB_DIR}/libjpeg-9.dll)
    set_target_properties(SDL2_image::libpng PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/${LIB_DIR}/libpng16-16.dll)
    set_target_properties(SDL2_image::libtiff PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/${LIB_DIR}/libtiff-5.dll)
    set_target_properties(SDL2_image::libwebp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/${LIB_DIR}/libwebp-7.dll)
    
    set_target_properties(SDL2_image PROPERTIES
        IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/${LIB_DIR}/SDL2_image.dll
        IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/${LIB_DIR}/SDL2_image.lib  
    )
else()
  message(FATAL_ERROR "There is no bundled SDL2_image for this platform")
endif()

target_include_directories(SDL2_image INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
