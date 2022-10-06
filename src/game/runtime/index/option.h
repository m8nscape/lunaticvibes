#pragma once

/*
* These indices are dymanically converted to LR2skin indices.
* You may modify the sequnce order freely.
*/
enum class IndexOption : unsigned
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

    KEY_CONFIG_MODE,        // 7/9/5
    KEY_CONFIG_KEY7,        // none/1P1/1P2/1P3/1P4/1P5/1P6/1P7/1PLEFT/1PRIGHT/1PSTART/1PSELECT/2P1/2P2/2P3/2P4/2P5/2P6/2P7/2PLEFT/2PRIGHT/2PSTART/2PSELECT/1PSCRATCH(new)/2PSCRATCH(new)
    KEY_CONFIG_KEY9,        // none/1/2/3/4/5/6/7/8/9/START/SELECT
    KEY_CONFIG_KEY5,        // none/1P1/1P2/1P3/1P4/1P5/1PLEFT/1PRIGHT/1PSTART/1PSELECT/2P1/2P2/2P3/2P4/2P5/2PLEFT/2PRIGHT/2PSTART/2PSELECT/1PSCRATCH(new)/2PSCRATCH(new)

    CHART_DIFFICULTY,
    CHART_PLAY_KEYS,
    CHART_JUDGE_TYPE,

    CHART_SAVE_LAMP_TYPE,    // 

    PLAY_SCENE_STAT,
    PLAY_MODE,              // SP/DP/SP Battle/DB/SP-GB/DP-GB
    PLAY_KEYS,
    PLAY_BGA_TYPE,          // OFF/ON/AUTOPLAY
    PLAY_BGA_SIZE,          // NORMAL/EXTEND
    PLAY_GHOST_TYPE_1P,     // OFF/A/B/C
    PLAY_GHOST_TYPE_2P,
    PLAY_GAUGE_TYPE_1P,     // GROOVE/HARD/SUDDEN/EASY/EXHARD(new)/ASSIST(new)
    PLAY_GAUGE_TYPE_2P,     // GROOVE/HARD/SUDDEN/EASY/EXHARD(new)/ASSIST(new)
    PLAY_RANDOM_TYPE_1P,
    PLAY_RANDOM_TYPE_2P,
    PLAY_LANE_EFFECT_TYPE_1P,	// OFF/HID+/SUD+/SUD+&HID+/LIFT(new)/LIFT&SUD+(new)
    PLAY_LANE_EFFECT_TYPE_2P,	// OFF/HID+/SUD+/SUD+&HID+/LIFT(new)/LIFT&SUD+(new)
    PLAY_HSFIX_TYPE,	//OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT
    PLAY_BATTLE_TYPE,	// OFF/BATTLE/DB/SPtoDP,9to7/GhostBattle
    PLAY_TARGET_TYPE,   // 0%/MYBEST/AAA/AA/A/DEFAULT/IRTOP/IRNEXT/IRAVERAGE

    PLAY_RANK_ESTIMATED_1P,
    PLAY_RANK_ESTIMATED_2P,
    PLAY_RANK_BORDER_1P,
    PLAY_RANK_BORDER_2P,
    PLAY_HEALTH_1P,
    PLAY_HEALTH_2P,
    PLAY_LAST_JUDGE_1P,
    PLAY_LAST_JUDGE_2P,
    PLAY_LAST_JUDGE_FASTSLOW_1P,   // 0:exact 1:fast 2:slow
    PLAY_LAST_JUDGE_FASTSLOW_2P,   // 0:exact 1:fast 2:slow
    PLAY_ENTRY_TYPE,
    PLAY_COURSE_STAGE,

    RESULT_RANK_1P,
    RESULT_RANK_2P,
    RESULT_MYBEST_RANK,
    RESULT_UPDATED_RANK,
    RESULT_CLEARED,
    RESULT_BATTLE_WIN_LOSE,  // 0:DRAW 1:1PWIN 2:2PWIN

    SYS_WINDOWED,
    SYS_VSYNC,

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
    constexpr char* s_select_diff[] = {
        "ALL",
        "BEGINNER",
        "NORMAL",
        "HYPER",
        "ANOTHER",
        "INSANE"
    };

    enum e_select_sort {
        SORT_FOLDER,
        SORT_LEVEL,
        SORT_TITLE,
        SORT_CLEAR,
        SORT_RATE,
    };
    constexpr char* s_select_sort[] = {
        "FOLDER",
        "LEVEL",
        "TITLE",
        "CLEAR",
        "INSANE"
    };

    enum e_key_config_mode {
        KEYCFG_7,
        KEYCFG_9,
        KEYCFG_5
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
        PLAY_MODE_SINGLE,
        PLAY_MODE_DOUBLE,
        PLAY_MODE_BATTLE,
        PLAY_MODE_DOUBLE_BATTLE,
        PLAY_MODE_SP_GHOST_BATTLE,
        PLAY_MODE_DP_GHOST_BATTLE,
    };

    enum e_filter_keys {
        FILTER_KEYS_ALL,
        FILTER_KEYS_SINGLE,
        FILTER_KEYS_7,
        FILTER_KEYS_5,
        FILTER_KEYS_DOUBLE,
        FILTER_KEYS_14,
        FILTER_KEYS_10,
        FILTER_KEYS_9,
    };
    constexpr char *s_filter_keys[] = {
        "ALL KEYS",
        "SINGLE",
        "7KEYS",
        "5KEYS",
        "DOUBLE",
        "14KEYS",
        "10KEYS",
        "9BUTTONS"
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
    constexpr char* s_play_ghost_mode[] = {
        "OFF",
        "TYPE A",
        "TYPE B",
        "TYPE C",
    };

    enum e_bga_type {
        BGA_OFF,
        BGA_ON,
        BGA_AUTOPLAY,
    };
    constexpr char* s_bga_type[] = {
        "OFF",
        "ON",
        "AUTOPLAY"
    };

    enum e_bga_size {
        BGA_NORMAL,
        BGA_EXTEND,
    };
    constexpr char* s_bga_size[] = {
        "NORMAL",
        "EXTEND",
    };

    enum e_lane_effect_type {
        LANE_OFF,
        LANE_HIDDEN,
        LANE_SUDDEN,
        LANE_SUDHID,
        LANE_LIFT,
        LANE_LIFTSUD,
    };
    constexpr char* s_lane_effect_type[] = {
        "OFF",
        "HIDDEN+",
        "SUDDEN+",
        "SUD+&HID+",
        "LIFT",
        "LIFT&SUD+",
    };

    enum e_speed_type {
        SPEED_NORMAL,
        //SPEED_FIX_END,
        SPEED_FIX_MIN,
        SPEED_FIX_MAX,
        SPEED_FIX_AVG,
        SPEED_FIX_CONSTANT,
    };
    constexpr char* s_speed_type[] = {
        "OFF",
        //"END",
        "MIN FIX",
        "MAX FIX",
        "AVERAGE",
        "CONSTANT",
    };

    enum e_battle_type {
        BATTLE_OFF,
        BATTLE_LOCAL,
        BATTLE_DB,
        BATTLE_SPtoDP_9to7, // not implemented
        BATTLE_GHOST,
    };
    constexpr char* s_battle_type[] = {
        "OFF",
        "BATTLE",
        "D-BATTLE",
        "SP to DP",
        "GHOST",
    };

    enum e_target_type {
        TARGET_0,
        TARGET_MYBEST,
        TARGET_AAA,
        TARGET_AA,
        TARGET_A,
        TARGET_DEFAULT,
        TARGET_IR_TOP,
        TARGET_IR_NEXT,
        TARGET_IR_AVERAGE,
    };
    constexpr char* s_target_type[] = {
        "NO TARGET",
        "MY BEST",
        "RANK AAA",
        "RANK AA",
        "RANK A",
        "50%",
        "IR TOP",
        "IR NEXT",
        "IR AVERAGE",
    };

    enum e_gauge_type{
        GAUGE_NORMAL,
        GAUGE_HARD,
        GAUGE_DEATH,
        GAUGE_EASY,
        GAUGE_ASSIST,
        GAUGE_EXHARD,
    };
    constexpr char* s_gauge_type[] = {
        "NORMAL",
        "HARD",
        "DEATH",
        "EASY",
        "ASSIST",
        "EXHARD",
    };

    enum e_random_type {
        RAN_NORMAL,
        RAN_MIRROR,
        RAN_RANDOM,
        RAN_SRAN,
        RAN_HRAN,
        RAN_ALLSCR,
    };
    constexpr char* s_random_type[] = {
        "NORMAL",
        "MIRROR",
        "RANDOM",
        "S-RANDOM",
        "H-RANDOM",
        "ALL-SCR",
    };

    enum e_assist_type {
        ASSIST_NONE,
        ASSIST_AUTOSCR,
    };
    constexpr char* s_assist_type[] = {
        "NONE",
        "AUTO-SCR",
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
    constexpr e_rank_type getRankType(double rate)
    {
        if (rate >= 100.0)              return e_rank_type::RANK_0;
        else if (rate >= 100.0 * 8 / 9) return e_rank_type::RANK_1;
        else if (rate >= 100.0 * 7 / 9) return e_rank_type::RANK_2;
        else if (rate >= 100.0 * 6 / 9) return e_rank_type::RANK_3;
        else if (rate >= 100.0 * 5 / 9) return e_rank_type::RANK_4;
        else if (rate >= 100.0 * 4 / 9) return e_rank_type::RANK_5;
        else if (rate >= 100.0 * 3 / 9) return e_rank_type::RANK_6;
        else if (rate >= 100.0 * 2 / 9) return e_rank_type::RANK_7;
        else if (rate > 0.0)            return e_rank_type::RANK_8;
        else                            return e_rank_type::RANK_NONE;
    }

    enum e_health_range {
        HEALTH_0,  // 0~10
        HEALTH_10, // 10~20
        HEALTH_20, // 
        HEALTH_30, //
        HEALTH_40, //
        HEALTH_50, //
        HEALTH_60, //
        HEALTH_70, //
        HEALTH_80, //
        HEALTH_90, //
		HEALTH_100
    };
    constexpr e_health_range getHealthType(double health)
    {
        if (health >= 100.0)     return e_health_range::HEALTH_100;
        else if (health >= 90.0) return e_health_range::HEALTH_90;
        else if (health >= 80.0) return e_health_range::HEALTH_80;
        else if (health >= 70.0) return e_health_range::HEALTH_70;
        else if (health >= 60.0) return e_health_range::HEALTH_60;
        else if (health >= 50.0) return e_health_range::HEALTH_50;
        else if (health >= 40.0) return e_health_range::HEALTH_40;
        else if (health >= 30.0) return e_health_range::HEALTH_30;
        else if (health >= 20.0) return e_health_range::HEALTH_20;
        else if (health >= 10.0) return e_health_range::HEALTH_10;
        else                     return e_health_range::HEALTH_0;
    }

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
    constexpr char* s_windowed[] = {
        "FULLSCREEN",
        "BORDERLESS",
        "WINDOWED",
    };

    enum e_vsync_mode {
        VSYNC_OFF,
        VSYNC_ON,
        VSYNC_ADAPTIVE
    };
    constexpr char* s_vsync_mode[] = {
        "OFF",
        "ON",
        "ADAPTIVE",
    };

}


