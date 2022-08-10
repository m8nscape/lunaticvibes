#pragma once
//#include <bitset>
#include "buffered_global.h"

enum class eSwitch: unsigned
{
    _TRUE = 0,           // should be initialized with true

    SELECT_PANEL1,
    SELECT_PANEL2,
    SELECT_PANEL3,
    SELECT_PANEL4,
    SELECT_PANEL5,
    SELECT_PANEL6,
    SELECT_PANEL7,
    SELECT_PANEL8,
    SELECT_PANEL9,

    SOUND_EQ,
	SOUND_FX0,
	SOUND_FX1,
	SOUND_FX2,
	SOUND_VOLUME,
	SOUND_PITCH,

    SYSTEM_BGA,
    SYSTEM_AUTOPLAY,
    SYSTEM_SCOREGRAPH,

	NETWORK,
	PLAY_OPTION_EXTRA,

    PLAY_OPTION_AUTOSCR_1P,
    PLAY_OPTION_AUTOSCR_2P,
	PLAY_OPTION_DP_FLIP,

    PLAY_LOADING,
    PLAY_LOAD_FINISHED,

    RESULT_CLEAR,
	RESULT_UPDATED_SCORE,
	RESULT_UPDATED_MAXCOMBO,
	RESULT_UPDATED_BP,
	RESULT_UPDATED_TRIAL,
	RESULT_UPDATED_IRRANK,
	RESULT_UPDATED_RANK,

    CHART_HAVE_BGA,
    CHART_HAVE_LN,
    CHART_HAVE_README,
    CHART_HAVE_SPEEDCHANGE,
    CHART_HAVE_BPMCHANGE,
    CHART_HAVE_RANDOM,
    CHART_HAVE_STAGEFILE,
    CHART_HAVE_BANNER,
    CHART_HAVE_BACKBMP,

	CHART_CAN_SAVE_SCORE,

    S1_DOWN,
    S1L_DOWN,
    S1R_DOWN,
    K11_DOWN,
    K12_DOWN,
    K13_DOWN,
    K14_DOWN,
    K15_DOWN,
    K16_DOWN,
    K17_DOWN,
    K18_DOWN,
    K19_DOWN,
    K1START_DOWN,
    K1SELECT_DOWN,
    K1SPDUP_DOWN,
    K1SPDDN_DOWN,

    S2_DOWN,
    S2L_DOWN,
    S2R_DOWN,
    K21_DOWN,
    K22_DOWN,
    K23_DOWN,
    K24_DOWN,
    K25_DOWN,
    K26_DOWN,
    K27_DOWN,
    K28_DOWN,
    K29_DOWN,
    K2START_DOWN,
    K2SELECT_DOWN,
    K2SPDUP_DOWN,
    K2SPDDN_DOWN,


    K11_CONFIG,
    K12_CONFIG,
    K13_CONFIG,
    K14_CONFIG,
    K15_CONFIG,
    K16_CONFIG,
    K17_CONFIG,
    K18_CONFIG,
    K19_CONFIG,
    S1L_CONFIG,
    S1R_CONFIG,
    K1START_CONFIG,
    K1SELECT_CONFIG,
    K1SPDUP_CONFIG,
    K1SPDDN_CONFIG,
    S1A_CONFIG,

    K21_CONFIG,
    K22_CONFIG,
    K23_CONFIG,
    K24_CONFIG,
    K25_CONFIG,
    K26_CONFIG,
    K27_CONFIG,
    K28_CONFIG,
    K29_CONFIG,
    S2L_CONFIG,
    S2R_CONFIG,
    K2START_CONFIG,
    K2SELECT_CONFIG,
    K2SPDUP_CONFIG,
    K2SPDDN_CONFIG,
    S2A_CONFIG,

        KEY_CONFIG_SLOT0,
        KEY_CONFIG_SLOT1,
        KEY_CONFIG_SLOT2,
        KEY_CONFIG_SLOT3,
        KEY_CONFIG_SLOT4,
        KEY_CONFIG_SLOT5,
        KEY_CONFIG_SLOT6,
        KEY_CONFIG_SLOT7,
        KEY_CONFIG_SLOT8,
        KEY_CONFIG_SLOT9,

	P1_SETTING_SPEED,
	P2_SETTING_SPEED,

    _FALSE = 999,        // should be initialized with false

    _TEST1,
    SWITCH_COUNT
};

inline buffered_global<eSwitch, bool, (size_t)eSwitch::SWITCH_COUNT> gSwitches;

/*
class gSwitches
{
protected:
    constexpr gSwitches() : _data{ false }, _dataBuffer{ false } { _data[(size_t)eSwitch::FALSE] = true; }
private:
    static gSwitches _inst;
    std::bitset<(size_t)eSwitch::SWITCH_COUNT> _data;
    std::bitset<(size_t)eSwitch::SWITCH_COUNT> _dataBuffer;
public:
    static constexpr bool get(eSwitch n) { return _inst._data[(size_t)n] - _inst._data[0]; }
    static constexpr void queue(eSwitch n, bool value) { _inst._dataBuffer[(size_t)n] = value; }
    static constexpr void flush() { _inst._data = _inst._dataBuffer; }
protected:
    static constexpr void _set(eSwitch n, bool value) { _inst._data[(size_t)n] = _inst._dataBuffer[(size_t)n] = value; }
};
*/

