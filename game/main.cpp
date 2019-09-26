#include "meta.h"
#include "game/graphics/graphics.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/scene/scene_mgr.h"
#include "game/input/input_mgr.h"
#include "game/skin/skin_lr2.h"
#include "utils.h"
#include "sysutil.h"
#include "game/scene/scene_context.h"
#include "game/generic_info.h"
#include <plog/Log.h>

#include "common/chart/bms.h"

#if _DEBUG
#include <plog/Appenders/ColorConsoleAppender.h>        // for command prompt log purpose
#endif

#ifdef WIN32
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

        // sound update (temporary)
        SoundMgr::update();
    }
	__ginfo.loopEnd();
}

int main(int argc, char* argv[])
{
    SetThreadName("MainThread");

    char exePath[256];
    size_t exePathLen;
    GetExecutablePath(exePath, 256, exePathLen);
    executablePath = fs::path(exePath);
    fs::current_path(executablePath);

    // init logger
#if _DEBUG
    auto appender = plog::ColorConsoleAppender<plog::TxtFormatterImpl<false>>();
    plog::init(plog::debug, &appender);
#else
    auto fmt = std::put_time(std::localtime(nullptr), "%F");
    std::stringstream logfile;
    logfile << MAIN_NAME << "-" << fmt << ".log";
    plog::init(plog::info, logfile.str().c_str(), 1000000, 5);
#endif

    ConfigMgr::init();
    ConfigMgr::load();

    if (auto ginit = graphics_init())
        return ginit;
    if (auto sinit = SoundMgr::initFMOD())
        return sinit;

	// load input bindings
	InputMgr::updateDevices();
	InputMgr::updateBindings(7, Input::Ingame::S1L);

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
    if (argc >= 2)
    {
        std::shared_ptr<BMS> bms = std::make_shared<BMS>(argv[1]);
        context_chart = __chart_context_params{
            argv[1],
            "",
            bms,
            false,
            false,
            false,

            bms->_title,
            bms->_title2,
            bms->_artist,
            bms->_artist2,
            bms->_genre,
            bms->_version,

            2.0,
            bms->_minBPM,
            bms->_itlBPM,
            bms->_maxBPM,
        };
    }

#ifdef WIN32
    timeBeginPeriod(1);
#endif

    mainLoop();

    graphics_free();

#ifdef WIN32
    timeEndPeriod(1);
#endif

    ConfigMgr::save();

    return 0;
}
