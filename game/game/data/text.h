#pragma once
#include "buffered_global.h"

enum class eText: unsigned
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

    SKIN_NAME = 50,
    SKIN_MAKER_NAME,

    PLAY_MODE = 60,         // #MODE, ALL KEYS, SINGLE, 7KEYS, 5KEYS, DOUBLE, 14KEYS, 10KEYS, 9KEYS, 
    SORT_MODE,              // #SORT, OFF, TITLE, LEVEL, FOLDER, CLEAR, RANK, 
    DIFFICULTY,             // #DIFFICULTY, ALL, EASY, STANDARD, HARD, EXPERT, ULTIMATE,

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

    クリア文字列（仮）1 = 130,
    クリア文字列（仮）2 = 131,

    コース曲タイトル1stage = 150,
    コース曲タイトル2stage = 151,
    コース曲タイトル3stage = 152,
    コース曲タイトル4stage = 153,
    コース曲タイトル5stage = 154,
    コース曲タイトル6stage = 155,
    コース曲タイトル7stage = 156,
    コース曲タイトル8stage = 157,
    コース曲タイトル9stage = 158,
    コース曲タイトル10stage = 159,

    コース曲サブタイトル1stage = 160,
    コース曲サブタイトル2stage = 161,
    コース曲サブタイトル3stage = 162,
    コース曲サブタイトル4stage = 163,
    コース曲サブタイトル5stage = 164,
    コース曲サブタイトル6stage = 165,
    コース曲サブタイトル7stage = 166,
    コース曲サブタイトル8stage = 167,
    コース曲サブタイトル9stage = 168,
    コース曲サブタイトル10stage = 169,

    コースオプション繋ぎモードstage1－2 = 171,
    コースオプション繋ぎモードstage2－3 = 172,
    コースオプション繋ぎモードstage3－4 = 173,
    コースオプション繋ぎモードstage4－5 = 174,
    コースオプション繋ぎモードstage5－6 = 175,
    コースオプション繋ぎモードstage6－7 = 176,
    コースオプション繋ぎモードstage7－8 = 177,
    コースオプション繋ぎモードstage8－9 = 178,
    コースオプション繋ぎモードstage9－10 = 179,



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

	_TEST1 = 250,
	_TEST2,


    TEXT_COUNT
};

enum TextAlign
{
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

inline buffered_global<eText, std::string, (size_t)eText::TEXT_COUNT> gTexts;