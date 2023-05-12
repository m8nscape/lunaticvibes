#pragma once
#include "skin.h"
#include "../scene/scene.h"

namespace lunaticvibes
{

/*
* Designed as a minimized customization skin model, similar to the circle clicking game.
* Too much freedom leads to a much more complex developing procedure.
* I have not fully implemented LR2Skin compability. Yet. That was a beast. Many features are held up because I have to extend LR2Skin definitions first. And sadly I don't have much time and tension for that anymore. Plus everyone are just going to make beatoraja skins. Who wants to read a merciless thousands lines of #DST_IMAGE,0,0,0,0,0,0,255,255,255,255,0,0,0? Reading bms files is already a painful work.
*/
class SkinLv : public SkinBase
{
public:
    SkinLv() = delete;
    SkinLv(SkinType type);
    virtual ~SkinLv();
};

}
