#pragma once

#include "config.h"

// Stores in player folder
inline const StringPath CONFIG_FILE_SKIN = "skin.yml";

namespace cfg
{
    inline const StringContent S_PATH_PLAY_7 = "Play7";
    inline const StringContent S_DEFAULT_PATH_PLAY_7 = "./LR2files/Theme/LR2/Play/play_7.lr2skin";

    inline const StringContent S_PATH_RESULT = "Result";
    inline const StringContent S_DEFAULT_PATH_RESULT = "./LR2files/Theme/LR2/Result/result.lr2skin";
}

class ConfigSkin : public vConfig
{
public:
    ConfigSkin();
    virtual ~ConfigSkin();

    virtual void setDefaults() noexcept override;
};
