#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_ttf.h"
#include "graphics_SDL2.h"
#include "window_SDL2.h"
#include "defs/defs_general.h"
#include <plog/Log.h>
#include <string>

int initLib_SDL2()
{
    // SDL2
    {
        if (SDL_Init(SDL_INIT_VIDEO))
        {
            LOG_ERROR << "[SDL2] Library init ERROR! " << SDL_GetError();
            return -99;
        }
        LOG_INFO << "[SDL2] Library version " << SDL_MAJOR_VERSION << '.' << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL;
        
        std::string title;
        title += MAIN_NAME;
        title += ' ';
        title += SUB_NAME;
        title += ' ';
        title += std::to_string(VER_MAJOR);
        title += '.';
        title += std::to_string(VER_MINOR);
        if (VER_PATCH)
        {
            title += '.';
            title += std::to_string(VER_PATCH);
        }

        _frame_window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            CANVAS_WIDTH, CANVAS_HEIGHT,
            SDL_WINDOW_OPENGL);
        if (!_frame_window)
        {
            LOG_ERROR << "[SDL2] Init window ERROR! " << SDL_GetError();
            return -1;
        }

        _frame_renderer = SDL_CreateRenderer(
            _frame_window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        if (!_frame_renderer)
        {
            LOG_ERROR << "[SDL2] Init renderer ERROR! " << SDL_GetError();
            return -2;
        }
        LOG_DEBUG << "[SDL2] Initializing window and render complete.";
    }

    // SDL_Image
    {
        auto flags = IMG_INIT_JPG | IMG_INIT_PNG;
        if (flags != IMG_Init(flags))
        {
            // error handling
            LOG_ERROR << "[SDL2] Image module init failed. " << IMG_GetError();
            return 1;
        }
        LOG_INFO << "[SDL2] Image module version " << SDL_IMAGE_MAJOR_VERSION << '.' << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_PATCHLEVEL;
    }
    // SDL_ttf
    {
        if (-1 == TTF_Init())
        {
            // error handling
            LOG_ERROR << "[SDL2] TTF module init failed. " << TTF_GetError();
            return 2;
        }
        LOG_INFO << "[SDL2] TTF module version " << SDL_TTF_MAJOR_VERSION << '.' << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_PATCHLEVEL;
    }

    return 0;
}

void clear_SDL2()
{
    SDL_RenderClear(_frame_renderer);
}

void flush_SDL2()
{
    SDL_RenderPresent(_frame_renderer);
}

int freeLib_SDL2()
{
    SDL_DestroyRenderer(_frame_renderer);
    SDL_DestroyWindow(_frame_window);
    SDL_Quit();
    TTF_Quit();
    IMG_Quit();

    return 0;
}

void handle_event_SDL2()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ;
    }
}