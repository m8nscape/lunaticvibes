#include "scene.h"
#include "beat.h"
#include "game/data/timer.h"
#include "game/data/switch.h"
#include "game/data/number.h"
#include "game/data/slider.h"
#include "game/skin/skin_mgr.h"

// prototype
vScene::vScene(eMode mode, unsigned rate) : _input(), AsyncLooper(std::bind(&vScene::_updateAsync, this), rate)
{
    _skin = SkinMgr::get(mode);

    gNumbers::reset();
    gSwitches::reset();
    gSliders::reset();
    gTimers::reset();
    auto t = getTimePoint();
    gTimers::set(eTimer::SCENE_START, t);
    gTimers::set(eTimer::START_INPUT, t + _skin->info.timeIntro);
}

void vScene::update()
{
    _skin->update();
}

void vScene::draw() const
{
    _skin->draw();
}
