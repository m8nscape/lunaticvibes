#pragma once
#include "scene.h"

namespace lunaticvibes
{

class ScenePlayRetryTrans : public SceneBase
{
public:
    ScenePlayRetryTrans();
    virtual ~ScenePlayRetryTrans() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override {}
};

}
