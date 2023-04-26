#include "common/pch.h"
#include "scene_mgr.h"
#include "scene_context.h"
#include "scene_pre_select.h"
#include "scene_select.h"
#include "scene_decide.h"
#include "scene_play.h"
#include "scene_result.h"
#include "scene_play_course_trans.h"
#include "scene_play_retry_trans.h"
#include "scene_course_result.h"
#include "scene_keyconfig.h"
#include "scene_customize.h"
#include "scene_exit_trans.h"
#include "game/skin/skin_mgr.h"
#include "game/ruleset/ruleset.h"

SceneMgr SceneMgr::_inst;

pScene SceneMgr::get(SceneType e)
{
    State::set(IndexTimer::SCENE_START, TIMER_NEVER);
    State::set(IndexTimer::START_INPUT, TIMER_NEVER);
    State::set(IndexTimer::_LOAD_START, TIMER_NEVER);
    State::set(IndexTimer::PLAY_READY, TIMER_NEVER);
    State::set(IndexTimer::PLAY_START, TIMER_NEVER);

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
        switch (gPlayContext.mode)
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
            LOG_ERROR << "[Scene] Invalid mode: " << int(gPlayContext.mode);
            return nullptr;
        }
        break;

    case SceneType::RETRY_TRANS:
        ps = std::make_shared<ScenePlayRetryTrans>();
        break;

    case SceneType::RESULT:
        switch (gPlayContext.mode)
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
            LOG_ERROR << "[Scene] Invalid mode: " << int(gPlayContext.mode);
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
        ps = std::make_shared<SceneCustomize>();
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

    Time t;
    State::set(IndexTimer::SCENE_START, t.norm());
    State::set(IndexTimer::START_INPUT, t.norm() + (ps ? ps->getSkinInfo().timeIntro : 0));

	return ps;
}

void SceneMgr::clean()
{
	SkinMgr::clean();
}