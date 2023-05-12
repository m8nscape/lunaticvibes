#include "common/pch.h"
#include "soundset.h"
#include "config/config_mgr.h"

namespace lunaticvibes
{

Path vSoundSet::getPathBGMSelect() const
{
	return convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2files/Bgm/LR2 ver sta/select.wav");
}

Path vSoundSet::getPathBGMDecide() const
{
	return convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), "LR2files/Bgm/LR2 ver sta/decide.wav");
}

}
