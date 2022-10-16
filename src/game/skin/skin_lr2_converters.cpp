#include "skin_lr2_converters.h"

namespace lr2skin
{

IndexNumber num(int n)
{
    return IndexNumber(n);
}

IndexTimer timer(int n)
{
    return IndexTimer(n);
}

IndexText text(int n)
{
    return IndexText(n);
}

static const std::vector<std::variant<std::monostate, IndexSwitch, IndexOption>> buttonAdapter{
    // 0
    std::monostate(),

    // 1~9
    IndexSwitch::SELECT_PANEL1,
    IndexSwitch::SELECT_PANEL2,
    IndexSwitch::SELECT_PANEL3,
    IndexSwitch::SELECT_PANEL4,
    IndexSwitch::SELECT_PANEL5,
    IndexSwitch::SELECT_PANEL6,
    IndexSwitch::SELECT_PANEL7,
    IndexSwitch::SELECT_PANEL8,
    IndexSwitch::SELECT_PANEL9,

    // 10~12
    IndexOption::SELECT_FILTER_DIFF,
    IndexOption::SELECT_FILTER_KEYS,
    IndexOption::SELECT_SORT,

    // 13~19
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    // 20~28
    IndexOption::SOUND_FX0,
    IndexOption::SOUND_FX1,
    IndexOption::SOUND_FX2,
    IndexSwitch::SOUND_FX0,
    IndexSwitch::SOUND_FX1,
    IndexSwitch::SOUND_FX2,
    IndexOption::SOUND_TARGET_FX0,
    IndexOption::SOUND_TARGET_FX1,
    IndexOption::SOUND_TARGET_FX2,

    //29~30
    IndexSwitch::SOUND_EQ,	// EQ off/on
    IndexSwitch::_FALSE,	// EQ Preset

    //31~33
    IndexSwitch::SOUND_VOLUME,		// volume control
    IndexSwitch::SOUND_PITCH,
    IndexOption::SOUND_PITCH_TYPE,

    //34~39
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    //40~45
    IndexOption::PLAY_GAUGE_TYPE_1P,
    IndexOption::PLAY_GAUGE_TYPE_2P,
    IndexOption::PLAY_RANDOM_TYPE_1P,
    IndexOption::PLAY_RANDOM_TYPE_2P,
    IndexSwitch::PLAY_OPTION_AUTOSCR_1P,
    IndexSwitch::PLAY_OPTION_AUTOSCR_2P,

    //46~49
    IndexSwitch::_TRUE,	    // lanecover
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,	// reserved
    IndexSwitch::_FALSE,	// reserved

    //50~51
    IndexOption::PLAY_LANE_EFFECT_TYPE_1P,
    IndexOption::PLAY_LANE_EFFECT_TYPE_2P,

    IndexSwitch::_FALSE,	// reserved
    IndexSwitch::_FALSE,	// reserved

    //54
    IndexSwitch::PLAY_OPTION_DP_FLIP,
    IndexOption::PLAY_HSFIX_TYPE,
    IndexOption::PLAY_BATTLE_TYPE,
    IndexSwitch::_FALSE,	// HS-1P
    IndexSwitch::_FALSE,	// HS-2P

    // 59~69
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    // 70
    IndexSwitch::SYSTEM_SCOREGRAPH,
    IndexOption::PLAY_GHOST_TYPE_1P,
    IndexSwitch::_TRUE,	// bga off/on/autoplay only, special
    IndexSwitch::_TRUE, // bga normal/extend, special
    IndexSwitch::_FALSE,// JUDGE TIMING
    IndexSwitch::_FALSE,// AUTO ADJUST, not supported
    IndexSwitch::_FALSE, // default target rate
    IndexOption::PLAY_TARGET_TYPE, // target

    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    // 80
IndexSwitch::_TRUE, // screen mode full/window, special
IndexSwitch::_FALSE, // color mode, 32bit fixed
IndexSwitch::_TRUE, // vsync, special
IndexSwitch::_FALSE,//save replay, not supported
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

//90
IndexSwitch::_FALSE,//off/favorite/ignore, not supported
IndexSwitch::_FALSE,	// select all
IndexSwitch::_FALSE,	// select beginner
IndexSwitch::_FALSE,	// select normal
IndexSwitch::_FALSE,	// select hyper
IndexSwitch::_FALSE,	// select another
IndexSwitch::_FALSE,	// select insane

IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 100
IndexSwitch::_FALSE,
IndexSwitch::K11_CONFIG,
IndexSwitch::K12_CONFIG,
IndexSwitch::K13_CONFIG,
IndexSwitch::K14_CONFIG,
IndexSwitch::K15_CONFIG,
IndexSwitch::K16_CONFIG,
IndexSwitch::K17_CONFIG,
IndexSwitch::K18_CONFIG,
IndexSwitch::K19_CONFIG,
IndexSwitch::S1L_CONFIG,
IndexSwitch::S1R_CONFIG,
IndexSwitch::K1START_CONFIG,
IndexSwitch::K1SELECT_CONFIG,
IndexSwitch::K1SPDUP_CONFIG,    // new 114
IndexSwitch::K1SPDDN_CONFIG,    // new 115
IndexSwitch::S1A_CONFIG,        // new 116
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 120
IndexSwitch::_FALSE,
IndexSwitch::K21_CONFIG,
IndexSwitch::K22_CONFIG,
IndexSwitch::K23_CONFIG,
IndexSwitch::K24_CONFIG,
IndexSwitch::K25_CONFIG,
IndexSwitch::K26_CONFIG,
IndexSwitch::K27_CONFIG,
IndexSwitch::K28_CONFIG,
IndexSwitch::K29_CONFIG,
IndexSwitch::S2L_CONFIG,
IndexSwitch::S2R_CONFIG,
IndexSwitch::K2START_CONFIG,
IndexSwitch::K2SELECT_CONFIG,
IndexSwitch::K2SPDUP_CONFIG,    // new 134
IndexSwitch::K2SPDDN_CONFIG,    // new 135
IndexSwitch::S2A_CONFIG,        // new 136
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 140
IndexOption::KEY_CONFIG_KEY7,
IndexOption::KEY_CONFIG_KEY9,
IndexOption::KEY_CONFIG_KEY5,
IndexOption::KEY_CONFIG_MODE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 150
IndexSwitch::KEY_CONFIG_SLOT0,
IndexSwitch::KEY_CONFIG_SLOT1,
IndexSwitch::KEY_CONFIG_SLOT2,
IndexSwitch::KEY_CONFIG_SLOT3,
IndexSwitch::KEY_CONFIG_SLOT4,
IndexSwitch::KEY_CONFIG_SLOT5,
IndexSwitch::KEY_CONFIG_SLOT6,
IndexSwitch::KEY_CONFIG_SLOT7,
IndexSwitch::KEY_CONFIG_SLOT8,
IndexSwitch::KEY_CONFIG_SLOT9,

// 160 (undefined)
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 170
IndexSwitch::SKINSELECT_7KEYS,
IndexSwitch::SKINSELECT_5KEYS,
IndexSwitch::SKINSELECT_14KEYS,
IndexSwitch::SKINSELECT_10KEYS,
IndexSwitch::SKINSELECT_9KEYS,
IndexSwitch::SKINSELECT_SELECT,
IndexSwitch::SKINSELECT_DECIDE,
IndexSwitch::SKINSELECT_RESULT,
IndexSwitch::SKINSELECT_KEYCONFIG,
IndexSwitch::SKINSELECT_SKINSELECT,
IndexSwitch::SKINSELECT_SOUNDSET,
IndexSwitch::SKINSELECT_THEME,
IndexSwitch::SKINSELECT_7KEYS_BATTLE,
IndexSwitch::SKINSELECT_5KEYS_BATTLE,
IndexSwitch::SKINSELECT_9KEYS_BATTLE,
IndexSwitch::SKINSELECT_COURSE_RESULT,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 190
};

bool buttonSw(int n, IndexSwitch& out)
{
    if (n < 0 || n >= buttonAdapter.size()) 
        return false;
    if (auto sw = std::get_if<IndexSwitch>(&buttonAdapter[n]))
    {
        out = *sw;
        return true;
    }
    else
        return false;
}
bool buttonOp(int n, IndexOption& out)
{
    if (n < 0 || n >= buttonAdapter.size())
        return false;
    if (auto op = std::get_if<IndexOption>(&buttonAdapter[n]))
    {
        out = *op;
        return true;
    }
    else
        return false;
}

}