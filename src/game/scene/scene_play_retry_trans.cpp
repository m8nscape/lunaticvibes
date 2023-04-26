#include "common/pch.h"
#include "scene_play_retry_trans.h"
#include "scene_context.h"

ScenePlayRetryTrans::ScenePlayRetryTrans() : SceneBase(SkinType::RETRY_TRANS, 240)
{
	_type = SceneType::RETRY_TRANS;

	LOG_DEBUG << "[PlayRetryTrans]";
	clearContextPlayForRetry();
	gNextScene = SceneType::PLAY;
}
