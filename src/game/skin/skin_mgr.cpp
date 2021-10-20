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

    Path skinFilePath;
    Path skinFilePathDefault;
    eSkinType type = eSkinType::LR2;

    // Get skin path from config
    switch (e)
    {
    case eMode::MUSIC_SELECT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_MUSIC_SELECT;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_MUSIC_SELECT, cfg::S_DEFAULT_PATH_MUSIC_SELECT);
        break;

    case eMode::DECIDE:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_DECIDE;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_DECIDE, cfg::S_DEFAULT_PATH_DECIDE);
        break;

    case eMode::PLAY7:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_7;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_7, cfg::S_DEFAULT_PATH_PLAY_7);
        break;

    case eMode::RESULT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_RESULT;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_RESULT, cfg::S_DEFAULT_PATH_RESULT);
        break;

    case eMode::KEY_CONFIG:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_KEYCONFIG;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_KEYCONFIG, cfg::S_DEFAULT_PATH_KEYCONFIG);
        break;
    }

    skinFilePath = convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), skinFilePath);

    switch (type)
    {
    case eSkinType::LR2:
        skinObj = std::make_shared<SkinLR2>(skinFilePath);
        if (!skinObj->isLoaded())
            skinObj = std::make_shared<SkinLR2>(skinFilePathDefault);
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