#ifdef RENDER_SDL2

#include "meta.h"
#include "window_SDL2.h"
#include "graphics_SDL2.h"
#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_ttf.h"
#include "game/graphics/video.h"
#include <plog/Log.h>
#include <string>
#include "config/config_mgr.h"

int graphics_init()
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

        auto mode = ConfigMgr::G.get(cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
        if (mode == cfg::V_WINMODE_BORDERLESS)
        {
            _frame_window = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                ConfigMgr::G.get(cfg::V_RES_X, CANVAS_WIDTH), ConfigMgr::G.get(cfg::V_RES_Y, CANVAS_HEIGHT),
                SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        }
        else if (mode == cfg::V_WINMODE_FULL)
        {
            _frame_window = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                ConfigMgr::G.get(cfg::V_FULL_RES_X, CANVAS_WIDTH), ConfigMgr::G.get(cfg::V_FULL_RES_Y, CANVAS_HEIGHT),
                SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
        }
        else // fallback to windowed
        {
            _frame_window = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                ConfigMgr::G.get(cfg::V_RES_X, CANVAS_WIDTH), ConfigMgr::G.get(cfg::V_RES_Y, CANVAS_HEIGHT),
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        }
        if (!_frame_window)
        {
            LOG_ERROR << "[SDL2] Init window ERROR! " << SDL_GetError();
            return -1;
        }

        auto vsync = ConfigMgr::G.get(cfg::V_VSYNC, cfg::OFF);
        if (vsync == cfg::ON)
        {
            _frame_renderer = SDL_CreateRenderer(
                _frame_window, -1,
                SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        }
        else
        {
            _frame_renderer = SDL_CreateRenderer(
                _frame_window, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        }
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

	// libav
	video_init();

    return 0;
}

void graphics_clear()
{
    SDL_RenderClear(_frame_renderer);
}

void graphics_flush()
{
    SDL_RenderPresent(_frame_renderer);
}

int graphics_free()
{
    SDL_DestroyRenderer(_frame_renderer);
    SDL_DestroyWindow(_frame_window);
    TTF_Quit();
    IMG_Quit();
	SDL_Quit();

    return 0;
}

extern bool __event_quit;
void event_handle()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            __event_quit = true;
            break;

        default:
            break;
        }
    }
}

#endif