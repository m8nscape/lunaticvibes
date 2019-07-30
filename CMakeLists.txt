cmake_minimum_required(VERSION 3.12)

project(banshao VERSION 0.1
                DESCRIPTION "asdf"
                LANGUAGES CXX)

# settings
set(RENDER_LIB SDL2)
set(SOUND_LIB FMOD)

# _debug: build with debug info
string(TOLOWER ${CMAKE_BUILD_TYPE} buildtype)
if(buildtype MATCHES "debug")
    set(_debug YES)
endif()

# _windows/linux/osx: build platform
if (WIN32 OR MINGW OR MSYS)
    set(_windows TRUE)
elseif (UNIX AND NOT APPLE)
    set(_linux TRUE)
elseif (APPLE)
    set(_osx TRUE)
endif()

# _x86/x64: build architecture
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_x64 YES)
endif ()

set(module_list "")
set(predef_list "")

                
# ext: digestpp
add_library(digestpp INTERFACE)
include_directories(${CMAKE_SOURCE_DIR}/ext/digestpp)
target_include_directories(digestpp INTERFACE ext/digestpp)
target_compile_definitions(digestpp INTERFACE NOMINMAX)
list(APPEND module_list "digestpp")

# ext: FMOD
if (SOUND_LIB STREQUAL FMOD)
    set(fmod_lib_name "fmod")
    if (_debug)
        string(APPEND fmod_lib_name "L")
    endif(_debug)
    if (_windows)
        if (_x64)
            string(APPEND fmod_lib_name "64")
        endif (_x64)
        string(APPEND fmod_lib_name "_vc")
    endif(_windows)
    
    list(APPEND module_list ${fmod_lib_name})
    include_directories(${CMAKE_SOURCE_DIR}/ext/fmod/inc)
    link_directories(${CMAKE_SOURCE_DIR}/ext/fmod/lib)
endif()

# ext: SDL2
if (RENDER_LIB STREQUAL SDL2)
    list(APPEND predef_list "RENDER_SDL2")
    list(APPEND module_list "SDL2" "SDL2main" "SDL2_gfx" "SDL2_image" "SDL2_ttf")
    if (_debug)
        list(APPEND module_list "SDL2test")
    endif (_debug)
    
    include_directories(${CMAKE_SOURCE_DIR}/ext/SDL2/include
                        ${CMAKE_SOURCE_DIR}/ext/SDL2_gfx/include
                        ${CMAKE_SOURCE_DIR}/ext/SDL2_image/include
                        ${CMAKE_SOURCE_DIR}/ext/SDL2_ttf/include)
    
    if (_x64)
        link_directories(${CMAKE_SOURCE_DIR}/ext/SDL2/lib/x64
                         ${CMAKE_SOURCE_DIR}/ext/SDL2_gfx/lib/x64
                         ${CMAKE_SOURCE_DIR}/ext/SDL2_image/lib/x64
                         ${CMAKE_SOURCE_DIR}/ext/SDL2_ttf/lib/x64)
    else ()
        link_directories(${CMAKE_SOURCE_DIR}/ext/SDL2/lib/x86
                         ${CMAKE_SOURCE_DIR}/ext/SDL2_gfx/lib/x86
                         ${CMAKE_SOURCE_DIR}/ext/SDL2_image/lib/x86
                         ${CMAKE_SOURCE_DIR}/ext/SDL2_ttf/lib/x86)
    endif (_x64)
endif()

# ext: yaml
add_subdirectory(ext/yaml-cpp)
include_directories(${CMAKE_SOURCE_DIR}/ext/yaml-cpp/include)
set(YAML_CPP_INSTALL OFF)
list(APPEND module_list "yaml-cpp")

# ext: plog
add_subdirectory(ext/plog)
include_directories(${CMAKE_SOURCE_DIR}/ext/plog/include)
list(APPEND module_list "plog")

# sources
include_directories(src)
include_directories(src/common)
file(GLOB_RECURSE src_common    src/common/*.cpp)
file(GLOB         src_config    src/config/*.cpp)
file(GLOB_RECURSE src_db        src/db/*.cpp)
file(GLOB_RECURSE src_game      src/game/*.cpp)
file(GLOB_RECURSE src_network   src/network/*.cpp)
message(STATUS ${src_common})
message(STATUS ${src_config})
message(STATUS ${src_game})
add_executable(banshao ${src_common} ${src_config} ${src_db} ${src_game} ${src_network})
target_compile_features(banshao PUBLIC cxx_std_17)
set_target_properties(banshao PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(banshao ${module_list})
target_compile_definitions(banshao PUBLIC ${predef_list})

# test
#enable_testing()
include("GoogleTest")
add_subdirectory(${CMAKE_SOURCE_DIR}/ext/googletest)
set(INSTALL_GTEST OFF)

aux_source_directory(src/test src_test)

add_executable(banshao-tests ${src_test})
cmake_policy(SET CMP0079 NEW)
target_link_libraries(banshao-tests gtest gmock)
gtest_discover_tests(banshao-tests)