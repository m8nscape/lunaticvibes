#pragma once
#include "buffered_global.h"
#include <algorithm>

class dpercent
{
protected:
	double _data;
public:
	dpercent() : dpercent(0) {}
	dpercent(double d) { _data = std::clamp(d, 0.0, 1.0); }
	operator double() const { return _data; }
};

enum class eBargraph : unsigned
{
    ZERO = 0,

	MUSIC_PROGRESS = 1,
	MUSIC_LOAD_PROGRESS,

	LEVEL_BAR = 3,

	LEVEL_BAR_BEGINNER = 5,
	LEVEL_BAR_NORMAL,
	LEVEL_BAR_HYPER,
	LEVEL_BAR_ANOTHER,
	LEVEL_BAR_INSANE,
	
	PLAY_EXSCORE = 10,
	PLAY_EXSCORE_PREDICT,
	PLAY_MYBEST_NOW,
	PLAY_MYBEST,
	PLAY_RIVAL_EXSCORE,
	PLAY_RIVAL_EXSCORE_FINAL,

	RESULT_PG = 20,
	RESULT_GR,
	RESULT_GD,
	RESULT_BD,
	RESULT_PR,
	RESULT_MAXCOMBO,
	RESULT_SCORE,
	RESULT_EXSCORE,

	RESULT_RIVAL_PG = 30,
	RESULT_RIVAL_GR,
	RESULT_RIVAL_GD,
	RESULT_RIVAL_BD,
	RESULT_RIVAL_PR,
	RESULT_RIVAL_MAXCOMBO,
	RESULT_RIVAL_SCORE,
	RESULT_RIVAL_EXSCORE,

	SELECT_MYBEST_PG = 40,
	SELECT_MYBEST_GR,
	SELECT_MYBEST_GD,
	SELECT_MYBEST_BD,
	SELECT_MYBEST_PR,
	SELECT_MYBEST_MAXCOMBO,
	SELECT_MYBEST_SCORE,
	SELECT_MYBEST_EXSCORE,

	// LR2HelperG
	PLAY_1P_SLOW_COUNT = 48,
	PLAY_1P_FAST_COUNT,
	MINBPM_LANECOVER = 50,	// ?
	MAXBPM_LANECOVER,

	// new
	PLAY_2P_SLOW_COUNT = 58,
	PLAY_2P_FAST_COUNT,

	// key config extended
	FORCE_S1L = 60,
	FORCE_S1R,
	FORCE_K1Start,
	FORCE_K1Select,

	FORCE_S2L = 65,
	FORCE_S2R,
	FORCE_K2Start,
	FORCE_K2Select,

	FORCE_S1A = 70,
	FORCE_K11,
	FORCE_K12,
	FORCE_K13,
	FORCE_K14,
	FORCE_K15,
	FORCE_K16,
	FORCE_K17,
	FORCE_K18,
	FORCE_K19,

	FORCE_S2A = 80,
	FORCE_K21,
	FORCE_K22,
	FORCE_K23,
	FORCE_K24,
	FORCE_K25,
	FORCE_K26,
	FORCE_K27,
	FORCE_K28,
	FORCE_K29,

	// internal 
    MUSIC_LOAD_PROGRESS_SYS = 90,
    MUSIC_LOAD_PROGRESS_WAV,
    MUSIC_LOAD_PROGRESS_BGA,

    _TEST1,

    BARGRAPH_COUNT
};

inline buffered_global<eBargraph, dpercent, (size_t)eBargraph::BARGRAPH_COUNT> gBargraphs;

/*
class gBargraphs
{
protected:
    constexpr gBargraphs() : _data{ 0u } {}
private:
    std::array<percent, (size_t)eBargraph::SLIDER_COUNT> _data;
public:
    constexpr int get(eBargraph n) { return _data[(size_t)n]; }
    constexpr void set(eBargraph n, percent value) { _data[(size_t)n] = value < 100u ? value : 100u; }
};
*/