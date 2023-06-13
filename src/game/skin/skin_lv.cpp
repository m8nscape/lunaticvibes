#include "common/pch.h"
#include "skin_lv.h"

namespace lunaticvibes
{

SkinLv::SkinLv(SkinType type)
{
    _version = SkinVersion::LunaticVibes;
    info.mode = type;
    info.name = "Lunatic Vibes Default";
    info.maker = "rustbell";
}

SkinLv::~SkinLv()
{

}

}