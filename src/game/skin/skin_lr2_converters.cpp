#include "skin_lr2_converters.h"

namespace lr2skin
{

eNumber num(int n)
{
    return eNumber(n);
}

eTimer timer(int n)
{
    return eTimer(n);
}

eText text(int n)
{
    return eText(n);
}

static const std::vector<std::variant<std::monostate, eSwitch, eOption>> buttonAdapter{
    // 0
    std::monostate(),

    // 1~9
    eSwitch::SELECT_PANEL1,
    eSwitch::SELECT_PANEL2,
    eSwitch::SELECT_PANEL3,
    eSwitch::SELECT_PANEL4,
    eSwitch::SELECT_PANEL5,
    eSwitch::SELECT_PANEL6,
    eSwitch::SELECT_PANEL7,
    eSwitch::SELECT_PANEL8,
    eSwitch::SELECT_PANEL9,

    // 10~12
    eOption::SELECT_FILTER_DIFF,
    eOption::SELECT_FILTER_KEYS,
    eOption::SELECT_SORT,

    // 13~19
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,

    // 20~28
    eOption::SOUND_FX0,
    eOption::SOUND_FX1,
    eOption::SOUND_FX2,
    eSwitch::SOUND_FX0,
    eSwitch::SOUND_FX1,
    eSwitch::SOUND_FX2,
    eOption::SOUND_TARGET_FX0,
    eOption::SOUND_TARGET_FX1,
    eOption::SOUND_TARGET_FX2,

    //29~30
    eSwitch::_FALSE,	// EQ off/on
    eSwitch::_FALSE,	// EQ Preset

    //31~33
    eSwitch::SOUND_VOLUME,		// volume control
    eSwitch::SOUND_PITCH,
    eOption::SOUND_PITCH_TYPE,

    //34~39
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,

    //40~45
    eOption::PLAY_GAUGE_TYPE_1P,
    eOption::PLAY_GAUGE_TYPE_2P,
    eOption::PLAY_RANDOM_TYPE_1P,
    eOption::PLAY_RANDOM_TYPE_2P,
    eSwitch::PLAY_OPTION_AUTOSCR_1P,
    eSwitch::PLAY_OPTION_AUTOSCR_2P,

    //46~49
    eSwitch::_FALSE,	// shutter?
    eSwitch::_FALSE,
    eSwitch::_FALSE,	// reserved
    eSwitch::_FALSE,	// reserved

    //50~51
    eOption::PLAY_LANE_EFFECT_TYPE_1P,
    eOption::PLAY_LANE_EFFECT_TYPE_2P,

    eSwitch::_FALSE,	// reserved
    eSwitch::_FALSE,	// reserved

    //54
    eSwitch::PLAY_OPTION_DP_FLIP,
    eOption::PLAY_HSFIX_TYPE_1P,
    eOption::PLAY_BATTLE_TYPE,
    eSwitch::_FALSE,	// HS-1P
    eSwitch::_FALSE,	// HS-2P

    // 59~69
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,
    eSwitch::_FALSE,

    // 70
    eSwitch::SYSTEM_SCOREGRAPH,
    eOption::PLAY_GHOST_TYPE_1P,
    eSwitch::_TRUE,	// bga off/on/autoplay only, special
    eSwitch::_TRUE, // bga normal/extend, special
    eSwitch::_FALSE,// JUDGE TIMING
    eSwitch::_FALSE,// AUTO ADJUST, not supported
    eSwitch::_FALSE, // default target rate
    eSwitch::_FALSE, // target

    eSwitch::_FALSE,
    eSwitch::_FALSE,

    // 80
eSwitch::_TRUE, // screen mode full/window, special
eSwitch::_FALSE, // color mode, 32bit fixed
eSwitch::_TRUE, // vsync, special
eSwitch::_FALSE,//save replay, not supported
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,

//90
eSwitch::_FALSE,//off/favorite/ignore, not supported
eSwitch::_FALSE,	// select all
eSwitch::_FALSE,	// select beginner
eSwitch::_FALSE,	// select normal
eSwitch::_FALSE,	// select hyper
eSwitch::_FALSE,	// select another
eSwitch::_FALSE,	// select insane

eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,

// 100
eSwitch::_FALSE,
eSwitch::K11_CONFIG,
eSwitch::K12_CONFIG,
eSwitch::K13_CONFIG,
eSwitch::K14_CONFIG,
eSwitch::K15_CONFIG,
eSwitch::K16_CONFIG,
eSwitch::K17_CONFIG,
eSwitch::K18_CONFIG,
eSwitch::K19_CONFIG,
eSwitch::S1L_CONFIG,
eSwitch::S1R_CONFIG,
eSwitch::K1START_CONFIG,
eSwitch::K1SELECT_CONFIG,
eSwitch::K1SPDUP_CONFIG,
eSwitch::K1SPDDN_CONFIG,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,

// 120
eSwitch::_FALSE,
eSwitch::K21_CONFIG,
eSwitch::K22_CONFIG,
eSwitch::K23_CONFIG,
eSwitch::K24_CONFIG,
eSwitch::K25_CONFIG,
eSwitch::K26_CONFIG,
eSwitch::K27_CONFIG,
eSwitch::K28_CONFIG,
eSwitch::K29_CONFIG,
eSwitch::S2L_CONFIG,
eSwitch::S2R_CONFIG,
eSwitch::K2START_CONFIG,
eSwitch::K2SELECT_CONFIG,
eSwitch::K2SPDUP_CONFIG,
eSwitch::K2SPDDN_CONFIG,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,

// 140
eOption::KEY_CONFIG_KEY7,
eOption::KEY_CONFIG_KEY9,
eOption::KEY_CONFIG_KEY5,
eOption::KEY_CONFIG_MODE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,
eSwitch::_FALSE,

// 150
eSwitch::KEY_CONFIG_SLOT0,
eSwitch::KEY_CONFIG_SLOT1,
eSwitch::KEY_CONFIG_SLOT2,
eSwitch::KEY_CONFIG_SLOT3,
eSwitch::KEY_CONFIG_SLOT4,
eSwitch::KEY_CONFIG_SLOT5,
eSwitch::KEY_CONFIG_SLOT6,
eSwitch::KEY_CONFIG_SLOT7,
eSwitch::KEY_CONFIG_SLOT8,
eSwitch::KEY_CONFIG_SLOT9,

};
bool buttonSw(int n, eSwitch& out)
{
    if (n < 0 || n >= buttonAdapter.size()) 
        return false;
    if (auto sw = std::get_if<eSwitch>(&buttonAdapter[n]))
    {
        out = *sw;
        return true;
    }
    else
        return false;
}
bool buttonOp(int n, eOption& out)
{
    if (n < 0 || n >= buttonAdapter.size())
        return false;
    if (auto op = std::get_if<eOption>(&buttonAdapter[n]))
    {
        out = *op;
        return true;
    }
    else
        return false;
}

}