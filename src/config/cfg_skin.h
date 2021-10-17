#pragma once

#include "config.h"

// Stores in player folder
constexpr char CONFIG_FILE_SKIN[] = "skin.yml";

namespace cfg
{
    constexpr char S_PATH_MUSIC_SELECT[] = "Select";
    constexpr char S_DEFAULT_PATH_MUSIC_SELECT[] = "LR2files/Theme/LR2/Decide/decide.lr2skin";

    constexpr char S_PATH_DECIDE[] = "Decide";
    constexpr char S_DEFAULT_PATH_DECIDE[] = "LR2files/Theme/LR2/Decide/decide.lr2skin";

    constexpr char S_PATH_PLAY_7[] = "Play7";
    constexpr char S_DEFAULT_PATH_PLAY_7[] = "LR2files/Theme/LR2/Play/play_7.lr2skin";
    //constexpr char S_DEFAULT_PATH_PLAY_7[] = "LR2files/test.lr2skin"_p;

    constexpr char S_PATH_RESULT[] = "Result";
    constexpr char S_DEFAULT_PATH_RESULT[] = "LR2files/Theme/LR2/Result/result.lr2skin";

    constexpr char S_PATH_BGM[] = "BGM folder (LR2)";
    constexpr char S_DEFAULT_PATH_BGM[] = "LR2files/Bgm/LR2 ver sta";

    constexpr char S_PATH_SOUND[] = "Sound folder (LR2)";
    constexpr char S_DEFAULT_PATH_SOUND[] = "LR2files/Sound/lr2";
}

class ConfigSkin : public vConfig
{
public:
    ConfigSkin(const std::string& profile) : vConfig(profile, CONFIG_FILE_SKIN) {}
    virtual ~ConfigSkin() = default;

    virtual void setDefaults() noexcept override;
};
