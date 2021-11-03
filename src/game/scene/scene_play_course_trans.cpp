#include "scene_play_course_trans.h"
#include "scene_context.h"

ScenePlayCourseTrans::ScenePlayCourseTrans() : vScene(eMode::COURSE_TRANS, 240)
{
	LOG_DEBUG << "[PlayCourseTrans]";
	clearContextPlayForRetry();
	gNextScene = eScene::SELECT;
}
