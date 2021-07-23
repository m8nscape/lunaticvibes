#include "common/meta.h"
#include "config/config_mgr.h"
#include "game/graphics/graphics.h"
#include "game/sound/sound_mgr.h"
#include "game/scene/scene_mgr.h"
#include "game/input/input_mgr.h"
#include "game/skin/skin_lr2.h"
#include "common/utils.h"
#include "common/sysutil.h"
#include "game/scene/scene_context.h"
#include "game/generic_info.h"
#include "game/graphics/dxa.h"

#include "common/chartformat/format_bms.h"

#include "common/log.h"

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
                using e = eScene;
            case e::SELECT: LOG_INFO << "Scene changed to SELECT"; break;
            case e::DECIDE: LOG_INFO << "Scene changed to DECIDE"; break;
            case e::PLAY: LOG_INFO << "Scene changed to PLAY"; break;
            case e::RESULT: LOG_INFO << "Scene changed to RESULT"; break;
            case e::EXIT: LOG_INFO << "Scene changed to EXIT"; break;
            default: break;
            }

			scene->loopEnd();
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

int main(int argc, char* argv[])
{
    SetThreadName("MainThread");

    char exePath[256];
    size_t exePathLen;
    GetExecutablePath(exePath, 256, exePathLen);
    executablePath = fs::path(exePath);
    fs::current_path(executablePath);

    // init logger
    InitLogger();

    ConfigMgr::selectProfile(PROFILE_DEFAULT);

    if (auto ginit = graphics_init())
        return ginit;
    if (auto sinit = SoundMgr::initFMOD())
        return sinit;

    // extract dxa
    for (auto& it : std::filesystem::recursive_directory_iterator("./LR2Files/Theme"))
    {
        if (std::filesystem::is_regular_file(it))
        {
            auto ext = it.path().extension().string();
            for (char& c: ext) c = std::tolower(c);
            if (ext == ".dxa")
            {
                LOG_INFO << "Extracting DXArchive file " << it.path().string() << "...";
                extractDxaToFile(it.path());
            }
        }
    }

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
    ConfigMgr::setGlobals();

    // initialize song list
    g_pSongDB = std::make_shared<SongDB>("database/song.db");
    g_pSongDB->addFolder("song");
    SongListProperties rootFolderProp{
        "",
        ROOT_FOLDER_HASH,
        "",
        {},
        0
    };
    auto top = g_pSongDB->browse(ROOT_FOLDER_HASH, false);
    for (size_t i = 0; i < top.getContentsCount(); ++i)
        rootFolderProp.list.push_back(top.getEntry(i));

    gSelectContext.backtrace.push(rootFolderProp);

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
#endif

    mainLoop();

    SceneMgr::clean ();	// clean resources before releasing framework
    graphics_free();

#ifdef WIN32
    timeEndPeriod(1);
#endif

    ConfigMgr::save();

    return 0;
}
