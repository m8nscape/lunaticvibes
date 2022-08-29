
if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    set(GRAPHICS_BACKEND_SRC
        graphics/SDL2/common_SDL2.cpp
        graphics/SDL2/rect_SDL2.cpp
        graphics/SDL2/ttf_SDL2.cpp
        graphics/SDL2/window_SDL2.cpp
    )
endif()

add_library(gamelib
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
    input/input_linux.cpp
    input/input_mgr.cpp
    input/input_dinput8.cpp
    input/input_windows.cpp
    input/input_wrapper.cpp
    ruleset/ruleset_bms.cpp
    ruleset/ruleset_bms_auto.cpp
    scene/scene.cpp
    scene/scene_context.cpp
    scene/scene_decide.cpp
    scene/scene_mgr.cpp
    scene/scene_play.cpp
    scene/scene_pre_select.cpp
    scene/scene_result.cpp
    scene/scene_select.cpp
    scene/scene_keyconfig.cpp
    scene/scene_play_course_trans.cpp
    scene/scene_customize.cpp
    scene/scene_select_imgui.cpp
    scene/scene_play_retry_trans.cpp
    scene/scene_exit_trans.cpp
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
)

if (UNIX OR MINGW)
    target_link_libraries(gamelib PRIVATE -lpthread)
    target_link_libraries(gamelib PRIVATE -lstdc++fs)
    target_link_libraries(gamelib PRIVATE -lcrypto)
    target_link_libraries(gamelib PRIVATE -lssl)
elseif (MSVC)
    target_link_libraries(gamelib PRIVATE libcrypto.lib)
    target_link_libraries(gamelib PRIVATE libssl.lib)
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

add_executable(game game.cpp)
target_link_libraries(game PRIVATE gamelib)
set_target_properties(game PROPERTIES
    CXX_STANDARD 17
)
if (${EXT_DEPEND_LIBFILES_SET})
    add_custom_command(TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EXT_DEPEND_LIBFILES}
        $<TARGET_FILE_DIR:game>
    )
endif()
