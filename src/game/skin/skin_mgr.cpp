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
        skinFile = ConfigMgr::get("S", cfg::S_PATH_MUSIC_SELECT, Path(cfg::S_DEFAULT_PATH_MUSIC_SELECT).generic_string());
        break;

    case eMode::DECIDE:
        skinFileDefault = cfg::S_DEFAULT_PATH_DECIDE;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_DECIDE, Path(cfg::S_DEFAULT_PATH_DECIDE).generic_string());
        break;

    case eMode::PLAY7:
        skinFileDefault = cfg::S_DEFAULT_PATH_PLAY_7;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_PLAY_7, Path(cfg::S_DEFAULT_PATH_PLAY_7).generic_string());
        break;

    case eMode::RESULT:
        skinFileDefault = cfg::S_DEFAULT_PATH_RESULT;
        skinFile = ConfigMgr::get("S", cfg::S_PATH_RESULT, Path(cfg::S_DEFAULT_PATH_RESULT).generic_string());
        break;
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