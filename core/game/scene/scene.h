#pragma once
#include <bitset>
#include <string>
#include <memory>
#include <mutex>
#include "game/skin/skin.h"
#include "game/data/timer.h"
#include "game/data/switch.h"
#include "game/data/number.h"
#include "game/data/slider.h"
#include "game/input/input_wrapper.h"
#include "types.h"

// Parent class of scenes, defines how an object being stored and drawn.
// Every classes of scenes should inherit this class.
class vScene: public AsyncLooper
{
protected:
	std::mutex _mutex;

protected:
    std::shared_ptr<vSkin> _skin;
    InputWrapper _input;

public:
    vScene(eMode mode, unsigned rate = 240);

public:
    vScene() = delete;
    virtual ~vScene() = default;
    virtual void update();      // skin update
    virtual void draw() const;

protected:
    virtual void _updateAsync() = 0;
};
