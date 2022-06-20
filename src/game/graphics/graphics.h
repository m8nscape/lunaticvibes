#pragma once

// This header intends to provide a Render Layer Selection function,
// Which layer is used is decided by pre-defined macro.
// e.g.
//      #ifdef SDL2
//          #include "graphics_SDL2.h"
//      #elif defined SFML
//          #include "graphics_SFML.h"
//      #elif defined OGLNative
//          #include "graphics_OpenGL.h"
//      #endif

#ifdef RENDER_SDL2
#include "SDL2/graphics_SDL2.h"
#endif

int graphics_init();
void graphics_clear();
void graphics_flush();
int graphics_free();

// 0: windowed / 1: fullscreen / 2: borderless
void graphics_change_window_mode(int mode);

void graphics_change_vsync(bool enable);

void event_handle();

void ImGuiNewFrame();