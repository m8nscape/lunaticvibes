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
        set(EXT_DEPEND_LIBFILES ${EXT_DEPEND_LIBFILES}
            "${CMAKE_CURRENT_LIST_DIR}/lib/x64/libjpeg-9.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x64/libpng16-16.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x64/libtiff-5.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x64/libwebp-7.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x64/zlib1.dll"
             CACHE INTERNAL "dependencies"
            )
        set(EXT_DEPEND_LIBFILES_SET TRUE CACHE INTERNAL "dependencies")
    else()
        set_target_properties(SDL2_image PROPERTIES
            IMPORTED_LOCATION   ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.dll
            IMPORTED_IMPLIB     ${CMAKE_CURRENT_LIST_DIR}/lib/x86/SDL2_image.lib  
        )
        set(EXT_DEPEND_LIBFILES ${EXT_DEPEND_LIBFILES}
            "${CMAKE_CURRENT_LIST_DIR}/lib/x86/libjpeg-9.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x86/libpng16-16.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x86/libtiff-5.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x86/libwebp-7.dll"
            "${CMAKE_CURRENT_LIST_DIR}/lib/x86/zlib1.dll"
             CACHE INTERNAL "dependencies"
            )
        set(EXT_DEPEND_LIBFILES_SET TRUE CACHE INTERNAL "dependencies")
    endif()
elseif (LINUX)
endif()

target_include_directories(SDL2_image INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

get_target_property(SDL2IMAGE_LIB_FILES SDL2_image IMPORTED_LOCATION)
set(EXT_DEPEND_LIBFILES ${EXT_DEPEND_LIBFILES} 
        ${SDL2IMAGE_LIB_FILES}
        CACHE INTERNAL "dependencies"
)
set(EXT_DEPEND_LIBFILES_SET TRUE CACHE INTERNAL "dependencies")
