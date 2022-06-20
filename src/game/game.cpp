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
#include "game/graphics/dxa.h"

#include "common/chartformat/chartformat_bms.h"

#include "common/log.h"

#include "imgui.h"

#ifdef WIN32
#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#endif //WIN32

#include <regex>

bool gEventQuit;
GenericInfoUpdater gGenericInfo{ 1 };

void mainLoop()
{
    eScene currentScene = gNextScene;
    auto scene = SceneMgr::get(currentScene);
	gGenericInfo.loopStart();
    while (!gEventQuit && currentScene != eScene::EXIT && gNextScene != eScene::EXIT)
    {
        // Evenet handling
        event_handle();
        if (gEventQuit)
            gNextScene = eScene::EXIT;

        // Scene change
        if (currentScene != gNextScene)
        {
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
			scene = SceneMgr::get(currentScene);
			if (currentScene == eScene::EXIT)
				break;
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

        // sound update (temporary)
        SoundMgr::update();
    }
	gGenericInfo.loopEnd();
}

// SDL_main
int main(int argc, char* argv[])
{
    SetThreadName("MainThread");
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

    // extract dxa
    for (auto& it : std::filesystem::recursive_directory_iterator(
        convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2Files/Theme")))
    {
        if (std::filesystem::is_regular_file(it))
        {
            auto ext = it.path().extension().u8string();
            for (char& c: ext) c = std::tolower(c);
            if (ext == ".dxa")
            {
                LOG_INFO << "Extracting DXArchive file " << it.path().u8string() << "...";
                extractDxaToFile(it.path());
            }
        }
    }

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
    gTimers.setDefault(eTimer::_NEVER, LLONG_MAX);

    gNumbers.reset();
    gSwitches.reset();
    gSliders.reset();
    gTexts.reset();
    gTimers.reset();
    ConfigMgr::setGlobals();

    // score db
    std::string scoreDBPath = (ConfigMgr::Profile()->getPath() / "score.db").u8string();
    g_pScoreDB = std::make_shared<ScoreDB>(scoreDBPath.c_str());

    // initialize song list
    Path dbPath = Path(GAMEDATA_PATH) / "database";
    if (!fs::exists(dbPath)) fs::create_directories(dbPath);
    g_pSongDB = std::make_shared<SongDB>(dbPath / "song.db");

    // get folders from config
    for (auto& f : ConfigMgr::General()->getFoldersPath())
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
        rootFolderProp.list.push_back({ top.getEntry(i), nullptr });
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

#ifdef WIN32
    timeBeginPeriod(1);
    OleInitialize(NULL);
#endif

    mainLoop();

#ifdef WIN32
    OleUninitialize();
    timeEndPeriod(1);
#endif

    SceneMgr::clean();	// clean resources before releasing framework
    graphics_free();

    ImGui::DestroyContext();

    ConfigMgr::save();

    StopHandleMainThreadTask();
    return 0;
}
