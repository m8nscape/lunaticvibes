enable_language (C)
#enable_language (ASM)

add_library(SDL2_gfx STATIC
    src/SDL2_framerate.c
    src/SDL2_gfxPrimitives.c
    src/SDL2_imageFilter.c
    src/SDL2_rotozoom.c
    #src/SDL2_gfxMMX.asm
)

target_include_directories(SDL2_gfx PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(SDL2_gfx PUBLIC
    SDL2::SDL2
)