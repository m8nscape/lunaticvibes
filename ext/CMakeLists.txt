list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
list(APPEND CMAKE_LIBRARY_PATH ${CMAKE_CURRENT_LIST_DIR})

# Add fmod
add_subdirectory(fmod)

# Add ffmpeg
if (${VIDEO_ENABLED})
    find_library(libavcodec NAMES avcodec GLOBAL QUIET)
    if (libavcodec)
        find_library(libavdevice NAMES avdevice REQUIRED GLOBAL)
        find_library(libavfilter NAMES avfilter REQUIRED GLOBAL)
        find_library(libavformat NAMES avformat REQUIRED GLOBAL)
        find_library(libswresample NAMES swresample REQUIRED GLOBAL)
        find_library(libavutil NAMES avutil REQUIRED GLOBAL)
        find_library(libswscale NAMES swscale REQUIRED GLOBAL)

        set(
            FFMPEG_LIBS
            ${libavcodec}
            ${libavdevice}
            ${libavfilter}
            ${libavformat}
            ${libswresample}
            ${libavutil}
            ${libswscale}

            PARENT_SCOPE
        )
    else()
        add_subdirectory(FFmpeg)
        set(
            FFMPEG_LIBS
            libavcodec
            libavdevice
            libavfilter
            libavformat
            libswresample
            libavutil
            libswscale

            PARENT_SCOPE
        )
    endif()
endif()

# Add plog
set(PLOG_BUILD_SAMPLES OFF CACHE INTERNAL "Build plog's samples.")
add_subdirectory(plog)

# Add yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "Enable parse tools")
add_subdirectory(yaml-cpp)

# Add sqlite3 if needed
set(USE_BUNDLED_SQLITE3 FALSE CACHE BOOL "USE_BUNDLED_SQLITE3" FORCE)
if (${CMAKE_VERSION} VERSION_GREATER "3.24")
    find_package(SQLite3 QUIET GLOBAL)
    if (SQLite3_FOUND)
        set(sqlite3_LIBRARY SQLite::SQLite3)
    else()
        find_package(unofficial-sqlite3 CONFIG QUIET)
        if (unofficial-sqlite3_FOUND)
            set(sqlite3_LIBRARY unofficial::sqlite3::sqlite3)
        else()
            add_subdirectory(sqlite3)
            set(sqlite3_LIBRARY SQLite3)
        endif()
    endif()
else()
    # Lower cmake versions could not export the imported target
    add_subdirectory(sqlite3)
    set(sqlite3_LIBRARY SQLite3)
endif()
set (sqlite3_LIBRARY ${sqlite3_LIBRARY} CACHE INTERNAL "sqlite3 target name")

# Add SDL2
if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    # Add SDL2
    find_package(SDL2 QUIET)
    if (NOT SDL2_FOUND)
        add_subdirectory(SDL2)
    endif()

    # Add SDL2_image
    find_package(SDL2_image QUIET)
    if (NOT SDL2_image_FOUND)
        add_subdirectory(SDL2_image)
    endif()

    # Add SDL2_ttf
    find_package(SDL2_ttf QUIET)
    if (NOT SDL2_ttf_FOUND)
        add_subdirectory(SDL2_ttf)
    endif()

    # Add SDL2_gfx
    add_subdirectory(SDL2_gfx)
endif()

# Add gtest
option(INSTALL_GTEST OFF)
add_subdirectory(googletest)

# Add Dear ImGui
add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)
if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    target_link_libraries(imgui PRIVATE SDL2::SDL2)
    target_sources(imgui PRIVATE
        imgui/backends/imgui_impl_sdl2.cpp
        imgui/backends/imgui_impl_sdlrenderer.cpp
    )
endif()
target_include_directories(imgui PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/imgui)


# Add google/re2
option(RE2_BUILD_TESTING "enable testing for RE2" OFF)
add_subdirectory(re2)

# Add taocpp/json
add_subdirectory(json)

# Add cereal
option(BUILD_DOC "Build documentation" OFF)
option(BUILD_SANDBOX "Build sandbox examples" OFF)
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance sandbox comparison (requires boost)" ON)
add_subdirectory(cereal)
