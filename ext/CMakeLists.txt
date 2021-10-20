set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR})

# Add fmod
add_subdirectory(fmod)

# Add libav
if (${VIDEO_ENABLED})
    find_package(libavcodec QUIET)
    if (NOT libavcodec_FOUND)
        add_subdirectory(libav)
    endif()
endif()

# Add plog
set(PLOG_BUILD_SAMPLES OFF CACHE INTERNAL "Build plog's samples.")
add_subdirectory(plog)

# Add yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "Enable parse tools")
add_subdirectory(yaml-cpp)

# Add sqlite3 if needed
find_package(SQLite3 CONFIG QUIET)
if (SQLite3_FOUND)
    set(sqlite3_LIBRARY SQLite::SQLite3)
else()
    find_package(unofficial-sqlite3 CONFIG QUIET)
    if (unofficial-sqlite3_FOUND)
        set(sqlite3_LIBRARY unofficial::sqlite3::sqlite3)
    else()
        add_subdirectory(sqlite3)
        if (${USE_BUNDLED_SQLITE3})
            set(sqlite3_LIBRARY SQLite3)
        else()
            set(sqlite3_LIBRARY -lsqlite3)
        endif()
    endif()
endif()
set (sqlite3_LIBRARY ${sqlite3_LIBRARY} CACHE INTERNAL "sqlite3 target name")

# Add SDL2
if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    # Add SDL2
    find_package(SDL2 REQUIRED)

    # Add SDL2_image
    find_package(SDL2_Image QUIET)
    if (NOT SDL2_Image_FOUND)
        add_subdirectory(SDL2_Image)
    endif()

    # Add SDL2_ttf
    find_package(SDL2_ttf QUIET)
    if (NOT SDL2_ttf_FOUND)
        add_subdirectory(SDL2_ttf)
    endif()

    # Add SDL2_gfx
    add_subdirectory(SDL2_gfx)
endif()
