#pragma once
#include "buffered_global.h"

enum class eOption: unsigned
{
    DEFAULT = 0,           // should be initialized with 0

    // global
    RULE_TYPE,

	SOUND_FX0,				// OFF/REVERB/DELAY/LOWPASS/HIGHPASS/FLANGER/CHORUS/DISTORTION/PITCH
	SOUND_FX1,
	SOUND_FX2,
	SOUND_TARGET_FX0,		// MASTER/KEY/BGM
	SOUND_TARGET_FX1,
	SOUND_TARGET_FX2,
	SOUND_PITCH_TYPE,		// FREQENCY/PITCH/SPEED

    SELECT_ENTRY_TYPE,
    SELECT_DIFFICULTY,
    SELECT_ENTRY_LAMP,
    SELECT_ENTRY_RANK,
	SELECT_FILTER_DIFF,
	SELECT_FILTER_KEYS,
	SELECT_SORT,

    CHART_DIFFICULTY,
    CHART_PLAY_MODE,
    CHART_PLAY_KEYS,
    CHART_JUDGE_TYPE,

	PLAY_SCENE_STAT,
    PLAY_MODE,
    PLAY_KEYS,
    PLAY_BGA_TYPE,
    PLAY_GHOST_TYPE,
    PLAY_GAUGE_TYPE,
	PLAY_RANDOM_TYPE,
	PLAY_LANE_EFFECT_TYPE,	// OFF/HIDDEN/SUDDEN/HID+SUD
	PLAY_HSFIX_TYPE,	//OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT
	PLAY_BATTLE_TYPE,	// OFF/BATTLE(VS HUMAN)/BATTLE(VS GHOST)/DOUBLE BATTLE/ONLINE BATTLE
    PLAY_RANK_ESTIMATED,
    PLAY_RANK_BORDER,
    PLAY_LAST_JUDGE,
    PLAY_ENTRY_TYPE,
    PLAY_COURSE_STAGE,

    RESULT_RANK,
    RESULT_MYBEST_RANK,
    
    OPTION_COUNT
};

namespace Option
{
    const unsigned NO  = 0;
    const unsigned YES = 1;
    const unsigned OFF = 0;
    const unsigned ON  = 1;

    enum e_ruleset_type {
        CLASSIC = 0,
    };

    enum e_entry_type {
        ENTRY_FOLDER,
        ENTRY_SONG,
        ENTRY_COURSE,
        ENTRY_NEW_COURSE,
    };

	enum e_chart_diff {
		DIFF_ANY,
		DIFF_BEGINNER,
		DIFF_NORMAL,
		DIFF_HYPER,
		DIFF_ANOTHER,
		DIFF_INSANE
	};

	enum e_play_scene_stat {
		SPLAY_PREPARE,
		SPLAY_LOADING,
		SPLAY_READY,
		SPLAY_PLAYING,
		SPLAY_OUTRO,
		SPLAY_FADEOUT,
		SPLAY_FAILED,
	};

    enum e_play_mode {
        PLAY_SINGLE,
        PLAY_DOUBLE,
        PLAY_BATTLE
    };

    enum e_play_keys {
        KEYS_7,
        KEYS_5,
        KEYS_14,
        KEYS_10,
        KEYS_9,
        KEYS_24,
        KEYS_48,
    };

    enum e_play_ghost_mode {
        GHOST_OFF,
        GHOST_TOP, // A
        GHOST_SIDE, // B
        GHOST_SIDE_BOTTOM // C
    };

    enum e_bga_type {
        BGA_OFF,
        BGA_NORMAL,
        BGA_EXTEND,
    };

    enum e_gauge_type{
        GAUGE_ASSIST,
        GAUGE_EASY,
        GAUGE_NORMAL,
        GAUGE_HARD,
        GAUGE_EXHARD,
        GAUGE_DEATH,
    };

    enum e_chart_difficulty {
        DIFF_0, // -
        DIFF_1, // b
        DIFF_2, // n
        DIFF_3, // h
        DIFF_4, // a
        DIFF_5, // m
    };

    enum e_lamp_type {
        LAMP_NOPLAY,
        LAMP_FAILED,
        LAMP_ASSIST,
        LAMP_EASY,
        LAMP_NORMAL,
        LAMP_HARD,
        LAMP_EXHARD,
        LAMP_FULLCOMBO,
        LAMP_PERFECT,
        LAMP_MAX,
    };

    enum e_rank_type {
        RANK_0, // MAX
        RANK_1, // AAA
        RANK_2, // AA
        RANK_3, // A
        RANK_4, // B
        RANK_5, // C
        RANK_6, // D
        RANK_7, // E
        RANK_8, // F
        RANK_9  // -
    };

    enum e_judge_diff {
        JUDGE_GAMBOLA,
        JUDGE_VHARD,
        JUDGE_HARD,
        JUDGE_NORMAL,
        JUDGE_EASY
    };

    enum e_judge_type {
        JUDGE_0,
        JUDGE_1,
        JUDGE_2,
        JUDGE_3,
        JUDGE_4,
        JUDGE_5,
        JUDGE_6,
        //...
    };

    enum e_course_stage {
        STAGE_FINAL, 
        STAGE_1, 
        STAGE_2, 
        STAGE_3, 
        STAGE_4, 
        STAGE_5, 
        STAGE_6, 
        STAGE_7, 
    };

}


inline buffered_global<eOption, unsigned, (size_t)eOption::OPTION_COUNT> gOptions;
