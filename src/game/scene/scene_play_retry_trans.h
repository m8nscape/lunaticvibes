#pragma once
#include "scene.h"

class ScenePlayRetryTrans : public SceneBase
{
public:
    ScenePlayRetryTrans();
    virtual ~ScenePlayRetryTrans() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override {}
};