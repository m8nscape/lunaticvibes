
if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    set(GRAPHICS_BACKEND_SRC
        graphics/SDL2/common_SDL2.cpp
        graphics/SDL2/rect_SDL2.cpp
        graphics/SDL2/ttf_SDL2.cpp
        graphics/SDL2/window_SDL2.cpp
    )
endif()

add_library(game STATIC
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
    input/input_windows.cpp
    input/input_wrapper.cpp
    ruleset/ruleset_classic.cpp
    scene/scene.cpp
    scene/scene_context.cpp
    scene/scene_decide.cpp
    scene/scene_mgr.cpp
    scene/scene_play.cpp
    scene/scene_result.cpp
    scene/scene_select.cpp
    skin/skin.cpp
    skin/skin_lr2.cpp
    skin/skin_lr2_button_callbacks.cpp
    skin/skin_lr2_dst.cpp
    skin/skin_mgr.cpp
    sound/sound_fmod.cpp
    sound/sound_fmod_callback.cpp
    sound/sound_mgr.cpp
    sound/sound_sample.cpp
)

target_include_directories(game PRIVATE 
    ${PROJECT_INCLUDE_DIR}
)

set_target_properties(game PROPERTIES
    CXX_STANDARD 17)

target_link_libraries(game 
    PUBLIC common
    PUBLIC config
    PUBLIC db
    PUBLIC fmod
)

if (${GRAPHICS_BACKEND} STREQUAL SDL2)
    target_link_libraries(game
        PUBLIC SDL2
        PUBLIC SDL2_main
        PUBLIC SDL2_gfx
        PUBLIC SDL2_ttf
        PUBLIC SDL2_image
    )
endif()

if (${VIDEO_ENABLED})
    target_link_libraries(game
        PUBLIC libavcodec    
        PUBLIC libavdevice   
        PUBLIC libavfilter   
        PUBLIC libavformat   
        PUBLIC libavresample 
        PUBLIC libavutil     
        PUBLIC libswscale   
    )
else()
    add_definitions(-DVIDEO_DISABLED)
endif()

target_precompile_headers(game REUSE_FROM common)