#ifdef RENDER_SDL2

#include "common/meta.h"
#include "window_SDL2.h"
#include "graphics_SDL2.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "SDL_syswm.h"
#include "game/graphics/video.h"
#include "common/log.h"
#include <climits>
#include <cstdint>
#include <string>
#include "config/config_mgr.h"
#include "common/sysutil.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include <game/graphics/SDL2/input.h>

static SDL_Rect canvasRect;
static SDL_Rect windowRect;
static double canvasScaleX = 1.0;
static double canvasScaleY = 1.0;

int graphics_init()
{
    LOG_INFO << "[SDL2] Initializing...";

    // SDL2
    {
        if (SDL_Init(SDL_INIT_VIDEO))
        {
            LOG_FATAL << "[SDL2] Library init ERROR! " << SDL_GetError();
            return -99;
        }
        LOG_INFO << "[SDL2] Library version " << SDL_MAJOR_VERSION << '.' << SDL_MINOR_VERSION << "." << SDL_PATCHLEVEL;

        std::string title;
        title += PROJECT_NAME;
#ifdef _DEBUG
        title += " Debug";
#endif

#if _WIN32
        // I have tested several renderers on Windows and each of them has its own issues.
        // direct3d (D3D9): skin "filter" parameters are affected by textures
        // direct3d11: such a simple 2D game should not be so picky about hardware...
        // direct3d12: same as above; plus the dll from the official site does not include this renderer
        // opengl: does not support certain custom blend mode
        // opengles2: does not support virtual textures created by hand (e.g. black dot)
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
#else
        //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif

        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

        Uint32 flags = SDL_WINDOW_HIDDEN;   // window created with opengles2 will destroy itself when creating Renderer, resulting in flashes
#ifdef _DEBUG
        flags |= SDL_WINDOW_RESIZABLE;
#endif
        auto mode = ConfigMgr::get("V", cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
        if (strEqual(mode, cfg::V_WINMODE_BORDERLESS, true))
        {
            flags |= SDL_WINDOW_BORDERLESS;
        }
        else if (strEqual(mode, cfg::V_WINMODE_FULL, true))
        {
            flags |= SDL_WINDOW_FULLSCREEN;
        }
        else 
        {
            // fallback to windowed
        }
        windowRect.w = ConfigMgr::get("V", cfg::V_DISPLAY_RES_X, CANVAS_WIDTH);
        windowRect.h = ConfigMgr::get("V", cfg::V_DISPLAY_RES_Y, CANVAS_HEIGHT);
        gFrameWindow = SDL_CreateWindow(title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowRect.w, windowRect.h, flags);
        if (!gFrameWindow)
        {
            LOG_FATAL << "[SDL2] Init window ERROR! " << SDL_GetError();
            return -1;
        }

        //canvasRect.w = ConfigMgr::get("V", cfg::V_RES_X, CANVAS_WIDTH);
        //canvasRect.h = ConfigMgr::get("V", cfg::V_RES_Y, CANVAS_HEIGHT);
        //graphics_resize_canvas(canvasRect.w, canvasRect.h);
        graphics_resize_canvas(windowRect.w, windowRect.h);

        int ss = ConfigMgr::get("V", cfg::V_RES_SUPERSAMPLE, 1);
        graphics_set_supersample_level(ss);

        int maxFPS = ConfigMgr::get("V", cfg::V_MAXFPS, 480);
        if (maxFPS < 30 && maxFPS != 0)
            maxFPS = 30;
        graphics_set_maxfps(maxFPS);

        if (ConfigMgr::get("V", cfg::V_VSYNC, false))
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
            LOG_FATAL << "[SDL2] Init renderer ERROR! " << SDL_GetError();
            return -2;
        }
        SDL_SetRenderDrawColor(gFrameRenderer, 0, 0, 0, 255);

        SDL_ShowWindow(gFrameWindow);

        SDL_RendererInfo rendererInfo;
        if (SDL_GetRendererInfo(gFrameRenderer, &rendererInfo) == 0)
        {
            LOG_INFO << "[SDL2] Renderer driver: " << rendererInfo.name;
        }

        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        if (SDL_GetWindowWMInfo(gFrameWindow, &wmInfo) == SDL_TRUE)
        {
#if _WIN32 || _WIN64
            setWindowHandle((void*)&wmInfo.info.win.window);
#endif
        }

        gInternalRenderTarget = SDL_CreateTexture(gFrameRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET,
            CANVAS_WIDTH_MAX, CANVAS_HEIGHT_MAX);
        if (!gInternalRenderTarget)
        {
            LOG_FATAL << "[SDL2] Init Target Texture Error! " << SDL_GetError();
            return -3;
        }
        SDL_SetTextureScaleMode(gInternalRenderTarget, SDL_ScaleModeBest);

        SDL_SetRenderTarget(gFrameRenderer, gInternalRenderTarget);

        SDL_ShowCursor(SDL_DISABLE);

        SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);

        LOG_INFO << "[SDL2] SDL2 init finished.";
    }

    // SDL_Image
    {
        LOG_INFO << "[SDL2] Initializing SDL2_Image...";

        auto flags = IMG_INIT_JPG | IMG_INIT_PNG;
        if (flags != IMG_Init(flags))
        {
            // error handling
            LOG_FATAL << "[SDL2] SDL2_Image init failed. " << IMG_GetError();
            return 1;
        }
        LOG_INFO << "[SDL2] SDL2_Image init finished. Version " << SDL_IMAGE_MAJOR_VERSION << '.' << SDL_IMAGE_MINOR_VERSION << "." << SDL_IMAGE_PATCHLEVEL;
    }
    // SDL_ttf
    {
        LOG_INFO << "[SDL2] Initializing SDL2_TTF...";

        if (-1 == TTF_Init())
        {
            // error handling
            LOG_FATAL << "[SDL2] SDL2_TTF init failed. " << TTF_GetError();
            return 2;
        }
        LOG_INFO << "[SDL2] SDL2_TTF init finished. Version " << SDL_TTF_MAJOR_VERSION << '.' << SDL_TTF_MINOR_VERSION << "." << SDL_TTF_PATCHLEVEL;
    }

#ifndef VIDEO_DISABLED
	// libav
	video_init();
#endif

    // imgui
    LOG_INFO << "Initializing ImGui for SDL renderer...";
    if (!ImGui_ImplSDL2_InitForSDLRenderer(gFrameWindow, gFrameRenderer) ||
        !ImGui_ImplSDLRenderer_Init(gFrameRenderer))
    {
        LOG_FATAL << "ImGui init failed.";
        return 3;
    }
    LOG_INFO << "ImGui init finished.";

    // Draw a black frame to prevent flashbang
    graphics_clear();
    graphics_flush();

    return 0;
}

void graphics_clear()
{
    SDL_RenderClear(gFrameRenderer);
}

static int maxFPS = 0;
static std::chrono::nanoseconds desiredFrameTimeBetweenFrames;
static std::chrono::high_resolution_clock::time_point frameTimestampPrev;
static Path screenshotPath;
void graphics_flush()
{
    SDL_SetRenderTarget(gFrameRenderer, NULL);
    {
        // TODO scale internal canvas
        Rect ssRect = canvasRect;
        int ssLevel = graphics_get_supersample_level();
        ssRect.w *= ssLevel;
        ssRect.h *= ssLevel;

        // render internal canvas texture
        SDL_RenderCopyEx(
            gFrameRenderer, gInternalRenderTarget,
            &ssRect, &windowRect,
            0, NULL, SDL_FLIP_NONE);

        // render imgui
        auto pData = ImGui::GetDrawData();
        if (pData != NULL)
        {
            ImGui_ImplSDLRenderer_RenderDrawData(pData);
        }
        SDL_RenderPresent(gFrameRenderer);

        if (!screenshotPath.empty())
        {
            fs::create_directories(screenshotPath.parent_path());
            SDL_Surface* sshot = SDL_CreateRGBSurface(0, windowRect.w, windowRect.h, 32, 0, 0, 0, 0);
            SDL_RenderReadPixels(gFrameRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
            IMG_SavePNG(sshot, screenshotPath.u8string().c_str());
            SDL_FreeSurface(sshot);
            screenshotPath.clear();
        }
    }
    SDL_SetRenderTarget(gFrameRenderer, gInternalRenderTarget);

    if (maxFPS != 0)
    {
        long long sleep_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
            frameTimestampPrev + desiredFrameTimeBetweenFrames - std::chrono::high_resolution_clock::now()).count();
        preciseSleep(sleep_ns);
    }
    frameTimestampPrev = std::chrono::high_resolution_clock::now();
}

int graphics_free()
{
    LOG_INFO << "Shutting down ImGui module...";
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();

    LOG_INFO << "Releasing SDL resources...";
    SDL_DestroyTexture(gInternalRenderTarget);
    gInternalRenderTarget = nullptr;
    SDL_DestroyRenderer(gFrameRenderer);
    gFrameRenderer = nullptr;
    SDL_DestroyWindow(gFrameWindow);
    gFrameWindow = nullptr;

    LOG_INFO << "[SDL2] De-initializing TTF module...";
    TTF_Quit();
    LOG_INFO << "[SDL2] De-initializing Image module...";
    IMG_Quit();
    LOG_INFO << "[SDL2] De-initializing SDL2...";
    SDL_Quit();

    LOG_INFO << "Graphics shutdown complete.";

    return 0;
}

void graphics_copy_screen_texture(Texture& texture)
{
    assert(IsMainThread());

    SDL_SetRenderTarget(gFrameRenderer, (SDL_Texture*)texture.raw());
    SDL_RenderClear(gFrameRenderer);
    SDL_Rect rect{ 0, 0, canvasRect.w * graphics_get_supersample_level(), canvasRect.h * graphics_get_supersample_level() };
    SDL_RenderCopyEx(gFrameRenderer, gInternalRenderTarget, &rect, &canvasRect, 0, NULL, SDL_FLIP_NONE);
    SDL_SetRenderTarget(gFrameRenderer, gInternalRenderTarget);
}

int graphics_get_monitor_index()
{
    return SDL_GetWindowDisplayIndex(gFrameWindow);
}

std::pair<int, int> graphics_get_desktop_resolution()
{
    int index = graphics_get_monitor_index();
    SDL_DisplayMode mode = { 0 };
    SDL_GetDesktopDisplayMode(index, &mode);
    return { mode.w, mode.h };
}

std::vector<std::tuple<int, int, int>> graphics_get_resolution_list()
{
    int index = graphics_get_monitor_index();
    int modes = SDL_GetNumDisplayModes(index);
    std::vector<std::tuple<int, int, int>> res;
    for (int i = 0; i < modes; ++i)
    {
        SDL_DisplayMode mode = { 0 };
        SDL_GetDisplayMode(index, i, &mode);
        LOG_DEBUG << mode.w << 'x' << mode.h << ' ' << mode.refresh_rate << "Hz";
        res.push_back({ mode.w, mode.h, mode.refresh_rate });
    }
    return res;
}

void graphics_change_window_mode(int mode)
{
    LOG_WARNING << "Setting window mode to " << mode;
    switch (mode)
    {
    case 0:
        SDL_SetWindowFullscreen(gFrameWindow, 0);
        SDL_SetWindowBordered(gFrameWindow, SDL_TRUE);
        break;
    case 1:
        SDL_SetWindowFullscreen(gFrameWindow, SDL_WINDOW_FULLSCREEN);
        break;
    case 2:
        SDL_SetWindowFullscreen(gFrameWindow, 0);
        SDL_SetWindowBordered(gFrameWindow, SDL_FALSE);
        break;
    case 3:
        SDL_SetWindowFullscreen(gFrameWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_SetWindowBordered(gFrameWindow, SDL_FALSE);
        break;
    }
}

void graphics_resize_window(int x, int y)
{
    LOG_WARNING << "Resizing window mode to " << x << 'x' << y;
    if (x != 0) windowRect.w = x;
    if (y != 0) windowRect.h = y;
    canvasScaleX = (double)x / canvasRect.w;
    canvasScaleY = (double)y / canvasRect.h;
    SDL_SetWindowSize(gFrameWindow, windowRect.w, windowRect.h);
    SDL_SetWindowPosition(gFrameWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void graphics_change_vsync(int mode)
{
    LOG_WARNING << "Setting vsync mode to " << mode;
#if _WIN32
    SDL_RenderSetVSync(gFrameRenderer, mode);
#else
    // codes below should work since we are explicitly indicated to use OpenGL backend
    SDL_GL_SetSwapInterval(mode == 2 ? -1 : mode);
#endif
}

static int superSampleLevel = 1;
void graphics_set_supersample_level(int level)
{
    LOG_WARNING << "Setting supersample level to " << level;
    //assert(canvasRect.w * level <= 3840);
    superSampleLevel = level;
}
int graphics_get_supersample_level()
{
    return superSampleLevel;
}

void graphics_resize_canvas(int x, int y)
{
    LOG_WARNING << "Resizing canvas to " << x << 'x' << y;
    canvasRect.w = x;
    canvasRect.h = y;
    canvasScaleX = (double)windowRect.w / x;
    canvasScaleY = (double)windowRect.h / y;
}
double graphics_get_canvas_scale_x() { return canvasScaleX; }
double graphics_get_canvas_scale_y() { return canvasScaleY; }


void graphics_set_maxfps(int fps)
{
    LOG_WARNING << "Setting max fps to " << fps;
    maxFPS = fps;
    if (maxFPS != 0)
    {
        desiredFrameTimeBetweenFrames = std::chrono::nanoseconds(static_cast<long long>(std::round(1e9 / maxFPS)));
    }
}

///////////////////////////////////////////////////////////////////////////

extern bool gEventQuit;

static bool isEditing = false;
static std::string textBuf, textBufSuffix;
void funEditing(const SDL_TextEditingEvent& e);
void funInput(const SDL_TextInputEvent& e);
void funKeyDown(const SDL_KeyboardEvent& e);

static std::int16_t i16_from_i32(std::int32_t value) {
    if (value > SHRT_MAX) {
        return SHRT_MAX;
    } else if (value < SHRT_MIN) {
        return SHRT_MIN;
    } else {
        return static_cast<std::int16_t>(value);
    }
}

void event_handle()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        ImGui_ImplSDL2_ProcessEvent(&e);

        switch (e.type)
        {
        case SDL_QUIT:
            LOG_WARNING << "[Event] SDL_QUIT";
            gEventQuit = true;
            break;

        case SDL_SYSWMEVENT:
#ifdef WIN32
            callWMEventHandler(
                &e.syswm.msg->msg.win.hwnd,
                &e.syswm.msg->msg.win.msg,
                &e.syswm.msg->msg.win.wParam,
                &e.syswm.msg->msg.win.lParam);
#elif defined __linux__

#endif
            break;

        case SDL_WINDOWEVENT:
            switch (e.window.event)
            {
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                SetWindowForeground(true);
                break;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                SetWindowForeground(false);
                break;
            default:
                break;
            }
            break;

        case SDL_TEXTINPUT:
            if (isEditing) funInput(e.text);
            break;

        case SDL_TEXTEDITING:
            if (isEditing) funEditing(e.edit);
            break;

        case SDL_KEYDOWN:
            sdl::state::g_keyboard_scancodes[e.key.keysym.scancode] = true;

            if (isEditing) funKeyDown(e.key);
            break;

        case SDL_KEYUP:
            sdl::state::g_keyboard_scancodes[e.key.keysym.scancode] = false;

            break;

        case SDL_MOUSEMOTION:
            sdl::state::g_mouse_x = e.motion.x;
            sdl::state::g_mouse_y = e.motion.y;
            break;

        case SDL_MOUSEBUTTONDOWN:
            sdl::state::g_mouse_buttons[e.button.button] = true;
            break;

        case SDL_MOUSEBUTTONUP:
            sdl::state::g_mouse_buttons[e.button.button] = false;
            break;

        case SDL_MOUSEWHEEL:
            sdl::state::g_mouse_wheel_delta = i16_from_i32(e.wheel.y);
            break;
        default:
            break;
        }
    }
}

void ImGuiNewFrame()
{
    SDL_SetRenderTarget(gFrameRenderer, NULL);
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    SDL_SetRenderTarget(gFrameRenderer, gInternalRenderTarget);
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
}

static std::function<void(const std::string&)> funUpdateText;
void startTextInput(const RectF& textBox, const std::string& oldText, std::function<void(const std::string&)> funUpdateText)
{
    LOG_DEBUG << "Start Text Input";

    textBuf = oldText;
    textBuf.reserve(32);

    ::funUpdateText = funUpdateText;

    SDL_Rect r;
    r.x = (int)std::floor(textBox.x);
    r.y = (int)std::floor(textBox.y);
    r.w = (int)std::ceil(textBox.w);
    r.h = (int)std::ceil(textBox.h);
    SDL_SetTextInputRect(&r);
    SDL_StartTextInput();
    isEditing = true;

    funUpdateText(textBuf);
}

void stopTextInput()
{
    LOG_DEBUG << "Stop Text Input";

    isEditing = false;
    funUpdateText = [](const std::string&) {};
    SDL_StopTextInput();
}


void funEditing(const SDL_TextEditingEvent& e)
{
    LOG_DEBUG << "Editing " << e.start << " " << e.length << " " << e.text;
    if (strlen(e.text) == 0)
    {
        textBuf += textBufSuffix;
        textBufSuffix.clear();
        funUpdateText(textBuf);
    }
    else
    {
        if (e.length > 0)
        {
            textBufSuffix = textBuf.substr(e.start + e.length);
            textBuf.erase(e.start, textBuf.npos);
        }
        funUpdateText(textBuf + e.text + textBufSuffix);
    }
}

void funInput(const SDL_TextInputEvent& e)
{
    textBuf = textBuf + e.text + textBufSuffix;
    textBufSuffix.clear();
    funUpdateText(textBuf);
}

void funKeyDown(const SDL_KeyboardEvent& e)
{
    if (e.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL))
    {
        switch (e.keysym.sym)
        {
        case SDLK_v:
        {
            std::stringstream ss;
            char* pText = SDL_GetClipboardText();
            ss << pText;
            SDL_free(pText);
            std::string textLine;
            std::getline(ss, textLine);
            textBuf += textLine;
            funUpdateText(textBuf + textBufSuffix);
            break;
        }
        }
    }
    else
    {
        switch (e.keysym.sym)
        {
        case SDLK_BACKSPACE:
            if (textBufSuffix.empty() && !textBuf.empty())
            {
                std::u32string textTmp = utf8_to_utf32(textBuf);
                textTmp.erase(textTmp.length() - 1, 1);
                textBuf = utf32_to_utf8(textTmp);
                funUpdateText(textBuf + textBufSuffix);
            }
            break;
        }
    }
}

#endif

void graphics_screenshot(const Path& png)
{
    LOG_INFO << "Screenshot: " << png.u8string();

    screenshotPath = png;
}