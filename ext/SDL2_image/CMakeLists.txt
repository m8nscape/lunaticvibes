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



##### library generation #####
add_library(SDL2_image SHARED IMPORTED GLOBAL)
if (WIN32)
    if (${BUILD_X64})
        set_target_properties(SDL2_image PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_image.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x64/SDL2_image.lib  
        )
    else()
        set_target_properties(SDL2_image PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.lib  
        )
    endif()
elseif (LINUX)
endif()

add_library(SDL2_image_libjpeg  SHARED IMPORTED)
add_library(SDL2_image_libpng   SHARED IMPORTED)
add_library(SDL2_image_libtiff  SHARED IMPORTED)
add_library(SDL2_image_libwebp  SHARED IMPORTED)
add_library(SDL2_image_zlib     SHARED IMPORTED)
if (WIN32)
    if (${BUILD_X64})
        set_target_properties(SDL2_image_libjpeg    PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/libjpeg-9.dll)
        set_target_properties(SDL2_image_libpng     PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/libpng16-16.dll)
        set_target_properties(SDL2_image_libtiff    PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/libtiff-5.dll)
        set_target_properties(SDL2_image_libwebp    PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/libwebp-7.dll)
        set_target_properties(SDL2_image_zlib       PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x64/zlib1.dll)
    else()
        set_target_properties(SDL2_image_libjpeg    PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x86/libjpeg-9.dll)
        set_target_properties(SDL2_image_libpng     PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x86/libpng16-16.dll)
        set_target_properties(SDL2_image_libtiff    PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x86/libtiff-5.dll)
        set_target_properties(SDL2_image_libwebp    PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x86/libwebp-7.dll)
        set_target_properties(SDL2_image_zlib       PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/x86/zlib1.dll)
    endif()
elseif (LINUX)
endif()

target_link_libraries(SDL2_image
    INTERFACE SDL2
    INTERFACE SDL2_image_libjpeg
    INTERFACE SDL2_image_libpng 
    INTERFACE SDL2_image_libtiff
    INTERFACE SDL2_image_libwebp
    INTERFACE SDL2_image_zlib   
)
target_include_directories(SDL2_image INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)
