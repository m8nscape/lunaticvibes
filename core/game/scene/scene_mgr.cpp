#include "scene_mgr.h"
#include "scene_context.h"
#include "scene_play.h"
#include "scene_result.h"
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
        switch (context_play.mode)
        {
        case eMode::PLAY5:
        case eMode::PLAY7:
        case eMode::PLAY9:
        case eMode::PLAY10:
        case eMode::PLAY14:
            return std::make_shared<ScenePlay>(ePlayMode::SINGLE);

        case eMode::PLAY5_2:
        case eMode::PLAY7_2:
        case eMode::PLAY9_2:
            return std::make_shared<ScenePlay>(ePlayMode::LOCAL_BATTLE);

        default:
            LOG_ERROR << "[Scene] Invalid mode: " << int(context_play.mode);
            return nullptr;
        }

    case eScene::RESULT:
        return std::make_shared<SceneResult>();

	default:
		return nullptr;
    }
}
