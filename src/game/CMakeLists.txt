
if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    set(GRAPHICS_BACKEND_SRC
        graphics/SDL2/common_SDL2.cpp
        graphics/SDL2/input.cpp
        graphics/SDL2/rect_SDL2.cpp
        graphics/SDL2/ttf_SDL2.cpp
        graphics/SDL2/window_SDL2.cpp
    )
endif()

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/git_version.cpp
           ${CMAKE_BINARY_DIR}/_dummy.cpp
    COMMAND ${CMAKE_COMMAND} -P 
            ${CMAKE_CURRENT_SOURCE_DIR}/git_version.cmake
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_library(gamelib
    ${CMAKE_BINARY_DIR}/git_version.cpp
    runtime/state.cpp
    chart/chart.cpp
    chart/chart_bms.cpp
    ${GRAPHICS_BACKEND_SRC}
    graphics/sprite.cpp
    graphics/sprite_bar_entry.cpp
    graphics/sprite_graph.cpp
    graphics/sprite_imagetext.cpp
    graphics/sprite_lane.cpp
    graphics/sprite_video.cpp
    graphics/texture_extra.cpp
    graphics/video.cpp
    graphics/dxa.cpp
    input/input_mgr.cpp
    input/input_mgr_sdl.cpp
    input/input_dinput8.cpp
    input/input_windows.cpp
    input/input_wrapper.cpp
    ruleset/ruleset_bms.cpp
    ruleset/ruleset_bms_auto.cpp
    ruleset/ruleset_bms_replay.cpp
    ruleset/ruleset_network.cpp
    ruleset/ruleset_bms_network.cpp
    scene/scene.cpp
    scene/scene_context.cpp
    scene/scene_decide.cpp
    scene/scene_mgr.cpp
    scene/scene_play.cpp
    scene/scene_pre_select.cpp
    scene/scene_result.cpp
    scene/scene_course_result.cpp
    scene/scene_select.cpp
    scene/scene_keyconfig.cpp
    scene/scene_play_course_trans.cpp
    scene/scene_customize.cpp
    scene/scene_select_imgui.cpp
    scene/scene_play_retry_trans.cpp
    scene/scene_exit_trans.cpp
    scene/scene_play_imgui.cpp
    skin/skin.cpp
    skin/skin_lr2.cpp
    skin/skin_lr2_button_callbacks.cpp
    skin/skin_lr2_slider_callbacks.cpp
    skin/skin_lr2_converters.cpp
    skin/skin_lr2_dst.cpp
    skin/skin_lr2_debug.cpp
    skin/skin_mgr.cpp
    sound/sound_fmod.cpp
    sound/sound_fmod_callback.cpp
    sound/sound_mgr.cpp
    sound/sound_sample.cpp  
    sound/soundset.cpp
    sound/soundset_lr2.cpp
    replay/replay_chart.cpp
    runtime/i18n.cpp 
    arena/arena_data.cpp
    arena/arena_internal.cpp
    arena/arena_host.cpp
    arena/arena_client.cpp
)

if (UNIX OR MINGW)
    target_link_libraries(gamelib PRIVATE -lpthread)
    target_link_libraries(gamelib PRIVATE -lstdc++fs)
    target_link_libraries(gamelib PRIVATE -lcrypto)
    target_link_libraries(gamelib PRIVATE -lssl)
    target_link_libraries(gamelib PRIVATE -lcurl)
elseif (MSVC)
    target_link_libraries(gamelib PRIVATE libcrypto.lib)
    target_link_libraries(gamelib PRIVATE libssl.lib)
    target_link_libraries(gamelib PRIVATE libcurl.lib)
endif()

target_include_directories(gamelib PUBLIC 
    ${PROJECT_INCLUDE_DIR}
)

set_target_properties(gamelib PROPERTIES
    CXX_STANDARD 17
)


target_link_libraries(gamelib 
    PUBLIC common
    PUBLIC config
    PUBLIC db
    PUBLIC fmod
    PUBLIC imgui
    PUBLIC re2
    PUBLIC cereal
)

if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    target_link_libraries(gamelib
        PUBLIC SDL2
        PUBLIC SDL2_main
        PUBLIC SDL2_gfx
        PUBLIC SDL2_ttf
        PUBLIC SDL2_image
    )
endif()

if (${VIDEO_ENABLED})
    target_link_libraries(gamelib
        PUBLIC libavcodec    
        PUBLIC libavdevice   
        PUBLIC libavfilter   
        PUBLIC libavformat   
        PUBLIC libswresample 
        PUBLIC libavutil     
        PUBLIC libswscale   
    )
else()
    target_compile_definitions(gamelib PUBLIC -DVIDEO_DISABLED)
endif()

target_precompile_headers(gamelib REUSE_FROM common)

#############################################################

if (WIN32)
    add_executable(LunaticVibes game.cpp LunaticVibes.exe.manifest)
else()
    add_executable(LunaticVibes game.cpp)
endif()
target_link_libraries(LunaticVibes PRIVATE gamelib)
set_target_properties(LunaticVibes PROPERTIES
    CXX_STANDARD 17
)

add_custom_command(TARGET LunaticVibes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:fmod>
    $<$<BOOL:${USE_BUNDLED_SQLITE3}>:$<TARGET_FILE:SQLite3>>
    $<TARGET_FILE_DIR:LunaticVibes>
)

add_custom_command(TARGET LunaticVibes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res
    $<TARGET_FILE_DIR:LunaticVibes>/gamedata/resources
)

add_custom_command(TARGET LunaticVibes POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/LICENSE_3RD_PARTY
    $<TARGET_FILE_DIR:LunaticVibes>/gamedata/resources/LICENSE_3RD_PARTY
)

if (WIN32)
    if (BUILD_X64)
        set(MINGW_LIB_DIR ${CMAKE_SOURCE_DIR}/ext/mingw/lib/x64)
    else()
        set(MINGW_LIB_DIR ${CMAKE_SOURCE_DIR}/ext/mingw/lib/x86)
    endif()
    file(GLOB MINGW_LIB_DLLS ${MINGW_LIB_DIR}/*.dll)
    add_custom_command(TARGET LunaticVibes POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${MINGW_LIB_DLLS}
        $<TARGET_FILE_DIR:LunaticVibes>
    )
endif()

if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    add_custom_command(TARGET LunaticVibes POST_BUILD
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
        $<TARGET_FILE_DIR:LunaticVibes>
    )
endif()

if (${VIDEO_ENABLED})
    add_custom_command(TARGET LunaticVibes POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:libavcodec>
        $<TARGET_FILE:libavdevice>
        $<TARGET_FILE:libavfilter>
        $<TARGET_FILE:libavformat>
        $<TARGET_FILE:libswresample>
        $<TARGET_FILE:libavutil>
        $<TARGET_FILE:libswscale>
        $<TARGET_FILE_DIR:LunaticVibes>
    )
endif()