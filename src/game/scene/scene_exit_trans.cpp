#include "common/pch.h"
#include "scene_exit_trans.h"
#include "scene_context.h"

SceneExitTrans::SceneExitTrans() : SceneBase(SkinType::EXIT_TRANS, 240)
{
	_type = SceneType::EXIT_TRANS;

	LOG_DEBUG << "[ExitTrans]";
	gNextScene = SceneType::EXIT;
}
