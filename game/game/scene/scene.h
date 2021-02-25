#pragma once
#include <bitset>
#include <string>
#include <memory>
#include <array>
#include "game/skin/skin.h"
#include "game/data/timer.h"
#include "game/data/switch.h"
#include "game/data/number.h"
#include "game/data/slider.h"
#include "game/input/input_wrapper.h"
#include "game/graphics/video.h"
#include "types.h"

// Parent class of scenes, defines how an object being stored and drawn.
// Every classes of scenes should inherit this class.
class vScene: public AsyncLooper
{
protected:
    std::shared_ptr<vSkin> _skin;
    InputWrapper _input;

public:
	bool sceneEnding = false;

public:
	const static size_t SCENE_VIDEO_SLOT_MAX = 32;
	std::array<sVideo, SCENE_VIDEO_SLOT_MAX> _video;
	int getVideoSlot() const;
	void restartVideos();
	void restartSkinVideos();
	void playSkinVideos();

public:
    vScene(eMode mode, unsigned rate = 240);

public:
    vScene() = delete;
	virtual ~vScene() { sceneEnding = true; loopEnd(); }
    virtual void update();      // skin update
    virtual void draw() const;

protected:
    virtual void _updateAsync() = 0;
};
