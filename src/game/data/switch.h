#pragma once
//#include <bitset>
#include "buffered_global.h"

enum class eSwitch: unsigned
{
    _TRUE = 0,           // should be initialized with true AFTER reset

    SELECT_PANEL1,
    SELECT_PANEL2,
    SELECT_PANEL3,
    SELECT_PANEL4,
    SELECT_PANEL5,
    SELECT_PANEL6,
    SELECT_PANEL7,
    SELECT_PANEL8,
    SELECT_PANEL9,

	SOUND_FX0,
	SOUND_FX1,
	SOUND_FX2,
	SOUND_VOLUME,
	SOUND_PITCH,

    SYSTEM_BGA,
    SYSTEM_AUTOPLAY,
    SYSTEM_SCOREGRAPH,  // for single player only

	NETWORK,
	PLAY_OPTION_EXTRA,
    PLAY_OPTION_AUTOSCR,
	PLAY_OPTION_DP_FLIP,

    PLAY_LOADING,
    PLAY_LOAD_FINISHED,

    RESULT_CLEAR,

    CHART_HAVE_BGA,
    CHART_HAVE_LN,
    CHART_HAVE_README,
    CHART_HAVE_SPEEDCHANGE,
    CHART_HAVE_BPMCHANGE,
    CHART_HAVE_RANDOM,
    CHART_HAVE_STAGEFILE,
    CHART_HAVE_BANNER,
    CHART_HAVE_BACKBMP,

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

    _FALSE = 999,        // should be initialized with false
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

