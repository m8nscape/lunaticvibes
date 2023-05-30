#include "common/pch.h"
#include "config/config_mgr.h"
#include "game/graphics/graphics.h"
#include "game/sound/sound_mgr.h"
#include "game/scene/scene_mgr.h"
#include "game/input/input_mgr.h"
#include "game/skin/skin_lr2.h"
#include "game/skin/skin_mgr.h"
#include "game/runtime/generic_info.h"
#include "game/data/data_types.h"

#include "common/chartformat/chartformat_bms.h"

#include "game/runtime/i18n.h"

#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

#include "imgui.h"

#ifdef WIN32
#pragma comment(lib, "winmm.lib")
#include <consoleapi2.h>
#include <ole2.h>
#include <timeapi.h>
#endif //WIN32

#include <curl/curl.h>

namespace lunaticvibes
{

bool gEventQuit;
GenericInfoUpdater gGenericInfo{ 1 };

std::shared_ptr<SongDB> g_pSongDB;
std::shared_ptr<ScoreDB> g_pScoreDB;

void mainLoop();

}

// SDL_main
int main(int argc, char* argv[])
{
    using namespace lunaticvibes;

    SetDebugThreadName("MainThread");
    SetThreadAsMainThread();

    executablePath = GetExecutablePath();
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
    loadConfigs();

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
    SoundMgr::setVolume(SampleChannel::MASTER, SystemData.volumeMaster);
    SoundMgr::setVolume(SampleChannel::KEY, SystemData.volumeKey);
    SoundMgr::setVolume(SampleChannel::BGM, SystemData.volumeBgm);

    if (SystemData.fxType != FXType::Off)
        SoundMgr::setDSP(SystemData.fxType, 0, SampleChannel::MASTER, SystemData.fxVal, SystemData.fxVal);

    if (SystemData.freqType != FreqModifierType::Off)
    {
        static const double tick = std::pow(2, 1.0 / 12);
        double f = std::pow(tick, SystemData.freqVal);
        switch (SystemData.freqType)
        {
        case FreqModifierType::Frequency:
            SoundMgr::setFreqFactor(f);
            SystemData.pitchSpeed = f;
            break;
        case FreqModifierType::PitchOnly:
            SoundMgr::setFreqFactor(1.0);
            SoundMgr::setPitch(f);
            SystemData.pitchSpeed = 1.0;
            break;
        case FreqModifierType::SpeedOnly:
            SoundMgr::setFreqFactor(1.0);
            SoundMgr::setSpeed(f);
            SystemData.pitchSpeed = f;
            break;
        default:
            break;
        }
    }

    if (SystemData.equalizerEnabled)
    {
        SoundMgr::setEQ(EQFreq::_62_5, SystemData.equalizerVal62_5hz);
        SoundMgr::setEQ(EQFreq::_160, SystemData.equalizerVal160hz);
        SoundMgr::setEQ(EQFreq::_400, SystemData.equalizerVal400hz);
        SoundMgr::setEQ(EQFreq::_1000, SystemData.equalizerVal1khz);
        SoundMgr::setEQ(EQFreq::_2500, SystemData.equalizerVal2_5khz);
        SoundMgr::setEQ(EQFreq::_6250, SystemData.equalizerVal6_25khz);
        SoundMgr::setEQ(EQFreq::_16k, SystemData.equalizerVal16khz);
    }

    SelectData.scrollTimeLength = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);

    // load songs / tables at ScenePreSelect

    // arg parsing
    if (argc >= 2)
    {
        SystemData.gNextScene = SceneType::PLAY;
        SystemData.quitOnFinish = true;

        std::shared_ptr<ChartFormatBMS> bms = std::make_shared<ChartFormatBMS>(argv[1], std::time(NULL));
        SelectData.selectedChart = ChartMetadata{
            argv[1],
            md5file(argv[1]),
            bms,
            nullptr,

            false,
            HashMD5(),
            false,
            HashMD5(),
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
        SystemData.gNextScene = SceneType::PRE_SELECT;
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

    if (ArenaData.isOnline())
    {
        if (ArenaData.isClient())
        {
            if (ArenaData.isOnline())
                g_pArenaClient->leaveLobby();
            g_pArenaClient->loopEnd();
            g_pArenaClient.reset();
        }
        if (ArenaData.isServer())
        {
            if (ArenaData.isOnline())
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


void lunaticvibes::mainLoop()
{
    gGenericInfo.loopStart();

    SceneType currentScene = SceneType::NOT_INIT;

    pScene scene = nullptr;
    while (!gEventQuit && currentScene != SceneType::EXIT && SystemData.gNextScene != SceneType::EXIT)
    {
        // Evenet handling
        event_handle();
        if (gEventQuit)
        {
            SystemData.isAppExiting = true;
        }

        // Scene change
        if (SystemData.gNextScene != currentScene)
        {
            if (scene)
            {
                scene->inputLoopEnd();
                scene->loopEnd();
            }
            currentScene = SystemData.gNextScene;
            scene = SceneMgr::get(currentScene);
            scene->loopStart();
            scene->inputLoopStart();
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
