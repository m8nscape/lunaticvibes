#pragma once
#include "buffered_global.h"

enum class eOption : unsigned
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

    CHART_CAN_SAVE_LAMP,

    PLAY_SCENE_STAT,
    PLAY_MODE,
    PLAY_KEYS,
    PLAY_BGA_TYPE,
    PLAY_GHOST_TYPE_1P,
    PLAY_GHOST_TYPE_2P,
    PLAY_GAUGE_TYPE_1P,
    PLAY_GAUGE_TYPE_2P,
    PLAY_RANDOM_TYPE_1P,
    PLAY_RANDOM_TYPE_2P,
    PLAY_LANE_EFFECT_TYPE_1P,	// OFF/HIDDEN/SUDDEN/HID+SUD
    PLAY_LANE_EFFECT_TYPE_2P,	// OFF/HIDDEN/SUDDEN/HID+SUD
    PLAY_HSFIX_TYPE_1P,	//OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT
    PLAY_HSFIX_TYPE_2P,	//OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT
    PLAY_BATTLE_TYPE,	// OFF/BATTLE(VS HUMAN)/BATTLE(VS GHOST)/DOUBLE BATTLE/ONLINE BATTLE
    PLAY_RANK_ESTIMATED_1P,
    PLAY_RANK_ESTIMATED_2P,
    PLAY_RANK_BORDER_1P,
    PLAY_RANK_BORDER_2P,
    PLAY_ACCURACY_1P,
    PLAY_ACCURACY_2P,
    PLAY_LAST_JUDGE_1P,
    PLAY_LAST_JUDGE_2P,
    PLAY_ENTRY_TYPE,
    PLAY_COURSE_STAGE,

    RESULT_RANK_1P,
    RESULT_RANK_2P,
    RESULT_MYBEST_RANK,
    RESULT_UPDATED_RANK,
    RESULT_CLEARED,

    SYS_WINDOWED,

    _TEST1 = 200,

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

	enum e_select_diff {
		DIFF_ANY,
		DIFF_BEGINNER,
		DIFF_NORMAL,
		DIFF_HYPER,
		DIFF_ANOTHER,
		DIFF_INSANE
	};

    enum e_select_sort {
        SORT_FOLDER,
        SORT_TITLE,
        SORT_LEVEL,
        SORT_CLEAR,
        SORT_RATE,
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
        KEYS_NOT_PLAYABLE,
        KEYS_ALL,
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

    enum e_speed_type {
        SPEED_NORMAL,
        //SPEED_FIX_END,
        SPEED_FIX_MIN,
        SPEED_FIX_MAX,
        SPEED_FIX_AVG,
        SPEED_FIX_CONSTANT,
    };

    enum e_gauge_type{
        GAUGE_ASSIST,
        GAUGE_EASY,
        GAUGE_NORMAL,
        GAUGE_HARD,
        GAUGE_EXHARD,
        GAUGE_DEATH,
    };

    enum e_random_type {
        RAN_NORMAL,
        RAN_MIRROR,
        RAN_RANDOM,
        RAN_SRAN,
        RAN_HRAN,
        RAN_ALLSCR,
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
        LAMP_NOT_APPLICIABLE,
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
        RANK_NONE,  // -
        RANK_0, // MAX
        RANK_1, // AAA
        RANK_2, // AA
        RANK_3, // A
        RANK_4, // B
        RANK_5, // C
        RANK_6, // D
        RANK_7, // E
        RANK_8, // F
    };

    enum e_acc_type {
        ACC_0,  // 0~10
        ACC_10, // 10~20
        ACC_20, // 
        ACC_30, //
        ACC_40, //
        ACC_50, //
        ACC_60, //
        ACC_70, //
        ACC_80, //
        ACC_90, //
		ACC_100
    };

    enum e_judge_diff {
        JUDGE_GAMBOLA,
        JUDGE_VHARD,
        JUDGE_HARD,
        JUDGE_NORMAL,
        JUDGE_EASY
    };

    enum e_judge_type {
        JUDGE_NONE,
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
        STAGE_NOT_COURSE,
        STAGE_FINAL, 
        STAGE_1, 
        STAGE_2, 
        STAGE_3, 
        STAGE_4, 
        STAGE_5, 
        STAGE_6, 
        STAGE_7, 
    };

    enum e_freq_type {
        FREQ_FREQ,
        FREQ_PITCH,
        FREQ_SPEED
    };

    enum e_fx_target {
        FX_MASTER,
        FX_KEY,
        FX_BGM
    };

    enum e_fx_type {
        FX_OFF,
        FX_REVERB,
        FX_DELAY,
        FX_LOWPASS,
        FX_HIGHPASS,
        FX_FLANGER,
        FX_CHORUS,
        FX_DISTORTION,
    };

    enum e_windowed {
        WIN_FULLSCREEN,
        WIN_BORDERLESS,
        WIN_WINDOWED
    };

}


inline buffered_global<eOption, unsigned, (size_t)eOption::OPTION_COUNT> gOptions;
