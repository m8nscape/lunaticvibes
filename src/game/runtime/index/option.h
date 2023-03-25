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
    PLAY_GAUGE_TYPE_1P,     // GROOVE/HARD/SUDDEN/EASY/PATK(disabled)/GATK(disabled)/EXHARD(new)/ASSIST(new)
    PLAY_GAUGE_TYPE_2P,     // GROOVE/HARD/SUDDEN/EASY/PATK(disabled)/GATK(disabled)/EXHARD(new)/ASSIST(new)
    PLAY_RANDOM_TYPE_1P,    // OFF/MIRROR/RANDOM/S-RAN/H-RAN(SCATTER)/ALLSCR(CONVERGE)/R-RAN(new)/DB_SYNCHRONIZE(new)/DB_SYMMETRY(new)
    PLAY_RANDOM_TYPE_2P,    // OFF/MIRROR/RANDOM/S-RAN/H-RAN(SCATTER)/ALLSCR(CONVERGE)/R-RAN(new)/DB_SYNCHRONIZE(new)/DB_SYMMETRY(new)
    PLAY_LANE_EFFECT_TYPE_1P,	// OFF/HID+/SUD+/SUD+&HID+/LIFT(new)/LIFT&SUD+(new)
    PLAY_LANE_EFFECT_TYPE_2P,	// OFF/HID+/SUD+/SUD+&HID+/LIFT(new)/LIFT&SUD+(new)
    PLAY_HSFIX_TYPE,	//OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT/START(new)/MAIN(new)
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
    RESULT_BATTLE_WIN_LOSE,  // 0:DRAW 1:1PWIN 2:2PWIN
    RESULT_CLEAR_TYPE_1P,
    RESULT_CLEAR_TYPE_2P,
    RESULT_ARENA_PLAYER_RANKING,

    COURSE_TYPE,
    COURSE_STAGE_COUNT,
    COURSE_STAGE1_DIFFICULTY,
    COURSE_STAGE2_DIFFICULTY,
    COURSE_STAGE3_DIFFICULTY,
    COURSE_STAGE4_DIFFICULTY,
    COURSE_STAGE5_DIFFICULTY,

    SYS_WINDOWED,
    SYS_VSYNC,


    // network match
    ARENA_PLAYDATA_BASE = 100,

    ARENA_PLAYDATA_RANK_ESTIMATED = ARENA_PLAYDATA_BASE,
    ARENA_PLAYDATA_RANK,
    ARENA_PLAYDATA_HEALTH,
    ARENA_PLAYDATA_CLEAR_TYPE,
    ARENA_PLAYDATA_RANKING,

    ARENA_PLAYDATA_MAX = ARENA_PLAYDATA_BASE + 10 - 1,

    ARENA_PLAYDATA_ALL_MAX = ARENA_PLAYDATA_BASE + (ARENA_PLAYDATA_MAX - ARENA_PLAYDATA_BASE + 1) * 8 - 1,

    _TEST1 = 150,

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
    static constexpr const char* s_select_diff[] = {
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
    static constexpr const char* s_select_sort[] = {
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
    static constexpr const char *s_filter_keys[] = {
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
    static constexpr const char* s_play_ghost_mode[] = {
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
    static constexpr const char* s_bga_type[] = {
        "OFF",
        "ON",
        "AUTOPLAY"
    };

    enum e_bga_size {
        BGA_NORMAL,
        BGA_EXTEND,
    };
    static constexpr const char* s_bga_size[] = {
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
    static constexpr const char* s_lane_effect_type[] = {
        "OFF",
        "HIDDEN+",
        "SUDDEN+",
        "SUD+&HID+",
        "LIFT",
        "LIFT&SUD+",
    };

    enum e_speed_type {
        SPEED_NORMAL,
        SPEED_FIX_MIN,
        SPEED_FIX_MAX,
        SPEED_FIX_AVG,
        SPEED_FIX_CONSTANT,
        SPEED_FIX_INITIAL,
        SPEED_FIX_MAIN,
    };
    static constexpr const char* s_speed_type[] = {
        "OFF",
        "MIN FIX",
        "MAX FIX",
        "AVERAGE",
        "CONSTANT",
        "START FIX",
        "MAIN FIX"
    };

    enum e_battle_type {
        BATTLE_OFF,
        BATTLE_LOCAL,
        BATTLE_DB,
        BATTLE_SPtoDP_9to7, // not implemented
        BATTLE_GHOST,
    };
    static constexpr const char* s_battle_type[] = {
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
    static constexpr const char* s_target_type[] = {
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
        GAUGE_PATTACK,  // placeholder
        GAUGE_GATTACK,  // placeholder
        GAUGE_EXHARD,
        GAUGE_ASSISTEASY,
    };
    static constexpr const char* s_gauge_type[] = {
        "NORMAL",
        "HARD",
        "DEATH",
        "EASY",
        "PATK_NOT_IMPLEMENTED",
        "GATK_NOT_IMPLEMENTED",
        "EX-HARD",
        "ASSIST-EASY",
    };

    enum e_random_type {
        RAN_NORMAL,
        RAN_MIRROR,
        RAN_RANDOM,
        RAN_SRAN,
        RAN_HRAN,
        RAN_ALLSCR,
        RAN_RRAN,
        RAN_DB_SYNCHRONIZE_RANDOM,
        RAN_DB_SYMMETRY_RANDOM,
    };
    static constexpr const char* s_random_type[] = {
        "NORMAL",
        "MIRROR",
        "RANDOM",
        "S-RANDOM",
        "H-RANDOM",
        "ALL-SCR",
        "R-RANDOM",
        "SYNCHRONIZE",
        "SYMMETRY"
    };
    static constexpr const char* s_random_type_short[] = {
        "",
        "MIR",
        "RAN",
        "S-RAN",
        "H-RAN",
        "ALL-SCR",
        "R-RAN",
        "SYN",
        "SYM"
    };

    enum e_assist_type {
        ASSIST_NONE,
        ASSIST_AUTOSCR,
    };
    static constexpr const char* s_assist_type[] = {
        "NONE",
        "AUTO-SCR",
    };
    static constexpr const char* s_assist_type_short[] = {
        "NONE",
        "AS",
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
        if (health >= 1.0)     return e_health_range::HEALTH_100;
        else if (health >= 0.9) return e_health_range::HEALTH_90;
        else if (health >= 0.8) return e_health_range::HEALTH_80;
        else if (health >= 0.7) return e_health_range::HEALTH_70;
        else if (health >= 0.6) return e_health_range::HEALTH_60;
        else if (health >= 0.5) return e_health_range::HEALTH_50;
        else if (health >= 0.4) return e_health_range::HEALTH_40;
        else if (health >= 0.3) return e_health_range::HEALTH_30;
        else if (health >= 0.2) return e_health_range::HEALTH_20;
        else if (health >= 0.1) return e_health_range::HEALTH_10;
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
        JUDGE_0,    // PG
        JUDGE_1,    // GR
        JUDGE_2,    // GD
        JUDGE_3,    // BD
        JUDGE_4,    // PR
        JUDGE_5,    // KP
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
    static constexpr const char* s_windowed[] = {
        "FULL",
        "BORDERLESS",
        "WINDOW",
    };

    enum e_vsync_mode {
        VSYNC_OFF,
        VSYNC_ON,
        VSYNC_ADAPTIVE
    };
    static constexpr const char* s_vsync_mode[] = {
        "OFF",
        "ON",
        "ADAPTIVE",
    };

    enum e_course_type {
        COURSE_NONSTOP,
        COURSE_EXPERT,
        COURSE_GRADE,
    };
}


