#pragma once
#include "skin.h"

namespace lunaticvibes
{

typedef std::shared_ptr<SkinBase> pSkin;

class SkinMgr
{
private:
    static SkinMgr _inst;
    SkinMgr() = default;
    ~SkinMgr() = default;
public:
    SkinMgr(SkinMgr&) = delete;
    SkinMgr& operator= (SkinMgr&) = delete;

protected:
    std::array<pSkin, static_cast<size_t>(SkinType::MODE_COUNT)> c{};
    std::array<bool, static_cast<size_t>(SkinType::MODE_COUNT)> shouldReload{ false };

public:
    static void load(SkinType, bool simple = false);
    static void unload(SkinType);
    static pSkin get(SkinType);
    static void clean();
};

}
