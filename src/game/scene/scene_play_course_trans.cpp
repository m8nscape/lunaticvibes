#include "common/pch.h"
#include "scene_play_course_trans.h"
#include "scene_context.h"

namespace lunaticvibes
{

ScenePlayCourseTrans::ScenePlayCourseTrans() : SceneBase(SkinType::COURSE_TRANS, 240)
{
	_type = SceneType::COURSE_TRANS;

	LOG_DEBUG << "[PlayCourseTrans]";
	clearContextPlayForRetry();
	SystemData.gNextScene = SceneType::PLAY;
}

}
