#include "scene_exit_trans.h"
#include "scene_context.h"

SceneExitTrans::SceneExitTrans() : vScene(eMode::EXIT_TRANS, 240)
{
	_scene = eScene::EXIT_TRANS;

	LOG_DEBUG << "[ExitTrans]";
	gNextScene = eScene::EXIT;
}
