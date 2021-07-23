#ifdef RENDER_SDL2

#include "common/meta.h"
#include "window_SDL2.h"
#include "graphics_SDL2.h"
#include "SDL.h"
#include "SDL_Image.h"
#include "SDL_ttf.h"
#include "SDL_syswm.h"
#include "game/graphics/video.h"
#include "common/log.h"
#include <string>
#include "config/config_mgr.h"
#include "sysutil.h"

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

        /*
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
        {
            SDL_RendererInfo rendererInfo = {};
            SDL_GetRenderDriverInfo(i, &rendererInfo);
            LOG_INFO << rendererInfo.name;
        }
        */
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

        auto mode = ConfigMgr::get("V", cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
        if (mode == cfg::V_WINMODE_BORDERLESS)
        {
            gFrameWindow = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                ConfigMgr::get("V", cfg::V_RES_X, CANVAS_WIDTH), ConfigMgr::get("V", cfg::V_RES_Y, CANVAS_HEIGHT),
                SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        }
        else if (mode == cfg::V_WINMODE_FULL)
        {
            gFrameWindow = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                ConfigMgr::get("V", cfg::V_FULL_RES_X, CANVAS_WIDTH), ConfigMgr::get("V", cfg::V_FULL_RES_Y, CANVAS_HEIGHT),
                SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
        }
        else // fallback to windowed
        {
            gFrameWindow = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                ConfigMgr::get("V", cfg::V_RES_X, CANVAS_WIDTH), ConfigMgr::get("V", cfg::V_RES_Y, CANVAS_HEIGHT),
                SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        }
        if (!gFrameWindow)
        {
            LOG_ERROR << "[SDL2] Init window ERROR! " << SDL_GetError();
            return -1;
        }

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(gFrameWindow, &wmInfo);

#if _WIN32 || _WIN64
        setWindowHandle((void*)&wmInfo.info.win.window);
#endif

        auto vsync = ConfigMgr::get("V", cfg::V_VSYNC, cfg::OFF);
        if (vsync == cfg::ON)
        {
            gFrameRenderer = SDL_CreateRenderer(
                gFrameWindow, -1,
                SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        }
        else
        {
            gFrameRenderer = SDL_CreateRenderer(
                gFrameWindow, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        }
        if (!gFrameRenderer)
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
    SDL_RenderClear(gFrameRenderer);
}

void graphics_flush()
{
    SDL_RenderPresent(gFrameRenderer);
}

int graphics_free()
{
    SDL_DestroyRenderer(gFrameRenderer);
    gFrameRenderer = nullptr;
    SDL_DestroyWindow(gFrameWindow);
    gFrameWindow = nullptr;
    TTF_Quit();
    IMG_Quit();
	SDL_Quit();

    return 0;
}

extern bool gEventQuit;
void event_handle()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            gEventQuit = true;
            break;

        default:
            break;
        }
    }
}

#endif