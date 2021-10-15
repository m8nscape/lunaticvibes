#include "scene_mgr.h"
#include "scene_context.h"
#include "scene_select.h"
#include "scene_decide.h"
#include "scene_play.h"
#include "scene_result.h"
#include "scene_play_retry_trans.h"
#include "game/skin/skin_mgr.h"
#include "game/ruleset/ruleset.h"

SceneMgr SceneMgr::_inst;

pScene SceneMgr::get(eScene e)
{
	pScene ps = nullptr;
    switch (e)
    {
    case eScene::EXIT:
    case eScene::NOTHINGNESS:
		return nullptr;

    case eScene::SELECT:
        return std::make_shared<SceneSelect>();

    case eScene::DECIDE:
        return std::make_shared<SceneDecide>();

    case eScene::PLAY:
        switch (gPlayContext.mode)
        {
        case eMode::PLAY5:
        case eMode::PLAY7:
        case eMode::PLAY9:
        case eMode::PLAY10:
        case eMode::PLAY14:
            ps = std::make_shared<ScenePlay>(ePlayMode::SINGLE);
			break;

        case eMode::PLAY5_2:
        case eMode::PLAY7_2:
        case eMode::PLAY9_2:
            ps = std::make_shared<ScenePlay>(ePlayMode::LOCAL_BATTLE);
			break;

        default:
            LOG_ERROR << "[Scene] Invalid mode: " << int(gPlayContext.mode);
            return nullptr;
        }
		break;

    case eScene::RETRY:
        ps = std::make_shared<ScenePlayRetryTrans>();
        break;

    case eScene::RESULT:
        switch (gPlayContext.mode)
        {
        case eMode::PLAY5:
        case eMode::PLAY7:
        case eMode::PLAY9:
        case eMode::PLAY10:
        case eMode::PLAY14:
            ps = std::make_shared<SceneResult>(ePlayMode::SINGLE);
            break;

        case eMode::PLAY5_2:
        case eMode::PLAY7_2:
        case eMode::PLAY9_2:
            ps = std::make_shared<SceneResult>(ePlayMode::LOCAL_BATTLE);
            break;

        default:
            LOG_ERROR << "[Scene] Invalid mode: " << int(gPlayContext.mode);
            return nullptr;
        }
        break;

	default:
		return nullptr;
    }

	return ps;
}

void SceneMgr::clean()
{
	SkinMgr::clean();
}