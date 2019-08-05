#pragma once

#include "config.h"

// Stores in player folder
inline const StringPath CONFIG_FILE_SKIN = "skin.yml";

namespace cfg
{
    inline const StringContent S_PATH_PLAY = "Play";
    inline const StringContent S_DEFAULT_PATH_PLAY = "./LR2files/Theme/LR2/Play/play_7.lr2skin";
}

class ConfigSkin : public vConfig
{
public:
    ConfigSkin();
    virtual ~ConfigSkin();

    virtual void setDefaults() noexcept override;
};
