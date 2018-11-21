#include "scene_mgr.h"
#include "game/ruleset/ruleset.h"

SceneMgr SceneMgr::_inst;

pScene SceneMgr::get(eScene e)
{
    switch (e)
    {
    case eScene::EXIT:
    case eScene::NOTHINGNESS:
        return nullptr;
    case eScene::PLAY:
        return std::make_shared<ScenePlay>(ePlayMode::SINGLE, 7, StringPath(__argv[1]), eRuleset::CLASSIC);

    }
}
