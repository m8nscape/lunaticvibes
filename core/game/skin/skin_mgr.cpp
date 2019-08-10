#include "skin_mgr.h"
#include "game/skin/skin_lr2.h"
#include "config/config_mgr.h"
#include "utils.h"

SkinMgr SkinMgr::_inst;

void SkinMgr::load(eMode e)
{
    auto& skinObj = _inst.c[static_cast<size_t>(e)];
    if (skinObj != nullptr)
        unload(e);

    Path skinFile(executablePath / fs::path(
        ConfigMgr::get(cfg::S_PATH_PLAY, cfg::S_DEFAULT_PATH_PLAY))); // FIXME load from config
    eSkinType type = eSkinType::LR2; // FIXME load from config
    switch (type)
    {
    case eSkinType::LR2:
    {
        skinObj = std::make_shared<SkinLR2>(skinFile);
        if (!skinObj->isLoaded())
            /* Initialize with default skin path */;
        break;
    }
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