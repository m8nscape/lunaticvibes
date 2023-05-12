#pragma once
#include "scene.h"

namespace lunaticvibes
{

class ScenePlayCourseTrans : public SceneBase
{
public:
    ScenePlayCourseTrans();
    virtual ~ScenePlayCourseTrans() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override {}
};

}
