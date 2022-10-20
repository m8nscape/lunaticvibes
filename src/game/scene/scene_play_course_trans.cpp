#include "scene_play_course_trans.h"
#include "scene_context.h"

ScenePlayCourseTrans::ScenePlayCourseTrans() : vScene(eMode::COURSE_TRANS, 240)
{
	_scene = eScene::COURSE_TRANS;

	LOG_DEBUG << "[PlayCourseTrans]";
	clearContextPlayForRetry();
	gNextScene = eScene::PLAY;
}
