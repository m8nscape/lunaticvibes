#pragma once
#include "scene.h"

class ScenePlayCourseTrans : public SceneBase
{
public:
    ScenePlayCourseTrans();
    virtual ~ScenePlayCourseTrans() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override {}
};