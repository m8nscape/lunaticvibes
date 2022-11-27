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

#include <vector>
#include <utility>

int graphics_init();
void graphics_clear();
void graphics_flush();
int graphics_free();

void graphics_copy_screen_texture(Texture& texture);

// get monitor index where the window locates
int graphics_get_monitor_index();

// w, h
std::pair<int, int> graphics_get_desktop_resolution();

// w, h, hz
std::vector<std::tuple<int, int, int>> graphics_get_resolution_list();

// 0: windowed / 1: fullscreen / 2: borderless
void graphics_change_window_mode(int mode);

void graphics_resize_window(int x, int y);

// 0: off / 1: vsync (double buffer) / 2: adaptive vsync
void graphics_change_vsync(int mode);


// scaling functions
void graphics_set_supersample_level(int scale);
int  graphics_get_supersample_level();

void graphics_resize_canvas(int x, int y);
double graphics_get_canvas_scale_x();
double graphics_get_canvas_scale_y();


void graphics_set_maxfps(int fps);

void event_handle();

void ImGuiNewFrame();


// text input support
void startTextInput(const Rect& textBox, const std::string& oldText, std::function<void(const std::string&)> funUpdateText);
void stopTextInput();


void graphics_screenshot(const Path& png);