enable_testing()
include(GoogleTest)

add_executable(apptest EXCLUDE_FROM_ALL
    test_main.cpp
    test_config.cpp
    test_db.cpp
    common/test_fraction.cpp
    common/test_chartformat_bms.cpp
    game/test_graphics.cpp
 "game/test_lr2skin.cpp")
target_link_libraries(apptest PUBLIC
    GTest::gtest GTest::gmock)

gtest_discover_tests(apptest)

set_target_properties(apptest PROPERTIES
    CXX_STANDARD 17
    CTEST_OUTPUT_ON_FAILURE TRUE 
    GTEST_COLOR TRUE
)
    
target_link_libraries(apptest 
    PUBLIC gamelib
    PUBLIC plog
)
target_include_directories(apptest PRIVATE
    ${PROJECT_INCLUDE_DIR}
)

add_custom_command(TARGET apptest POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:fmod>
    $<$<BOOL:${USE_BUNDLED_SQLITE3}>:$<TARGET_FILE:SQLite3>>
    $<TARGET_FILE_DIR:apptest>
)

add_custom_command(TARGET apptest PRE_LINK
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_CURRENT_SOURCE_DIR}/test"
    "$<TARGET_FILE_DIR:apptest>/test"
)

if (WIN32)
    if (BUILD_X64)
        set(MINGW_LIB_DIR ${CMAKE_SOURCE_DIR}/ext/mingw/lib/x64)
    else()
        set(MINGW_LIB_DIR ${CMAKE_SOURCE_DIR}/ext/mingw/lib/x86)
    endif()
    file(GLOB MINGW_LIB_DLLS ${MINGW_LIB_DIR}/*.dll)
    add_custom_command(TARGET apptest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MINGW_LIB_DLLS}
        $<TARGET_FILE_DIR:apptest>
    )
endif()

if (${GRAPHICS_BACKEND} STREQUAL SDL2 AND WIN32)
    add_custom_command(TARGET apptest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE:SDL2_image>
        $<TARGET_FILE:SDL2_image::libjpeg>
        $<TARGET_FILE:SDL2_image::libpng>
        $<TARGET_FILE:SDL2_image::libtiff>
        $<TARGET_FILE:SDL2_image::libwebp>
        $<TARGET_FILE:SDL2_ttf>
        $<TARGET_FILE:SDL2_ttf::libfreetype>
        $<TARGET_FILE:SDL2_ttf::zlib>
        $<TARGET_FILE_DIR:apptest>
    )
endif()

if (${VIDEO_ENABLED} AND WIN32)
    add_custom_command(TARGET apptest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:libavcodec>
        $<TARGET_FILE:libavdevice>
        $<TARGET_FILE:libavfilter>
        $<TARGET_FILE:libavformat>
        $<TARGET_FILE:libswresample>
        $<TARGET_FILE:libavutil>
        $<TARGET_FILE:libswscale>
        $<TARGET_FILE_DIR:apptest>
    )
endif()
