#include "common/meta.h"
#include "config/config_mgr.h"
#include "game/graphics/graphics.h"
#include "game/sound/sound_mgr.h"
#include "game/scene/scene_mgr.h"
#include "game/input/input_mgr.h"
#include "game/skin/skin_lr2.h"
#include "game/skin/skin_mgr.h"
#include "common/utils.h"
#include "common/sysutil.h"
#include "game/scene/scene_context.h"
#include "game/runtime/generic_info.h"

#include "common/chartformat/chartformat_bms.h"

#include "common/log.h"

#include "imgui.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#include <consoleapi2.h>
#include <ole2.h>
#include <timeapi.h>
#endif //WIN32

#include <boost/format.hpp>

bool gEventQuit;
GenericInfoUpdater gGenericInfo{ 1 };

void mainLoop();

// SDL_main
int main(int argc, char* argv[])
{
    SetDebugThreadName("MainThread");
    SetThreadAsMainThread();

    char exePath[256];
    size_t exePathLen;
    GetExecutablePath(exePath, 256, exePathLen);
    executablePath = Path(exePath);
    fs::current_path(executablePath);

    // init logger
    InitLogger();

    // load configs
    ConfigMgr::init();
    ConfigMgr::load();

    if (!fs::is_directory(utf8_to_utf32(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2Files/"))))
    {
        LOG_ERROR << "LR2files directory not found!";
        return -1;
    }

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    // further operations present in graphics_init()

    // init graphics
    if (auto ginit = graphics_init())
        return ginit;

    // init sound
    if (auto sinit = SoundMgr::initFMOD())
        return sinit;
    SoundMgr::startUpdate();

	// load input bindings
    InputMgr::init();
	InputMgr::updateDevices();

    // reset globals
    ConfigMgr::setGlobals();

    SoundMgr::setVolume(SampleChannel::MASTER, (float)State::get(IndexSlider::VOLUME_MASTER));
    SoundMgr::setVolume(SampleChannel::KEY, (float)State::get(IndexSlider::VOLUME_KEY));
    SoundMgr::setVolume(SampleChannel::BGM, (float)State::get(IndexSlider::VOLUME_BGM));

    if (State::get(IndexSwitch::SOUND_FX0))
        SoundMgr::setDSP((DSPType)State::get(IndexOption::SOUND_FX0), 0, (SampleChannel)State::get(IndexOption::SOUND_TARGET_FX0), State::get(IndexSlider::FX0_P1), State::get(IndexSlider::FX0_P2));
    if (State::get(IndexSwitch::SOUND_FX1))
        SoundMgr::setDSP((DSPType)State::get(IndexOption::SOUND_FX1), 1, (SampleChannel)State::get(IndexOption::SOUND_TARGET_FX1), State::get(IndexSlider::FX1_P1), State::get(IndexSlider::FX1_P2));
    if (State::get(IndexSwitch::SOUND_FX2))
        SoundMgr::setDSP((DSPType)State::get(IndexOption::SOUND_FX2), 2, (SampleChannel)State::get(IndexOption::SOUND_TARGET_FX2), State::get(IndexSlider::FX2_P1), State::get(IndexSlider::FX2_P2));

    if (State::get(IndexSwitch::SOUND_PITCH))
    {
        static const double tick = std::pow(2, 1.0 / 12);
        double f = std::pow(tick, State::get(IndexNumber::PITCH));
        switch (State::get(IndexOption::SOUND_PITCH_TYPE))
        {
        case 0: // FREQUENCY
            SoundMgr::setFreqFactor(f);
            gSelectContext.pitchSpeed = f;
            break;
        case 1: // PITCH
            SoundMgr::setFreqFactor(1.0);
            SoundMgr::setPitch(f);
            gSelectContext.pitchSpeed = 1.0;
            break;
        case 2: // SPEED (freq up, pitch down)
            SoundMgr::setFreqFactor(1.0);
            SoundMgr::setSpeed(f);
            gSelectContext.pitchSpeed = f;
            break;
        default:
            break;
        }
    }

    if (State::get(IndexSwitch::SOUND_EQ))
    {
        for (int idx = 0; idx < 7; ++idx)
        {
            int val = State::get(IndexNumber(idx + (int)IndexNumber::EQ0));
            SoundMgr::setEQ((EQFreq)idx, val);
        }
    }

    gSelectContext.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);

    // score db
    std::string scoreDBPath = (ConfigMgr::Profile()->getPath() / "score.db").u8string();
    g_pScoreDB = std::make_shared<ScoreDB>(scoreDBPath.c_str());

    // song db
    Path dbPath = Path(GAMEDATA_PATH) / "database";
    if (!fs::exists(dbPath)) fs::create_directories(dbPath);
    g_pSongDB = std::make_shared<SongDB>(dbPath / "song.db");

    // load songs / tables at ScenePreSelect

    // arg parsing
    if (argc >= 2)
    {
        gNextScene = eScene::PLAY;
        gQuitOnFinish = true;

        std::shared_ptr<ChartFormatBMS> bms = std::make_shared<ChartFormatBMS>(argv[1], std::time(NULL));
        gChartContext = ChartContextParams{
            argv[1],
            md5file(argv[1]),
            bms,
            nullptr,

            false,
            false,
            false,

            false,

            bms->title,
            bms->title2,
            bms->artist,
            bms->artist2,
            bms->genre,
            bms->version,

            2.0,
            bms->minBPM,
            bms->startBPM,
            bms->maxBPM,
        };
    }
    else
    {
        gNextScene = eScene::PRE_SELECT;
    }

    /*
#ifndef _DEBUG
    // preload all skins
    LOG_INFO << "==============================================";
    LOG_INFO << "Preload all skins";
    for (eMode e = eMode::TITLE; e < eMode::MODE_COUNT; ++(*(int*)&e))
    {
        SkinMgr::load(e);
    }
    LOG_INFO << "Preload all skins finished";
    LOG_INFO << "==============================================";
#endif
    */

#ifdef WIN32
    timeBeginPeriod(1);

    HRESULT oleInitializeResult = OleInitialize(NULL);
    if (oleInitializeResult < 0)
    {
        LOG_WARNING << "OleInitialize Failed";
    }
#endif

    mainLoop();

#ifdef WIN32
    if (oleInitializeResult >= 0)
    {
        OleUninitialize();
    }

    timeEndPeriod(1);
#endif

    SoundMgr::stopUpdate();

    SceneMgr::clean();	// clean resources before releasing framework
    graphics_free();

    ImGui::DestroyContext();

    ConfigMgr::save();

    StopHandleMainThreadTask();
    return 0;
}


void mainLoop()
{
    gGenericInfo.loopStart();

    eScene currentScene = eScene::NOT_INIT;

    pScene scene = nullptr;
    while (!gEventQuit && currentScene != eScene::EXIT && gNextScene != eScene::EXIT)
    {
        // Evenet handling
        event_handle();
        if (gEventQuit)
        {
            gAppIsExiting = true;
        }

        // Scene change
        static pScene sceneCustomize = nullptr;
        if (gInCustomize)
        {
            if (sceneCustomize == nullptr)
            {
                sceneCustomize = SceneMgr::get(eScene::CUSTOMIZE);
                sceneCustomize->loopStart();
                sceneCustomize->inputLoopStart();
            }
            if (gExitingCustomize && sceneCustomize)
            {
                gInCustomize = false;
                sceneCustomize->inputLoopEnd();
                sceneCustomize->loopEnd();
                sceneCustomize.reset();
                gNextScene = eScene::SELECT;
            }
        }
        if (gInCustomize && gCustomizeSceneChanged || !gInCustomize && currentScene != gNextScene || gExitingCustomize)
        {
            if (!gInCustomize) gExitingCustomize = false;
            gCustomizeSceneChanged = false;

            if (scene)
            {
                scene->inputLoopEnd();
                scene->loopEnd();
                scene.reset();
            }

            clearCustomDstOpt();
            currentScene = gNextScene;
            if (currentScene != eScene::EXIT && currentScene != eScene::NOT_INIT)
            {
                scene = SceneMgr::get(currentScene);
                assert(scene != nullptr);
                scene->loopStart();
                scene->inputLoopStart();
                if (gInCustomize) scene->disableMouseInput();
            }
        }

        // draw
        {
            graphics_clear();
            doMainThreadTask();
            if (scene)
            {
                scene->update();
                scene->draw();
            }
            if (sceneCustomize)
            {
                sceneCustomize->update();
                sceneCustomize->draw();
            }
            graphics_flush();
        }
        ++gFrameCount[0];
    }
    if (scene)
    {
        scene->inputLoopEnd();
        scene->loopEnd();
        scene.reset();
    }

    gGenericInfo.loopEnd();
}
