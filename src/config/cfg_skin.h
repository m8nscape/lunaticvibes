#pragma once

#include "config.h"

// Stores in player folder
constexpr char CONFIG_FILE_SKIN[] = "skin.yml";

namespace cfg
{
    constexpr char S_PATH_MUSIC_SELECT[] = "Select";
    constexpr char S_DEFAULT_PATH_MUSIC_SELECT[] = "LR2files/Theme/LR2/Select/select.lr2skin";

    constexpr char S_PATH_DECIDE[] = "Decide";
    constexpr char S_DEFAULT_PATH_DECIDE[] = "LR2files/Theme/LR2/Decide/decide.lr2skin";

    constexpr char S_PATH_PLAY_5[] = "Play5";
    constexpr char S_DEFAULT_PATH_PLAY_5[] = "LR2files/Theme/LR2/Play/play_5.lr2skin";

    constexpr char S_PATH_PLAY_5_BATTLE[] = "Play5 Battle";
    constexpr char S_DEFAULT_PATH_PLAY_5_BATTLE[] = "LR2files/Theme/LR2/Play/play_5_battle.lr2skin";

    constexpr char S_PATH_PLAY_7[] = "Play7";
    constexpr char S_DEFAULT_PATH_PLAY_7[] = "LR2files/Theme/LR2/Play/play_7.lr2skin";
    //constexpr char S_DEFAULT_PATH_PLAY_7[] = "LR2files/test.lr2skin"_p;

    constexpr char S_PATH_PLAY_7_BATTLE[] = "Play7 Battle";
    constexpr char S_DEFAULT_PATH_PLAY_7_BATTLE[] = "LR2files/Theme/LR2/Play/play_7_battle.lr2skin";

    constexpr char S_PATH_PLAY_9[] = "Play9";
    constexpr char S_DEFAULT_PATH_PLAY_9[] = "LR2files/Theme/LR2/Play/play_9.lr2skin";

    constexpr char S_PATH_PLAY_10[] = "Play10";
    constexpr char S_DEFAULT_PATH_PLAY_10[] = "LR2files/Theme/LR2/Play/play_10.lr2skin";

    constexpr char S_PATH_PLAY_14[] = "Play14";
    constexpr char S_DEFAULT_PATH_PLAY_14[] = "LR2files/Theme/LR2/Play/play_14.lr2skin";

    constexpr char S_PATH_RESULT[] = "Result";
    constexpr char S_DEFAULT_PATH_RESULT[] = "LR2files/Theme/LR2/Result/result.lr2skin";

    constexpr char S_PATH_COURSE_RESULT[] = "CourseResult";
    constexpr char S_DEFAULT_PATH_COURSE_RESULT[] = "LR2files/Theme/LR2/CourseResult/courseresult.lr2skin";

    constexpr char S_PATH_KEYCONFIG[] = "Key Config";
    constexpr char S_DEFAULT_PATH_KEYCONFIG[] = "gamedata/resources/keyconfig/keyconfig.lr2skin";

    constexpr char S_PATH_CUSTOMIZE[] = "Customize";
    constexpr char S_DEFAULT_PATH_CUSTOMIZE[] = "LR2files/Theme/LR2/SkinSelect/skinselect.lr2skin";

    constexpr char S_PATH_SOUNDSET[] = "Soundset";
    constexpr char S_DEFAULT_PATH_SOUNDSET[] = "LR2files/Sound/lr2.lr2ss";
}

class ConfigSkin : public vConfig
{
public:
    ConfigSkin(const std::string& profile) : vConfig(profile, CONFIG_FILE_SKIN) {}
    virtual ~ConfigSkin() = default;

    virtual void setDefaults() noexcept override;
};
