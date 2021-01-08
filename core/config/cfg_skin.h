#pragma once

#include "config.h"

// Stores in player folder
inline const StringPath CONFIG_FILE_SKIN = "skin.yml"_p;

namespace cfg
{
    inline const StringContent S_PATH_MUSIC_SELECT = "Select";
    inline const StringPath S_DEFAULT_PATH_MUSIC_SELECT = "./LR2files/Theme/LR2/Play/play_7.lr2skin"_p;

    inline const StringContent S_PATH_PLAY_7 = "Play7";
    inline const StringPath S_DEFAULT_PATH_PLAY_7 = "./LR2files/Theme/LR2/Play/play_7.lr2skin"_p;
    //inline const StringPath S_DEFAULT_PATH_PLAY_7 = "./LR2files/test.lr2skin"_p;

    inline const StringContent S_PATH_RESULT = "Result";
    inline const StringPath S_DEFAULT_PATH_RESULT = "./LR2files/Theme/LR2/Result/result.lr2skin"_p;
}

class ConfigSkin : public vConfig
{
public:
    ConfigSkin(const std::string& profile) : vConfig(profile, CONFIG_FILE_SKIN) {}
    virtual ~ConfigSkin() = default;

    virtual void setDefaults() noexcept override;
};
