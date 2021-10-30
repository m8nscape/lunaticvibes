#include "scene_play_retry_trans.h"
#include "scene_context.h"

ScenePlayRetryTrans::ScenePlayRetryTrans() : vScene(eMode::RETRY_TRANS, 240)
{
	LOG_DEBUG << "[PlayRetryTrans]";
	clearContextPlayForRetry();
	gNextScene = eScene::PLAY;
}
