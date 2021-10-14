#pragma once
#include "scene.h"

class ScenePlayRetryTrans : public vScene
{
public:
    ScenePlayRetryTrans();
    virtual ~ScenePlayRetryTrans() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override {}
};