#include "common/pch.h"
#include "scene_play_course_trans.h"
#include "game/data/data_system.h"
#include "game/data/data_play.h"

namespace lunaticvibes
{

ScenePlayCourseTrans::ScenePlayCourseTrans() : SceneBase(SkinType::COURSE_TRANS, 240)
{
	_type = SceneType::COURSE_TRANS;

	LOG_DEBUG << "[PlayCourseTrans]";
	PlayData.clearContextPlayForRetry();
	SystemData.gNextScene = SceneType::PLAY;
}

}
