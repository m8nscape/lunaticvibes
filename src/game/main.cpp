#include "graphics/graphics.h"
#include "config/config_mgr.h"
#include "sound/sound_mgr.h"
#include "scene/scene_mgr.h"
#include "input/input_mgr.h"
#include "skin/skin_lr2.h"
#include "utils.h"
#include "scene/scene_context.h"
#include "game/generic_info.h"
#include <plog/Log.h>

#if _DEBUG
#include <plog/Appenders/ColorConsoleAppender.h>        // for command prompt log purpose
#endif

#if WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#endif //WIN32

GenericInfoUpdater __ginfo{ 1 };

void mainLoop()
{
    eScene currentScene = __next_scene;
    auto scene = SceneMgr::get(currentScene);
	__ginfo.loopStart();
    while (currentScene != eScene::EXIT)
    {
        // Evenet handling
        event_handle();

        // Scene change
        if (currentScene != __next_scene)
        {
            currentScene = __next_scene;
            scene = SceneMgr::get(currentScene);
            clearCustomDstOpt();
        }

        // draw
        graphics_clear();
        scene->update();
        scene->draw();
        graphics_flush();
		++__frames[0];
    }
	__ginfo.loopEnd();
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

    // reset globals
    gNumbers.set(eNumber::ZERO, 0);
    gSwitches.set(eSwitch::_FALSE, false);
    gSliders.set(eSlider::ZERO, 0);
    gTexts.set(eText::INVALID, "");
    gNumbers.reset();
    gSwitches.set(eSwitch::_TRUE, false);
    gSwitches.reset();
    gSwitches.set(eSwitch::_TRUE, true);
    gSliders.reset();
    gTexts.reset();
    gTimers.set(eTimer::SCENE_START, 0);
    gTimers.reset();

    // temporary: hardcode bms file for test
    context_chart = __chart_context_params{
        "res/sample_7.bme",
        "",
        nullptr,
        nullptr,
        false,
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
    gTexts.set(eText::PLAY_TITLE, context_chart.title);
    gTexts.set(eText::PLAY_SUBTITLE, context_chart.title2);
    gTexts.set(eText::PLAY_ARTIST, context_chart.artist);
    gTexts.set(eText::PLAY_SUBARTIST, context_chart.artist2);
    gTexts.set(eText::PLAY_GENRE, context_chart.genre);

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
