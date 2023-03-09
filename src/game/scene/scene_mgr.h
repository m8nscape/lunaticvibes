#pragma once
#include <memory>
#include "scene.h"
#include "scene_context.h"

typedef std::shared_ptr<SceneBase> pScene;

class SceneMgr
{
private:
    static SceneMgr _inst;
private:
    SceneMgr() = default;
    ~SceneMgr() = default;
    SceneMgr(SceneMgr&) = delete;
    SceneMgr& operator= (SceneMgr&) = delete;

public:
    static pScene get(SceneType);
	static void clean();
};
