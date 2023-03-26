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

#include "game/runtime/i18n.h"

#include "game/arena/arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

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

#include <curl/curl.h>

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

    // init curl
    LOG_INFO << "Initializing libcurl...";
    if (CURLcode ret = curl_global_init(CURL_GLOBAL_DEFAULT); ret != CURLE_OK)
    {
        std::stringstream ss;
        ss << "libcurl init error: " << ret;
        std::string str = ss.str();
        LOG_FATAL << str;
        panic("Error", str.c_str());
        return -1;
    }
    LOG_INFO << "libcurl version: " << curl_version();
    

    // load configs
    ConfigMgr::init();
    ConfigMgr::load();
    ConfigMgr::selectProfile(ConfigMgr::get('E', cfg::E_PROFILE, cfg::PROFILE_DEFAULT));
    SetLogLevel(ConfigMgr::get("E", cfg::E_LOG_LEVEL, 1));

    Path lr2path = Path(utf8_to_utf32(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2files/")));
    if (!fs::is_directory(lr2path))
    {
        LOG_FATAL << "LR2files directory not found! " << lr2path.u8string();
        panic("Error", "LR2files directory not found!");
        return -1;
    }

    // init imgui
    LOG_INFO << "Initializing ImGui...";
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    LOG_INFO << "ImGui version: " << ImGui::GetVersion();

    {
        ImGuiStyle& s = ImGui::GetStyle();

        s.WindowRounding = 0.f;
        s.WindowBorderSize = 0.f;
        s.FrameRounding = 0.f;
        s.FrameBorderSize = 1.f;

        auto& c = s.Colors;
        c[ImGuiCol_WindowBg] = { 0.f, 0.f, 0.f, 0.7f };

        c[ImGuiCol_FrameBg] = { 0.3f, 0.3f, 0.3f, 0.6f };
        c[ImGuiCol_FrameBgHovered] = { 0.6f, 0.6f, 0.6f, 0.6f };
        c[ImGuiCol_FrameBgActive] = { 0.5f, 0.5f, 0.5f, 1.0f };

        c[ImGuiCol_CheckMark] = { 0.f, 1.f, 0.f, 0.8f };

        c[ImGuiCol_Button] = { 0.f, 0.f, 0.f, 0.6f };
        c[ImGuiCol_ButtonHovered] = { 0.6f, 0.6f, 0.6f, 0.6f };
        c[ImGuiCol_ButtonActive] = { 0.5f, 0.5f, 0.5f, 1.0f };

        c[ImGuiCol_Header] = { 0.5f, 0.5f, 0.5f, 0.4f };
        c[ImGuiCol_HeaderHovered] = { 0.6f, 0.6f, 0.6f, 0.6f };
        c[ImGuiCol_HeaderActive] = { 0.5f, 0.5f, 0.5f, 1.0f };

        c[ImGuiCol_Tab] = { 0.f, 0.f, 0.f, 0.6f };
        c[ImGuiCol_TabHovered] = { 0.6f, 0.6f, 0.6f, 0.6f };
        c[ImGuiCol_TabActive] = { 0.5f, 0.5f, 0.5f, 1.0f };
        c[ImGuiCol_TabUnfocused] = { 0.f, 0.f, 0.f, 0.6f };
        c[ImGuiCol_TabUnfocusedActive] = { 0.5f, 0.5f, 0.5f, 0.8f };

    }

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

    // language
    i18n::init();
    i18n::setLanguage(ConfigMgr::get('P', cfg::P_LANGUAGE, "English"));

    // imgui font
    int fontIndex = 0;
    Path imguiFontPath = getSysFontPath(NULL, &fontIndex, i18n::getCurrentLanguage());
    if (!fs::exists(imguiFontPath))
    {
        LOG_FATAL << "Font file not found. Please reinstall the game.";
        panic("Error", "Font file not found. Please reinstall the game.");
        return -1;
    }
    ImFontConfig fontConfig;
    fontConfig.FontNo = fontIndex;
    ImFontAtlas& fontAtlas = *ImGui::GetIO().Fonts;
    fontAtlas.AddFontFromFileTTF(imguiFontPath.u8string().c_str(), 24, &fontConfig, fontAtlas.GetGlyphRangesChineseFull());

    // etc
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

    // load songs / tables at ScenePreSelect

    // arg parsing
    if (argc >= 2)
    {
        gNextScene = SceneType::PLAY;
        gQuitOnFinish = true;

        std::shared_ptr<ChartFormatBMS> bms = std::make_shared<ChartFormatBMS>(argv[1], std::time(NULL));
        gChartContext = ChartContextParams{
            argv[1],
            md5file(argv[1]),
            bms,
            nullptr,

            false,
            HashMD5(),
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
        gNextScene = SceneType::PRE_SELECT;
    }

    /*
#ifndef _DEBUG
    // preload all skins
    LOG_INFO << "==============================================";
    LOG_INFO << "Preload all skins";
    for (SkinType e = SkinType::TITLE; e < SkinType::MODE_COUNT; ++(*(int*)&e))
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
        LOG_ERROR << "OleInitialize Failed";
    }
#endif
    
    ///////////////////////////////////////////////////////////

    mainLoop();

    ///////////////////////////////////////////////////////////

    if (gArenaData.isOnline())
    {
        if (gArenaData.isClient())
        {
            if (gArenaData.isOnline())
                g_pArenaClient->leaveLobby();
            g_pArenaClient->loopEnd();
            g_pArenaClient.reset();
        }
        if (gArenaData.isServer())
        {
            if (gArenaData.isOnline())
                g_pArenaHost->disbandLobby();
            g_pArenaHost->loopEnd();
            g_pArenaHost.reset();
        }
    }

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

    curl_global_cleanup();

    return 0;
}


void mainLoop()
{
    gGenericInfo.loopStart();

    SceneType currentScene = SceneType::NOT_INIT;

    pScene scene = nullptr;
    while (!gEventQuit && currentScene != SceneType::EXIT && gNextScene != SceneType::EXIT)
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
                sceneCustomize = SceneMgr::get(SceneType::CUSTOMIZE);
                sceneCustomize->loopStart();
                sceneCustomize->inputLoopStart();
            }
            if (gExitingCustomize && sceneCustomize)
            {
                gInCustomize = false;
                sceneCustomize->inputLoopEnd();
                sceneCustomize->loopEnd();
                sceneCustomize.reset();
                gNextScene = SceneType::SELECT;
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
            if (currentScene != SceneType::EXIT && currentScene != SceneType::NOT_INIT)
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
