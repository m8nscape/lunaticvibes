#pragma once
#include "scene.h"

namespace lunaticvibes
{

class SceneExitTrans : public SceneBase
{
public:
    SceneExitTrans();
    virtual ~SceneExitTrans() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override {}
};

}
