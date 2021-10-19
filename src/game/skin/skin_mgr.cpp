#include "skin_mgr.h"
#include "game/skin/skin_lr2.h"
#include "config/config_mgr.h"
#include "common/utils.h"

SkinMgr SkinMgr::_inst;

void SkinMgr::load(eMode e)
{
    auto& skinObj = _inst.c[static_cast<size_t>(e)];
    if (skinObj != nullptr)
        unload(e);

    Path skinFile;
    Path skinFileDefault;
    eSkinType type = eSkinType::LR2;

    // Get skin path from config
    switch (e)
    {
    case eMode::MUSIC_SELECT:
        skinFileDefault = cfg::S_DEFAULT_PATH_MUSIC_SELECT;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_MUSIC_SELECT, cfg::S_DEFAULT_PATH_MUSIC_SELECT);
        break;

    case eMode::DECIDE:
        skinFileDefault = cfg::S_DEFAULT_PATH_DECIDE;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_DECIDE, cfg::S_DEFAULT_PATH_DECIDE);
        break;

    case eMode::PLAY7:
        skinFileDefault = cfg::S_DEFAULT_PATH_PLAY_7;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_PLAY_7, cfg::S_DEFAULT_PATH_PLAY_7);
        break;

    case eMode::RESULT:
        skinFileDefault = cfg::S_DEFAULT_PATH_RESULT;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_RESULT, cfg::S_DEFAULT_PATH_RESULT);
        break;
    }

    if (!fs::is_regular_file(skinFile) && strEqual(skinFile.string().substr(0, 8), "LR2Files", false))
    {
        skinFile = ConfigMgr::get('P', cfg::P_LR2PATH, ".") + skinFile.string();
    }

    switch (type)
    {
    case eSkinType::LR2:
        skinObj = std::make_shared<SkinLR2>(skinFile);
        if (!skinObj->isLoaded())
            skinObj = std::make_shared<SkinLR2>(skinFileDefault);
        break;

    default:
        break;
    }
}

pSkin SkinMgr::get(eMode e)
{
    auto& inst = _inst.c[static_cast<size_t>(e)];
    if (inst == nullptr) load(e);
    return inst;
}
void SkinMgr::unload(eMode e)
{
    _inst.c[static_cast<size_t>(e)].reset();
}

void SkinMgr::clean()
{
	for (auto& s : gSprites)
	{
		s.reset();
	}
	for (eMode e = eMode::TITLE; e < eMode::MODE_COUNT; ++*((int*)&e))
	{
		unload(e);
	}
}