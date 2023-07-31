#include "common/pch.h"
#include "scene_mgr.h"
#include "scene_pre_select.h"
#include "scene_select.h"
#include "scene_decide.h"
#include "scene_play.h"
#include "scene_result.h"
#include "scene_play_course_trans.h"
#include "scene_play_retry_trans.h"
#include "scene_course_result.h"
#include "scene_keyconfig.h"
#include "scene_exit_trans.h"
#include "game/skin/skin_mgr.h"
#include "game/ruleset/ruleset.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

SceneMgr SceneMgr::_inst;

pScene SceneMgr::get(SceneType e)
{
    SystemData.timers["scene_start"] = TIMER_NEVER;
    SystemData.timers["input_start"] = TIMER_NEVER;
    PlayData.timers["load_start"] = TIMER_NEVER;
    PlayData.timers["ready"] = TIMER_NEVER;
    PlayData.timers["play_start"] = TIMER_NEVER;

    pScene ps = nullptr;
    switch (e)
    {
    case SceneType::EXIT:
    case SceneType::NOT_INIT:
        break;

    case SceneType::PRE_SELECT:
        ps = std::make_shared<ScenePreSelect>();
        break;

    case SceneType::SELECT:
        ps = std::make_shared<SceneSelect>();
        break;

    case SceneType::DECIDE:
        ps = std::make_shared<SceneDecide>();
        break;

    case SceneType::PLAY:
        switch (PlayData.mode)
        {
        case SkinType::PLAY5:
        case SkinType::PLAY7:
        case SkinType::PLAY9:
        case SkinType::PLAY10:
        case SkinType::PLAY14:
        case SkinType::PLAY5_2:
        case SkinType::PLAY7_2:
        case SkinType::PLAY9_2:
            ps = std::make_shared<ScenePlay>();
            break;

        default:
            LOG_ERROR << "[Scene] Invalid mode: " << int(PlayData.mode);
            return nullptr;
        }
        break;

    case SceneType::RETRY_TRANS:
        ps = std::make_shared<ScenePlayRetryTrans>();
        break;

    case SceneType::RESULT:
        switch (PlayData.mode)
        {
        case SkinType::PLAY5:
        case SkinType::PLAY7:
        case SkinType::PLAY9:
        case SkinType::PLAY10:
        case SkinType::PLAY14:
        case SkinType::PLAY5_2:
        case SkinType::PLAY7_2:
        case SkinType::PLAY9_2:
            ps = std::make_shared<SceneResult>();
            break;

        default:
            LOG_ERROR << "[Scene] Invalid mode: " << int(PlayData.mode);
            return nullptr;
        }
        break;

    case SceneType::COURSE_TRANS:
        ps = std::make_shared<ScenePlayCourseTrans>();
        break;

    case SceneType::KEYCONFIG:
        ps = std::make_shared<SceneKeyConfig>();
        break;

    case SceneType::CUSTOMIZE:
        break;

    case SceneType::COURSE_RESULT:
        ps = std::make_shared<SceneCourseResult>();
        break;

    case SceneType::EXIT_TRANS:
        ps = std::make_shared<SceneExitTrans>();
        break;

    default:
        return nullptr;
    }

    _inst.currentScene = ps;
    return ps;
}

pScene SceneMgr::current()
{
    return _inst.currentScene;
}

void SceneMgr::clean()
{
    SkinMgr::clean();
}

}
