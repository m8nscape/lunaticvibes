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
#include "game/generic_info.h"

#include "common/chartformat/chartformat_bms.h"

#include "common/log.h"

#include "imgui.h"

#ifdef WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#include <consoleapi2.h>
#endif //WIN32

#include <regex>

bool gEventQuit;
GenericInfoUpdater gGenericInfo{ 1 };

void mainLoop()
{
    gGenericInfo.loopStart();

    eScene currentScene = eScene::NOT_INIT;
    gNextScene = eScene::SELECT;

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
        if (currentScene != gNextScene)
        {
            if (scene)
            {
                scene->inputLoopEnd();
                scene->loopEnd();
            }

            switch (gNextScene)
            {
            case eScene::SELECT: LOG_INFO << "Scene changed to SELECT"; break;
            case eScene::DECIDE: LOG_INFO << "Scene changed to DECIDE"; break;
            case eScene::PLAY:   LOG_INFO << "Scene changed to PLAY"; break;
            case eScene::RETRY_TRANS:  LOG_INFO << "Scene changed to RETRY_TRANS"; break;
            case eScene::RESULT: LOG_INFO << "Scene changed to RESULT"; break;
            case eScene::KEYCONFIG: LOG_INFO << "Scene changed to KEYCONFIG"; break;
            case eScene::CUSTOMIZE: LOG_INFO << "Scene changed to CUSTOMIZE"; break;
            case eScene::EXIT:   LOG_INFO << "Scene changed to EXIT"; break;
            default: break;
            }

            // Disable skin caching for now. dst options are changing all the time
            SkinMgr::clean();

            clearCustomDstOpt();
			currentScene = gNextScene;
            if (currentScene != eScene::EXIT)
            {
                scene = SceneMgr::get(currentScene);
                assert(scene != nullptr);
                scene->loopStart();
                scene->inputLoopStart();
            }
        }

        // draw
        {
            graphics_clear();
            doMainThreadTask();
            scene->update();
            scene->draw();
            graphics_flush();
        }
		++gFrameCount[0];
    }
    scene->inputLoopEnd();
    scene->loopEnd();

	gGenericInfo.loopEnd();
}

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
    if (ConfigMgr::get('P', cfg::P_RELATIVE_AXIS, false))
        InputMgr::setAxisMode(InputMgr::eAxisMode::AXIS_RELATIVE);

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
	InputMgr::updateBindings(7, Input::Pad::S1L);

    // reset globals
    //gNumbers.setDefault(eNumber::ZERO, 0);
    gSwitches.setDefault(eSwitch::_FALSE, false);
    gSwitches.setDefault(eSwitch::_TRUE, true);
    //gSliders.setDefault(eSlider::ZERO, 0);
    //gTexts.setDefault(eText::INVALID, "");
    gTimers.setDefault(eTimer::_NEVER, TIMER_NEVER);

    gNumbers.reset();
    gSwitches.reset();
    gSliders.reset();
    gTexts.reset();
    gTimers.reset();
    ConfigMgr::setGlobals();

    SoundMgr::setVolume(SampleChannel::MASTER, (float)gSliders.get(eSlider::VOLUME_MASTER));
    SoundMgr::setVolume(SampleChannel::KEY, (float)gSliders.get(eSlider::VOLUME_KEY));
    SoundMgr::setVolume(SampleChannel::BGM, (float)gSliders.get(eSlider::VOLUME_BGM));

    if (gSwitches.get(eSwitch::SOUND_FX0))
        SoundMgr::setDSP((DSPType)gOptions.get(eOption::SOUND_FX0), 0, (SampleChannel)gOptions.get(eOption::SOUND_TARGET_FX0), gSliders.get(eSlider::FX0_P1), gSliders.get(eSlider::FX0_P2));
    if (gSwitches.get(eSwitch::SOUND_FX1))
        SoundMgr::setDSP((DSPType)gOptions.get(eOption::SOUND_FX1), 1, (SampleChannel)gOptions.get(eOption::SOUND_TARGET_FX1), gSliders.get(eSlider::FX1_P1), gSliders.get(eSlider::FX1_P2));
    if (gSwitches.get(eSwitch::SOUND_FX2))
        SoundMgr::setDSP((DSPType)gOptions.get(eOption::SOUND_FX2), 2, (SampleChannel)gOptions.get(eOption::SOUND_TARGET_FX2), gSliders.get(eSlider::FX2_P1), gSliders.get(eSlider::FX2_P2));

    if (gSwitches.get(eSwitch::SOUND_PITCH))
    {
        static const double tick = std::pow(2, 1.0 / 12);
        double f = std::pow(tick, gNumbers.get(eNumber::PITCH));
        switch (gOptions.get(eOption::SOUND_PITCH_TYPE))
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

    if (gSwitches.get(eSwitch::SOUND_EQ))
    {
        for (int idx = 0; idx < 7; ++idx)
        {
            int val = gNumbers.get(eNumber(idx + (int)eNumber::EQ0));
            SoundMgr::setEQ((EQFreq)idx, val);
        }
    }

    // score db
    std::string scoreDBPath = (ConfigMgr::Profile()->getPath() / "score.db").u8string();
    g_pScoreDB = std::make_shared<ScoreDB>(scoreDBPath.c_str());

    // initialize song list
    Path dbPath = Path(GAMEDATA_PATH) / "database";
    if (!fs::exists(dbPath)) fs::create_directories(dbPath);
    g_pSongDB = std::make_shared<SongDB>(dbPath / "song.db");

    // get folders from config
    auto folderList = ConfigMgr::General()->getFoldersPath();
    for (auto& f : folderList)
    {
        g_pSongDB->addFolder(f);
    }

    SongListProperties rootFolderProp{
        "",
        ROOT_FOLDER_HASH,
        "",
        {},
        0
    };
    auto top = g_pSongDB->browse(ROOT_FOLDER_HASH, false);
    for (size_t i = 0; i < top.getContentsCount(); ++i)
    {
        auto entry = top.getEntry(i);

        bool deleted = true;
        for (auto& f : folderList)
        {
            if (fs::equivalent(f, entry->getPath()))
            {
                deleted = false;
                break;
            }
        }
        if (!deleted)
        {
            g_pSongDB->browse(entry->md5, true);
            rootFolderProp.list.push_back({ entry, nullptr });
        }
    }
    gSelectContext.backtrace.push(rootFolderProp);

    // arg parsing
    if (argc >= 2)
    {
        gNextScene = eScene::PLAY;
        gQuitOnFinish = true;

        std::shared_ptr<BMS> bms = std::make_shared<BMS>(argv[1]);
        gChartContext = ChartContextParams{
            argv[1],
            md5file(argv[1]),
            bms,
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
