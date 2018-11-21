#include "graphics/graphics.h"
#include "config/config_mgr.h"
#include "sound/sound_mgr.h"
#include "scene/scene_mgr.h"
#include "skin/skin_lr2.h"

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
    // TODO load config
    if (auto ginit = graphics_init())
        return ginit;
    if (auto sinit = SoundMgr::initFMOD())
        return sinit;

	// load input bindings
	InputMgr::updateDevices();
	InputMgr::updateBindings(Input::k::S1L);

    if (argc > 1)
        __arg_path = argv[1];

#if WIN32
    timeBeginPeriod(1);
#endif

    mainLoop();

    graphics_free();

#if WIN32
    timeEndPeriod(1);
#endif

    return 0;
}
