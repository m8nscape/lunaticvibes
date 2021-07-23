enable_language (C ASM)

add_library(sdl2_gfx STATIC
    src/SDL2_framerate.c
    src/SDL2_gfxPrimitives.c
    src/SDL2_imageFilter.c
    src/SDL2_rotozoom.c
    src/SDL2_gfxMMX.asm
)

target_include_directories(sdl2_gfx PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(sdl2_gfx PUBLIC
    SDL2
)