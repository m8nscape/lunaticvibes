#pragma once
#include <memory>
#include "scene.h"
#include "scene_play.h"

enum class eScene
{
    NOTHINGNESS,
    PLAY,
    EXIT
};

inline char* __arg_path = "";
inline eScene __next_scene = eScene::PLAY;

typedef std::shared_ptr<vScene> pScene;

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
    static pScene get(eScene);
};
