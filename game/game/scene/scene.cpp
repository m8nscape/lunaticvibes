#include <execution>
#include "scene.h"
#include "beat.h"
#include "game/data/timer.h"
#include "game/data/switch.h"
#include "game/data/number.h"
#include "game/data/slider.h"
#include "game/data/text.h"
#include "game/skin/skin_mgr.h"

// prototype
vScene::vScene(eMode mode, unsigned rate) : _input(), AsyncLooper(std::bind(&vScene::_updateAsync, this), rate, true)
{
    _skin = SkinMgr::get(mode);
	Time t;

    //gNumbers.reset();
    gSliders.reset();

    gSwitches.set(eSwitch::_TRUE, false);
    gSwitches.reset();
    gSwitches.set(eSwitch::_TRUE, true);

    gTimers.set(eTimer::SCENE_START, -1);
    gTimers.reset();
    gTimers.set(eTimer::SCENE_START, t.norm());
    gTimers.set(eTimer::START_INPUT, t.norm() + _skin->info.timeIntro);
}

void vScene::update()
{
    _skin->update();
}

void vScene::draw() const
{
    _skin->draw();
}

int vScene::getVideoSlot() const
{
	for (size_t i = 0; i < SCENE_VIDEO_SLOT_MAX; ++i)
	{
		if (!_video[i].haveVideo) return (int)i;
	}
	return -1;
}

void vScene::restartVideos()
{
#ifdef _DEBUG
	std::for_each(std::execution::seq, _video.begin(), _video.end(), [](auto& v) {v.seek(0); });
#else
	std::for_each(std::execution::par_unseq, _video.begin(), _video.end(), [](auto& v) {v.seek(0); });
#endif
}

void vScene::restartSkinVideos()
{
	if (!_skin) return;
#ifdef _DEBUG
	std::for_each(std::execution::seq, _skin->_video.begin(), _skin->_video.end(), [](auto& v) {v.seek(0); });
#else
	std::for_each(std::execution::par_unseq, _skin->_video.begin(), _skin->_video.end(), [](auto& v) {v.seek(0); });
#endif
}

void vScene::playSkinVideos()
{
	if (!_skin) return;
#ifdef _DEBUG
	std::for_each(std::execution::seq, _skin->_video.begin(), _skin->_video.end(), [](auto& v) {v.startPlaying(); });
#else
	std::for_each(std::execution::par_unseq, _skin->_video.begin(), _skin->_video.end(), [](auto& v) {v.startPlaying(); });
#endif
}