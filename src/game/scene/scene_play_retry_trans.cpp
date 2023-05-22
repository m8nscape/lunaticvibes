#include "common/pch.h"
#include "scene_play_retry_trans.h"
#include "game/data/data_system.h"
#include "game/data/data_play.h"

namespace lunaticvibes
{

ScenePlayRetryTrans::ScenePlayRetryTrans() : SceneBase(SkinType::RETRY_TRANS, 240)
{
	_type = SceneType::RETRY_TRANS;

	LOG_DEBUG << "[PlayRetryTrans]";
	PlayData.clearContextPlayForRetry();
	SystemData.gNextScene = SceneType::PLAY;
}

}
