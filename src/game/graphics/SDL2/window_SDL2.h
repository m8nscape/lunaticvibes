#pragma once
#include "SDL_render.h"

extern SDL_Window* _frame_window;
extern SDL_Renderer* _frame_renderer;

int initLib_SDL2();
void clear_SDL2();
void flush_SDL2();
int freeLib_SDL2();