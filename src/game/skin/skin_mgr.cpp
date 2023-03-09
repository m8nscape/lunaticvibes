#include "skin_mgr.h"
#include "game/skin/skin_lr2.h"
#include "config/config_mgr.h"
#include "common/utils.h"

SkinMgr SkinMgr::_inst;

void SkinMgr::load(SkinType e, bool simple)
{
    auto& skinObj = _inst.c[static_cast<size_t>(e)];
    if (skinObj != nullptr)
        unload(e);

    Path skinFilePath;
    Path skinFilePathDefault;
    SkinVersion version = SkinVersion::LR2beta3;

    // Get skin path from config
    switch (e)
    {
    case SkinType::MUSIC_SELECT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_MUSIC_SELECT;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_MUSIC_SELECT, cfg::S_DEFAULT_PATH_MUSIC_SELECT);
        break;

    case SkinType::DECIDE:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_DECIDE;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_DECIDE, cfg::S_DEFAULT_PATH_DECIDE);
        break;

    case SkinType::RESULT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_RESULT;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_RESULT, cfg::S_DEFAULT_PATH_RESULT);
        break;

    case SkinType::COURSE_RESULT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_COURSE_RESULT;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_COURSE_RESULT, cfg::S_DEFAULT_PATH_COURSE_RESULT);
        break;

    case SkinType::KEY_CONFIG:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_KEYCONFIG;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_KEYCONFIG, cfg::S_DEFAULT_PATH_KEYCONFIG);
        break;

    case SkinType::THEME_SELECT:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_CUSTOMIZE;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_CUSTOMIZE, cfg::S_DEFAULT_PATH_CUSTOMIZE);
        break;

    case SkinType::PLAY5:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_5;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_5, cfg::S_DEFAULT_PATH_PLAY_5);
        break;

    case SkinType::PLAY5_2:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_5_BATTLE;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_5_BATTLE, cfg::S_DEFAULT_PATH_PLAY_5_BATTLE);
        break;

    case SkinType::PLAY7:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_7;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_7, cfg::S_DEFAULT_PATH_PLAY_7);
        break;

    case SkinType::PLAY7_2:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_7_BATTLE;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_7_BATTLE, cfg::S_DEFAULT_PATH_PLAY_7_BATTLE);
        break;

    case SkinType::PLAY9:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_9;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_9, cfg::S_DEFAULT_PATH_PLAY_9);
        break;

    case SkinType::PLAY10:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_10;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_10, cfg::S_DEFAULT_PATH_PLAY_10);
        break;

    case SkinType::PLAY14:
        skinFilePathDefault = cfg::S_DEFAULT_PATH_PLAY_14;
        skinFilePath = ConfigMgr::get("S", cfg::S_PATH_PLAY_14, cfg::S_DEFAULT_PATH_PLAY_14);
        break;

    default:
        version = SkinVersion::UNDEF;
        break;
    }

    skinFilePath = PathFromUTF8(convertLR2Path(ConfigMgr::get('E', cfg::E_LR2PATH, "."), skinFilePath));

    switch (version)
    {
    case SkinVersion::LR2beta3:
        skinObj = std::make_shared<SkinLR2>(skinFilePath, simple ? 1 : 0);
        if (!skinObj->isLoaded())
            skinObj = std::make_shared<SkinLR2>(skinFilePathDefault, simple ? 1 : 0);
        break;

    default:
        break;
    }

    if (simple)
    {
        _inst.shouldReload[(size_t)e] = true;
    }
}

pSkin SkinMgr::get(SkinType e)
{
    auto& inst = _inst.c[static_cast<size_t>(e)];
    //if (inst == nullptr) load(e);
    return inst;
}
void SkinMgr::unload(SkinType e)
{
    _inst.c[static_cast<size_t>(e)].reset();
    _inst.shouldReload[(size_t)e] = false;
}

void SkinMgr::clean()
{
	for (auto& s : gSprites)
	{
		s.reset();
	}
	for (SkinType e = SkinType::TITLE; e < SkinType::MODE_COUNT; ++*((int*)&e))
	{
		unload(e);
	}
}