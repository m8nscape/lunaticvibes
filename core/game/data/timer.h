#pragma once
#include <chrono>
#include "buffered_global.h"
#include "beat.h"

enum class eTimer : unsigned
{
    SCENE_START = 0,
    
    START_INPUT,
    FADEOUT_BEGIN,
    FAIL_BEGIN,
    TEXT_INPUT_FINISHED,

    PLAY_READY = 40,
    PLAY_START,

    PLAY_JUDGE_1P = 46,
    PLAY_JUDGE_2P,
    PLAY_FULLCOMBO_1P, 
    PLAY_FULLCOMBO_2P,
	
	S1_BOMB = 50,
    K11_BOMB,
    K12_BOMB,
    K13_BOMB,
    K14_BOMB,
    K15_BOMB,
    K16_BOMB,
    K17_BOMB,
    K18_BOMB,
    K19_BOMB,

    S2_BOMB,
    K21_BOMB,
    K22_BOMB,
    K23_BOMB,
    K24_BOMB,
    K25_BOMB,
    K26_BOMB,
    K27_BOMB,
    K28_BOMB,
    K29_BOMB,


    S1L_DOWN = 100,
    K11_DOWN,
    K12_DOWN,
    K13_DOWN,
    K14_DOWN,
    K15_DOWN,
    K16_DOWN,
    K17_DOWN,
    K18_DOWN,
    K19_DOWN,

    S2L_DOWN,
    K21_DOWN,
    K22_DOWN,
    K23_DOWN,
    K24_DOWN,
    K25_DOWN,
    K26_DOWN,
    K27_DOWN,
    K28_DOWN,
    K29_DOWN,

    S1L_UP,
    K11_UP,
    K12_UP,
    K13_UP,
    K14_UP,
    K15_UP,
    K16_UP,
    K17_UP,
    K18_UP,
    K19_UP,

    S2L_UP,
    K21_UP,
    K22_UP,
    K23_UP,
    K24_UP,
    K25_UP,
    K26_UP,
    K27_UP,
    K28_UP,
    K29_UP,

    MUSIC_BEAT = 140,     // Special timer: [0, 1000), referring beat progress

    S1R_DOWN = 200,
    S1R_UP,
    K1START_DOWN,
    K1SELECT_DOWN,
    K1SPDUP_DOWN,
    K1SPDDN_DOWN,
    K1START_UP,
    K1SELECT_UP,
    K1SPDUP_UP,
    K1SPDDN_UP,

    S2R_DOWN,
    S2R_UP,
    K2START_DOWN,
    K2SELECT_DOWN,
    K2SPDUP_DOWN,
    K2SPDDN_DOWN,
    K2START_UP,
    K2SELECT_UP,
    K2SPDUP_UP,
    K2SPDDN_UP,

	// Inner timers
	_CURRENT,
	_LOAD_START,
		_JUDGE_1P_0,
		_JUDGE_1P_1,
		_JUDGE_1P_2,
		_JUDGE_1P_3,
		_JUDGE_1P_4,
		_JUDGE_1P_5,
		_JUDGE_2P_0,
		_JUDGE_2P_1,
		_JUDGE_2P_2,
		_JUDGE_2P_3,
		_JUDGE_2P_4,
		_JUDGE_2P_5,

	_NEVER,
    TIMER_COUNT
};

inline buffered_global<eTimer, long long, (size_t)eTimer::TIMER_COUNT> gTimers;
