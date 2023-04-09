#pragma once

/*
* !!! WARNING !!!
* These indices are currently DIRECTLY casted to LR2skin indices.
* When adding new value, do not modify existing values.
*/
enum class IndexText: unsigned
{
    INVALID = 0,           // should be initialized with ""

    TARGET_NAME = 1,
    PLAYER_NAME,

    PLAY_TITLE = 10,
    PLAY_SUBTITLE,
    PLAY_FULLTITLE,
    PLAY_GENRE,
    PLAY_ARTIST,
    PLAY_SUBARTIST,
    PLAY_TAG,
    PLAY_PLAYLEVEL,
    PLAY_DIFFICULTY,
    
    EDIT_TITLE = 20,
    EDIT_SUBTITLE,
    EDIT_FULLTITLE,
    EDIT_GENRE,
    EDIT_ARTIST,
    EDIT_SUBARTIST,
    EDIT_TAG,
    EDIT_PLAYLEVEL,
    EDIT_DIFFICULTY,
    EDIT_INSANE_LEVEL,
    EDIT_JUKEBOX_NAME,

    KEYCONFIG_SLOT1 = 40,
    KEYCONFIG_SLOT2,
    KEYCONFIG_SLOT3,
    KEYCONFIG_SLOT4,
    KEYCONFIG_SLOT5,
    KEYCONFIG_SLOT6,
    KEYCONFIG_SLOT7,
    KEYCONFIG_SLOT8,        // not defined
    KEYCONFIG_SLOT9,        // not defined
    KEYCONFIG_SLOT10,

    SKIN_NAME = 50,
    SKIN_MAKER_NAME,

    FILTER_KEYS = 60,         // #MODE, ALL KEYS, SINGLE, 7KEYS, 5KEYS, DOUBLE, 14KEYS, 10KEYS, 9KEYS, 
    SORT_MODE,              // #SORT, OFF, TITLE, LEVEL, FOLDER, CLEAR, RANK, 
    FILTER_DIFFICULTY,             // #DIFFICULTY, ALL, EASY, STANDARD, HARD, EXPERT, ULTIMATE,

    RANDOM_1P,
    RANDOM_2P,
    GAUGE_1P,
    GAUGE_2P,
    ASSIST_1P,
    ASSIST_2P,
    BATTLE,
    FLIP,
    SCORE_GRAPH,
    GHOST,
    SHUTTER,
    SCROLL_TYPE,
    BGA_SIZE,
    BGA,
    COLOR_DEPTH,
    VSYNC,
    WINDOWMODE,             // Fullscreen, Windowed
    JUDGE_AUTO,
    REPLAY_SAVE_TYPE,
    TRIAL1,
    TRIAL2,
    EFFECT_1P,
    EFFECT_2P,

    スキンカスタマイズカテゴリ名1個目 = 100,
    スキンカスタマイズカテゴリ名2個目 = 101,
    スキンカスタマイズカテゴリ名3個目 = 102,
    スキンカスタマイズカテゴリ名4個目 = 103,
    スキンカスタマイズカテゴリ名5個目 = 104,
    スキンカスタマイズカテゴリ名6個目 = 105,
    スキンカスタマイズカテゴリ名7個目 = 106,
    スキンカスタマイズカテゴリ名8個目 = 107,
    スキンカスタマイズカテゴリ名9個目 = 108,
    スキンカスタマイズカテゴリ名10個目 = 109,

    スキンカスタマイズ項目名1個目 = 110,
    スキンカスタマイズ項目名2個目 = 111,
    スキンカスタマイズ項目名3個目 = 112,
    スキンカスタマイズ項目名4個目 = 113,
    スキンカスタマイズ項目名5個目 = 114,
    スキンカスタマイズ項目名6個目 = 115,
    スキンカスタマイズ項目名7個目 = 116,
    スキンカスタマイズ項目名8個目 = 117,
    スキンカスタマイズ項目名9個目 = 118,
    スキンカスタマイズ項目名10個目 = 119,

    ランキング表プレイヤー名1人目 = 120,
    ランキング表プレイヤー名2人目 = 121,
    ランキング表プレイヤー名3人目 = 122,
    ランキング表プレイヤー名4人目 = 123,
    ランキング表プレイヤー名5人目 = 124,
    ランキング表プレイヤー名6人目 = 125,
    ランキング表プレイヤー名7人目 = 126,
    ランキング表プレイヤー名8人目 = 127,
    ランキング表プレイヤー名9人目 = 128,
    ランキング表プレイヤー名10人目 = 129,

    クリア文字列_仮_1 = 130,
    クリア文字列_仮_2 = 131,

    COURSE_STAGE1_TITLE = 150,
    COURSE_STAGE2_TITLE,
    COURSE_STAGE3_TITLE,
    COURSE_STAGE4_TITLE,
    COURSE_STAGE5_TITLE,
    COURSE_STAGE6_TITLE,
    COURSE_STAGE7_TITLE,
    COURSE_STAGE8_TITLE,
    COURSE_STAGE9_TITLE,
    COURSE_STAGE10_TITLE,
        
    COURSE_STAGE1_SUBTITLE = 160,
    COURSE_STAGE2_SUBTITLE,
    COURSE_STAGE3_SUBTITLE,
    COURSE_STAGE4_SUBTITLE,
    COURSE_STAGE5_SUBTITLE,
    COURSE_STAGE6_SUBTITLE,
    COURSE_STAGE7_SUBTITLE,
    COURSE_STAGE8_SUBTITLE,
    COURSE_STAGE9_SUBTITLE,
    COURSE_STAGE10_SUBTITLE,

    コースオプション繋ぎモードstage1_2 = 171,
    コースオプション繋ぎモードstage2_3 = 172,
    コースオプション繋ぎモードstage3_4 = 173,
    コースオプション繋ぎモードstage4_5 = 174,
    コースオプション繋ぎモードstage5_6 = 175,
    コースオプション繋ぎモードstage6_7 = 176,
    コースオプション繋ぎモードstage7_8 = 177,
    コースオプション繋ぎモードstage8_9 = 178,
    コースオプション繋ぎモードstage9_10 = 179,



    コースオプションソフラン = 180,
    コースオプションゲージタイプ = 181,
    コースオプションオプション禁止 = 182,
    コースオプションIR = 183,


    ランダムコースオプション最適レベル = 190,

    ランダムコースオプションレベル上限 = 191,

    ランダムコースオプションレベル下限 = 192,

    ランダムコースオプションbpm変動幅 = 193,

    ランダムコースオプションbpm上限 = 194,

    ランダムコースオプションbpm下限 = 195,

    ランダムコースオプションステージ数 = 196,

    全体コースオプションデフォルトつなぎタイプ = 198,

    全体コースオプションデフォルトゲージ = 199,

	// inner texts
    _SELECT_BAR_TITLE_FULL_0 = 200,
    _SELECT_BAR_TITLE_FULL_MAX = 231,

    _OVERLAY_TOPLEFT,
    _OVERLAY_TOPLEFT2,
    _OVERLAY_TOPLEFT3,
    _OVERLAY_TOPLEFT4,

    _OVERLAY_NOTIFICATION_0 = 240,
    _OVERLAY_NOTIFICATION_MAX = 249,

        _TEST1 = 250,
        _TEST2,

        ARENA_LOBBY_STATUS = 260,
        ARENA_PLAYER_NAME_1 = 261,
        ARENA_PLAYER_NAME_2,
        ARENA_PLAYER_NAME_3,
        ARENA_PLAYER_NAME_4,
        ARENA_PLAYER_NAME_5,
        ARENA_PLAYER_NAME_6,
        ARENA_PLAYER_NAME_7,
        ARENA_PLAYER_NAME_8,

        PLAYER_MODIFIER = 270,
        ARENA_MODIFIER_1 = 271,
        ARENA_MODIFIER_2,
        ARENA_MODIFIER_3,
        ARENA_MODIFIER_4,
        ARENA_MODIFIER_5,
        ARENA_MODIFIER_6,
        ARENA_MODIFIER_7,
        ARENA_MODIFIER_8,

        PLAYER_MODIFIER_SHORT = 280,
        ARENA_MODIFIER_SHORT_1 = 281,
        ARENA_MODIFIER_SHORT_2,
        ARENA_MODIFIER_SHORT_3,
        ARENA_MODIFIER_SHORT_4,
        ARENA_MODIFIER_SHORT_5,
        ARENA_MODIFIER_SHORT_6,
        ARENA_MODIFIER_SHORT_7,
        ARENA_MODIFIER_SHORT_8,

        KEYCONFIG_HINT_KEY = 300,
        KEYCONFIG_HINT_BIND,
        KEYCONFIG_HINT_DEADZONE,
        KEYCONFIG_HINT_F1,
        KEYCONFIG_HINT_F2,
        KEYCONFIG_HINT_DEL,
        KEYCONFIG_HINT_SCRATCH_ABS,

        KEYCONFIG_1P_1 = 311,
        KEYCONFIG_1P_2,
        KEYCONFIG_1P_3,
        KEYCONFIG_1P_4,
        KEYCONFIG_1P_5,
        KEYCONFIG_1P_6,
        KEYCONFIG_1P_7,
        KEYCONFIG_1P_8,
        KEYCONFIG_1P_9,
        KEYCONFIG_1P_SCRATCH_L,
        KEYCONFIG_1P_SCRATCH_R,
        KEYCONFIG_1P_START,
        KEYCONFIG_1P_SELECT,
        KEYCONFIG_1P_SCRATCH_ABS,

        KEYCONFIG_2P_1 = 331,
        KEYCONFIG_2P_2,
        KEYCONFIG_2P_3,
        KEYCONFIG_2P_4,
        KEYCONFIG_2P_5,
        KEYCONFIG_2P_6,
        KEYCONFIG_2P_7,
        KEYCONFIG_2P_8,
        KEYCONFIG_2P_9,
        KEYCONFIG_2P_SCRATCH_L,
        KEYCONFIG_2P_SCRATCH_R,
        KEYCONFIG_2P_START,
        KEYCONFIG_2P_SELECT,
        KEYCONFIG_2P_SCRATCH_ABS,

        KEYCONFIG_1P_BIND_1 = 351,
        KEYCONFIG_1P_BIND_2,
        KEYCONFIG_1P_BIND_3,
        KEYCONFIG_1P_BIND_4,
        KEYCONFIG_1P_BIND_5,
        KEYCONFIG_1P_BIND_6,
        KEYCONFIG_1P_BIND_7,
        KEYCONFIG_1P_BIND_8,
        KEYCONFIG_1P_BIND_9,
        KEYCONFIG_1P_BIND_SCRATCH_L,
        KEYCONFIG_1P_BIND_SCRATCH_R,
        KEYCONFIG_1P_BIND_START,
        KEYCONFIG_1P_BIND_SELECT,
        KEYCONFIG_1P_BIND_SCRATCH_ABS,
        KEYCONFIG_1P_SCRATCH_ABS_VALUE,

        KEYCONFIG_2P_BIND_1 = 371,
        KEYCONFIG_2P_BIND_2,
        KEYCONFIG_2P_BIND_3,
        KEYCONFIG_2P_BIND_4,
        KEYCONFIG_2P_BIND_5,
        KEYCONFIG_2P_BIND_6,
        KEYCONFIG_2P_BIND_7,
        KEYCONFIG_2P_BIND_8,
        KEYCONFIG_2P_BIND_9,
        KEYCONFIG_2P_BIND_SCRATCH_L,
        KEYCONFIG_2P_BIND_SCRATCH_R,
        KEYCONFIG_2P_BIND_START,
        KEYCONFIG_2P_BIND_SELECT,
        KEYCONFIG_2P_BIND_SCRATCH_ABS,
        KEYCONFIG_2P_SCRATCH_ABS_VALUE,

    TEXT_COUNT
};

enum TextAlign
{
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};
