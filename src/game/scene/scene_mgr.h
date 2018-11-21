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

inline static char* __arg_path = "";
inline static eScene __next_scene = eScene::PLAY;

typedef std::shared_ptr<vScene> pScene;

class SceneMgr
{
private:
    static SceneMgr _inst;
    SceneMgr() = default;
    ~SceneMgr() = default;
public:
    SceneMgr(SceneMgr&) = delete;
    SceneMgr& operator= (SceneMgr&) = delete;

public:
    static pScene get(eScene);
};
