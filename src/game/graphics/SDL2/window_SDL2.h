#pragma once
#include "SDL_render.h"

// global control pointers, do not modify
inline SDL_Window* _frame_window;
inline SDL_Renderer* _frame_renderer;

int initLib_SDL2();
void clear_SDL2();
void flush_SDL2();
int freeLib_SDL2();