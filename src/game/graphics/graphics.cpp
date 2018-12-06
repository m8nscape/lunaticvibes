#include "graphics.h"

#if __has_include("SDL2/graphics_SDL2.h")
#include "SDL2/window_SDL2.h"
int graphics_init()
{
    return initLib_SDL2();
}

void graphics_clear()
{
    return clear_SDL2();
}

void graphics_flush()
{
    return flush_SDL2();
}

int graphics_free()
{
    return freeLib_SDL2();
}

void event_handle()
{
    return handle_event_SDL2();
}
#endif
