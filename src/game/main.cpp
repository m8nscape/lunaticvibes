#include "graphics/graphics.h"
#include "config/config_mgr.h"
#include "sound/sound_mgr.h"
#include "scene/scene_mgr.h"
#include "input/input_mgr.h"
#include "skin/skin_lr2.h"
#include "utils.h"
#include "scene/scene_context.h"
#include <plog/Log.h>

#if _DEBUG
#include <plog/Appenders/ColorConsoleAppender.h>        // for command prompt log purpose
#endif

#if WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#endif //WIN32

void mainLoop()
{
    eScene currentScene = __next_scene;
    auto scene = SceneMgr::get(currentScene);
    while (currentScene != eScene::EXIT)
    {
        // Evenet handling
        event_handle();

        // Scene change
        if (currentScene != __next_scene)
        {
            currentScene = __next_scene;
            scene = SceneMgr::get(currentScene);
            SkinLR2::clearCustomDstOpt();
        }

        // draw
        graphics_clear();
        scene->update();
        scene->draw();
        graphics_flush();
    }
}

int main(int argc, char* argv[])
{
    SetThreadName("MainThread");

    // init logger
#if _DEBUG
    auto appender = plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>();
    plog::init(plog::debug, &appender);
#else
    plog::init(plog::info, "log.txt", 1000000, 5);
#endif

    ConfigMgr::init();
    ConfigMgr::load();

    if (auto ginit = graphics_init())
        return ginit;
    if (auto sinit = SoundMgr::initFMOD())
        return sinit;

	// load input bindings
	InputMgr::updateDevices();
	InputMgr::updateBindings(Input::Ingame::S1L);

    // temporary: hardcode bms file for test
    context_chart = __chart_context_params{
        "res/sample_7.bme",
        "",
        nullptr,
        nullptr,
        false,
        false,

        "TITLE",
        "subtitle",
        "ARTIST",
        "subartist",
        "GENRE",
        "Version",

        2.0,
        130,
        150,
        180
    };

#if WIN32
    timeBeginPeriod(1);
#endif

    mainLoop();

    graphics_free();

#if WIN32
    timeEndPeriod(1);
#endif

    ConfigMgr::save();

    return 0;
}
