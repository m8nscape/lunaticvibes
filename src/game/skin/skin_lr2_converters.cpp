#include "skin_lr2_converters.h"
#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"

namespace lr2skin
{

IndexNumber num(int n)
{
    return IndexNumber(n);
}

IndexTimer timer(int n)
{
    return IndexTimer(n);
}

IndexText text(int n)
{
    return IndexText(n);
}

static const std::vector<std::variant<std::monostate, IndexSwitch, IndexOption, unsigned>> buttonAdapter{
    // 0
    std::monostate(),

    // 1~9
    IndexSwitch::SELECT_PANEL1,
    IndexSwitch::SELECT_PANEL2,
    IndexSwitch::SELECT_PANEL3,
    IndexSwitch::SELECT_PANEL4,
    IndexSwitch::SELECT_PANEL5,
    IndexSwitch::SELECT_PANEL6,
    IndexSwitch::SELECT_PANEL7,
    IndexSwitch::SELECT_PANEL8,
    IndexSwitch::SELECT_PANEL9,

    // 10~12
    IndexOption::SELECT_FILTER_DIFF,
    IndexOption::SELECT_FILTER_KEYS,
    IndexOption::SELECT_SORT,

    // 13~19
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    // 20~28
    IndexOption::SOUND_FX0,
    IndexOption::SOUND_FX1,
    IndexOption::SOUND_FX2,
    IndexSwitch::SOUND_FX0,
    IndexSwitch::SOUND_FX1,
    IndexSwitch::SOUND_FX2,
    IndexOption::SOUND_TARGET_FX0,
    IndexOption::SOUND_TARGET_FX1,
    IndexOption::SOUND_TARGET_FX2,

    //29~30
    IndexSwitch::SOUND_EQ,	// EQ off/on
    IndexSwitch::_FALSE,	// EQ Preset

    //31~33
    IndexSwitch::SOUND_VOLUME,		// volume control
    IndexSwitch::SOUND_PITCH,
    IndexOption::SOUND_PITCH_TYPE,

    //34~39
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    //40~45
    IndexOption::PLAY_GAUGE_TYPE_1P,
    IndexOption::PLAY_GAUGE_TYPE_2P,
    IndexOption::PLAY_RANDOM_TYPE_1P,
    IndexOption::PLAY_RANDOM_TYPE_2P,
    IndexSwitch::PLAY_OPTION_AUTOSCR_1P,
    IndexSwitch::PLAY_OPTION_AUTOSCR_2P,

    //46~49
    IndexSwitch::P1_LANECOVER_ENABLED,	    // lanecover
    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,	// reserved
    IndexSwitch::_FALSE,	// reserved

    //50~51
    IndexOption::PLAY_LANE_EFFECT_TYPE_1P,
    IndexOption::PLAY_LANE_EFFECT_TYPE_2P,

    IndexSwitch::_FALSE,	// reserved
    IndexSwitch::_FALSE,	// reserved

    //54
    IndexSwitch::PLAY_OPTION_DP_FLIP,
    IndexOption::PLAY_HSFIX_TYPE,
    IndexOption::PLAY_BATTLE_TYPE,
    IndexSwitch::_FALSE,	// HS-1P
    IndexSwitch::_FALSE,	// HS-2P
    IndexSwitch::_FALSE,

    // 60 (undefined)
    IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,

    // 70
    IndexSwitch::SYSTEM_SCOREGRAPH,
    IndexOption::PLAY_GHOST_TYPE_1P,
    IndexOption::PLAY_BGA_TYPE,	// bga off/on/autoplay only, special
    IndexOption::PLAY_BGA_SIZE, // bga normal/extend, special
    IndexSwitch::_FALSE,// JUDGE TIMING
    IndexSwitch::_FALSE,// AUTO ADJUST, not supported
    IndexSwitch::_FALSE, // default target rate
    IndexOption::PLAY_TARGET_TYPE, // target

    IndexSwitch::_FALSE,
    IndexSwitch::_FALSE,

    // 80
IndexOption::SYS_WINDOWED, // screen mode full/window, special
IndexSwitch::_FALSE, // color mode, 32bit fixed
IndexOption::SYS_VSYNC, // vsync, special
2u, //save replay, not supported
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

//90
IndexSwitch::_FALSE,//off/favorite/ignore, not supported
IndexSwitch::_FALSE,	// select all
IndexSwitch::_FALSE,	// select beginner
IndexSwitch::_FALSE,	// select normal
IndexSwitch::_FALSE,	// select hyper
IndexSwitch::_FALSE,	// select another
IndexSwitch::_FALSE,	// select insane

IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 100
IndexSwitch::_FALSE,
IndexSwitch::K11_CONFIG,
IndexSwitch::K12_CONFIG,
IndexSwitch::K13_CONFIG,
IndexSwitch::K14_CONFIG,
IndexSwitch::K15_CONFIG,
IndexSwitch::K16_CONFIG,
IndexSwitch::K17_CONFIG,
IndexSwitch::K18_CONFIG,
IndexSwitch::K19_CONFIG,
IndexSwitch::S1L_CONFIG,
IndexSwitch::S1R_CONFIG,
IndexSwitch::K1START_CONFIG,
IndexSwitch::K1SELECT_CONFIG,
IndexSwitch::K1SPDUP_CONFIG,    // new 114
IndexSwitch::K1SPDDN_CONFIG,    // new 115
IndexSwitch::S1A_CONFIG,        // new 116
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 120
IndexSwitch::_FALSE,
IndexSwitch::K21_CONFIG,
IndexSwitch::K22_CONFIG,
IndexSwitch::K23_CONFIG,
IndexSwitch::K24_CONFIG,
IndexSwitch::K25_CONFIG,
IndexSwitch::K26_CONFIG,
IndexSwitch::K27_CONFIG,
IndexSwitch::K28_CONFIG,
IndexSwitch::K29_CONFIG,
IndexSwitch::S2L_CONFIG,
IndexSwitch::S2R_CONFIG,
IndexSwitch::K2START_CONFIG,
IndexSwitch::K2SELECT_CONFIG,
IndexSwitch::K2SPDUP_CONFIG,    // new 134
IndexSwitch::K2SPDDN_CONFIG,    // new 135
IndexSwitch::S2A_CONFIG,        // new 136
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 140
IndexOption::KEY_CONFIG_KEY7,
IndexOption::KEY_CONFIG_KEY9,
IndexOption::KEY_CONFIG_KEY5,
IndexOption::KEY_CONFIG_MODE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 150
IndexSwitch::KEY_CONFIG_SLOT0,
IndexSwitch::KEY_CONFIG_SLOT1,
IndexSwitch::KEY_CONFIG_SLOT2,
IndexSwitch::KEY_CONFIG_SLOT3,
IndexSwitch::KEY_CONFIG_SLOT4,
IndexSwitch::KEY_CONFIG_SLOT5,
IndexSwitch::KEY_CONFIG_SLOT6,
IndexSwitch::KEY_CONFIG_SLOT7,
IndexSwitch::KEY_CONFIG_SLOT8,
IndexSwitch::KEY_CONFIG_SLOT9,

// 160 (undefined)
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,

// 170
IndexSwitch::SKINSELECT_7KEYS,
IndexSwitch::SKINSELECT_5KEYS,
IndexSwitch::SKINSELECT_14KEYS,
IndexSwitch::SKINSELECT_10KEYS,
IndexSwitch::SKINSELECT_9KEYS,
IndexSwitch::SKINSELECT_SELECT,
IndexSwitch::SKINSELECT_DECIDE,
IndexSwitch::SKINSELECT_RESULT,
IndexSwitch::SKINSELECT_KEYCONFIG,
IndexSwitch::SKINSELECT_SKINSELECT,
IndexSwitch::SKINSELECT_SOUNDSET,
IndexSwitch::SKINSELECT_THEME,
IndexSwitch::SKINSELECT_7KEYS_BATTLE,
IndexSwitch::SKINSELECT_5KEYS_BATTLE,
IndexSwitch::SKINSELECT_9KEYS_BATTLE,
IndexSwitch::SKINSELECT_COURSE_RESULT,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 190
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,

// 200
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,

// 270
IndexOption::RESULT_CLEAR_TYPE_1P,  // FAILED / EASY / GROOVE / HARD / FULLCOMBO / ASSIST-EASY / EX-HARD
IndexOption::RESULT_CLEAR_TYPE_2P,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,
IndexSwitch::_FALSE,

// 280
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,
IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE, IndexSwitch::_FALSE,

// 300
IndexSwitch::_FALSE,
IndexOption::ARENA_PLAYDATA_CLEAR_TYPE, // NOPLAY / FAILED / ASSIST-EASY / EASY / GROOVE / HARD / EX-HARD / FULLCOMBO
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 1),
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 2),
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 3),
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 4),
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 5),
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 6),
IndexOption(int(IndexOption::ARENA_PLAYDATA_CLEAR_TYPE) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 7),
IndexSwitch::_FALSE,

// 310
IndexOption::RESULT_ARENA_PLAYER_RANKING,
IndexOption::ARENA_PLAYDATA_RANKING,    // ABSENT / 1st / 2nd / 3rd / 4th / 5th / 6th / 7th / 8th / 9th
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 1),
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 2),
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 3),
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 4),
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 5),
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 6),
IndexOption(int(IndexOption::ARENA_PLAYDATA_RANKING) + (int(IndexOption::ARENA_PLAYDATA_MAX) - int(IndexOption::ARENA_PLAYDATA_BASE) + 1) * 7),
IndexSwitch::_FALSE,
};

bool buttonSw(int n, IndexSwitch& out)
{
    if (n < 0 || n >= buttonAdapter.size()) 
        return false;
    if (auto sw = std::get_if<IndexSwitch>(&buttonAdapter[n]))
    {
        out = *sw;
        return true;
    }
    else
        return false;
}
bool buttonOp(int n, IndexOption& out)
{
    if (n < 0 || n >= buttonAdapter.size())
        return false;
    if (auto op = std::get_if<IndexOption>(&buttonAdapter[n]))
    {
        out = *op;
        return true;
    }
    else
        return false;
}
bool buttonFixed(int n, unsigned& out)
{
    if (n < 0 || n >= buttonAdapter.size())
        return false;
    if (auto sw = std::get_if<unsigned>(&buttonAdapter[n]))
    {
        out = *sw;
        return true;
    }
    else
        return false;
}

std::function<int()> num1(int n)
{
    using namespace lv;

    auto getCurrentSelectedChart = []() -> std::shared_ptr<ChartFormatBase>
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        auto p = std::dynamic_pointer_cast<EntryChart>(SelectData.entries[SelectData.selectedEntryIndex].first);
        if (p == nullptr)
            return nullptr;
        return p->getChart();
    };

    auto getCurrentSelectedSong = []() -> std::shared_ptr<EntryFolderSong>
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        auto p = std::dynamic_pointer_cast<EntryChart>(SelectData.entries[SelectData.selectedEntryIndex].first);
        if (p == nullptr)
            return nullptr;
        return p->getSongEntry();
    };

    switch (n)
    {
    case 10:
        return [] { return int(PlayData.player[PLAYER_SLOT_PLAYER].hispeed * 100); };
    case 11:
        return [] { return int(PlayData.player[PLAYER_SLOT_TARGET].hispeed * 100); };
    case 12:
        return [] { return PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual; };
    case 13:
        return [] { return PlayData.ghostType; };
    case 14:
        return [] { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop / 10; };
    case 15:
        return [] { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom / 10; };
    case 20:
        return [] { return (int)SystemData.currentRenderFPS; };
    case 21:
        return [] { return (int)SystemData.dateYear; };
    case 22:
        return [] { return (int)SystemData.dateMonthOfYear; };
    case 23:
        return [] { return (int)SystemData.dateDayOfMonth; };
    case 24:
        return [] { return (int)SystemData.timeHour; };
    case 25:
        return [] { return (int)SystemData.timeMin; };
    case 26:
        return [] { return (int)SystemData.timeSec; };
    case 30:
        return [] { return ProfileData.playCount; };
    case 31:
        return [] { return ProfileData.clearCount; };
    case 32:
        return [] { return ProfileData.failCount; };
    case 33:
        return [] { return 1234; };
    case 34:
        return [] { return 5678; };
    case 35:
        return [] { return 111; };
    case 36:
        return [] { return 22; };
    case 37:
        return [] { return 3; };
    case 38:
        return [] { return 0; };
    case 39:
        return [] { return 3076; };
    case 40:
        return [] { return 999; };
    case 41:
        return [] { return 0; };

    case 45:
    case 46:
    case 47:
    case 48:
    case 49:
        return [] { return -1; };

    case 50:
        return [] { return SystemData.equalizerVal62_5hz; };
    case 51:
        return [] { return SystemData.equalizerVal160hz; };
    case 52:
        return [] { return SystemData.equalizerVal400hz; };
    case 53:
        return [] { return SystemData.equalizerVal1khz; };
    case 54:
        return [] { return SystemData.equalizerVal2_5khz; };
    case 55:
        return [] { return SystemData.equalizerVal6_25khz; };
    case 56:
        return [] { return SystemData.equalizerVal16khz; };
    case 57:
        return [] { return int(SystemData.volumeMaster * 100); };
    case 58:
        return [] { return int(SystemData.volumeKey * 100); };
    case 59:
        return [] { return int(SystemData.volumeBgm * 100); };
    case 60:
        return [] { return int(SystemData.fxVal * 100); };
    case 66:
        return [] { return int(SystemData.freqVal * 100); };

    case 71:
        return [] {
            auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
            if (r)
            {
                return (int)r->getExScore();
            }
            return 0;
        };
    case 72:
        return [] {
            auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
            if (r)
            {
                return (int)r->getMaxScore();
            }
            return 0;
        };
    case 73:
        return [] {
            auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
            if (r)
            {
                return int(r->getData().acc * 100);
            }
            return 0;
        };
    case 74:
        return [] {
            auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
            if (r)
            {
                return (int)r->getNoteCount();
            }
            return 0;
        };
    case 75:
        return [] {
            auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
            if (r)
            {
                return (int)r->getData().maxCombo;
            }
            return 0;
        };
    case 76:
        return [] {
            auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
            if (r)
            {
                return 0;
            }
            return 0;
        };
    }
}

std::function<long long()> timer1(int n)
{
    using namespace lv;

    auto system = [](const char* key)
    {
        if (SystemData.timer.find(key) == SystemData.timer.end())
            return LLONG_MAX;
        return SystemData.timer.at(key);
    };

    auto select = [](const char* key)
    {
        if (SelectData.timer.find(key) == SelectData.timer.end())
            return LLONG_MAX;
        return SelectData.timer.at(key);
    };

    auto play = [](const char* key)
    {
        if (PlayData.timer.find(key) == PlayData.timer.end())
            return LLONG_MAX;
        return PlayData.timer.at(key);
    };

    auto result = [](const char* key)
    {
        if (ResultData.timer.find(key) == ResultData.timer.end())
            return LLONG_MAX;
        return ResultData.timer.at(key);
    };

    auto arena = [](const char* key)
    {
        if (ArenaData.timer.find(key) == ArenaData.timer.end())
            return LLONG_MAX;
        return ArenaData.timer.at(key);
    };

    static const std::map<int, std::pair<std::function<long long(const char*)>, const char*>> timers =
    {
        { 0, { system, "scene" } }, // メインタイマー
        { 1, { system, "start_input" } }, // 入力開始タイマー
        { 2, { system, "fadeout" } }, // フェードアウトタイマー
        { 3, { play, "fail" } }, // 閉店タイマー
        { 4, { select, "input_end" } }, // 文字入力終了タイマー
        { 10, { select, "list_move" } }, // 曲リスト移動タイマー
        { 11, { select, "list_entry_change" } }, // 曲変更タイマー
        // 12 曲リスト上移動タイマー (未)
        // 13 曲リスト下移動タイマー (未)
        // 14 曲リスト停止タイマー (未)
        { 15, { select, "readme_open" } }, // リードミー開始タイマー
        { 16, { select, "readme_close" } }, // リードミー終了タイマー
        // 17 コースセレクト開始タイマー
        // 18 コースセレクト終了タイマー
        { 21, { select, "panel1_start" } }, // パネル1起動
        { 22, { select, "panel2_start" } }, // パネル2起動
        { 23, { select, "panel3_start" } }, // パネル3起動
        { 24, { select, "panel4_start" } }, // パネル4起動
        { 25, { select, "panel5_start" } }, // パネル5起動
        { 26, { select, "panel6_start" } }, // パネル6起動
        { 27, { select, "panel7_start" } }, // パネル7起動
        { 28, { select, "panel8_start" } }, // パネル8起動
        { 29, { select, "panel9_start" } }, // パネル9起動
        { 31, { select, "panel1_end" } }, // パネル1終了
        { 32, { select, "panel2_end" } }, // パネル2終了
        { 33, { select, "panel3_end" } }, // パネル3終了
        { 34, { select, "panel4_end" } }, // パネル4終了
        { 35, { select, "panel5_end" } }, // パネル5終了
        { 36, { select, "panel6_end" } }, // パネル6終了
        { 37, { select, "panel7_end" } }, // パネル7終了
        { 38, { select, "panel8_end" } }, // パネル8終了
        { 39, { select, "panel9_end" } }, // パネル9終了
        { 40, { play, "ready" } }, // READY
        { 41, { play, "play_start" } }, // プレイ開始
        { 42, { play, "gauge_up_1p" } }, // ゲージ上昇タイマー 1P
        { 43, { play, "gauge_up_2p" } }, // ゲージ上昇タイマー 2P
        { 44, { play, "gauge_max_1p" } }, // ゲージマックスタイマー 1P
        { 45, { play, "gauge_max_2p" } }, // ゲージマックスタイマー 2P
        { 46, { play, "judge_1p" } }, // ジャッジタイマー 1P
        { 47, { play, "judge_2p" } }, // ジャッジタイマー 2P
        { 48, { play, "fullcombo_1p" } }, // フルコンタイマー 1P
        { 49, { play, "fullcombo_2p" } }, // フルコンタイマー 2P
        { 50, { play, "bomb_s_1p" } }, // 1Pボムタイマー 皿
        { 51, { play, "bomb_1_1p" } }, // 1Pボムタイマー 1鍵
        { 52, { play, "bomb_2_1p" } }, // 1Pボムタイマー 2鍵
        { 53, { play, "bomb_3_1p" } }, // 1Pボムタイマー 3鍵
        { 54, { play, "bomb_4_1p" } }, // 1Pボムタイマー 4鍵
        { 55, { play, "bomb_5_1p" } }, // 1Pボムタイマー 5鍵
        { 56, { play, "bomb_6_1p" } }, // 1Pボムタイマー 6鍵
        { 57, { play, "bomb_7_1p" } }, // 1Pボムタイマー 7鍵
        { 58, { play, "bomb_8_1p" } }, // 1Pボムタイマー 8鍵
        { 59, { play, "bomb_9_1p" } }, // 1Pボムタイマー 9鍵
        { 60, { play, "bomb_s_2p" } }, // 2pボムタイマー 皿
        { 61, { play, "bomb_1_2p" } }, // 2pボムタイマー 1鍵
        { 62, { play, "bomb_2_2p" } }, // 2pボムタイマー 2鍵
        { 63, { play, "bomb_3_2p" } }, // 2pボムタイマー 3鍵
        { 64, { play, "bomb_4_2p" } }, // 2pボムタイマー 4鍵
        { 65, { play, "bomb_5_2p" } }, // 2pボムタイマー 5鍵
        { 66, { play, "bomb_6_2p" } }, // 2pボムタイマー 6鍵
        { 67, { play, "bomb_7_2p" } }, // 2pボムタイマー 7鍵
        { 68, { play, "bomb_8_2p" } }, // 2pボムタイマー 8鍵
        { 69, { play, "bomb_9_2p" } }, // 2pボムタイマー 9鍵
        { 70, { play, "bomb_ln_s_1p" } }, // 1PLNエフェクトタイマー 皿
        { 71, { play, "bomb_ln_1_1p" } }, // 1PLNエフェクトタイマー 1鍵
        { 72, { play, "bomb_ln_2_1p" } }, // 1PLNエフェクトタイマー 2鍵
        { 73, { play, "bomb_ln_3_1p" } }, // 1PLNエフェクトタイマー 3鍵
        { 74, { play, "bomb_ln_4_1p" } }, // 1PLNエフェクトタイマー 4鍵
        { 75, { play, "bomb_ln_5_1p" } }, // 1PLNエフェクトタイマー 5鍵
        { 76, { play, "bomb_ln_6_1p" } }, // 1PLNエフェクトタイマー 6鍵
        { 77, { play, "bomb_ln_7_1p" } }, // 1PLNエフェクトタイマー 7鍵
        { 78, { play, "bomb_ln_8_1p" } }, // 1PLNエフェクトタイマー 8鍵
        { 79, { play, "bomb_ln_9_1p" } }, // 1PLNエフェクトタイマー 9鍵
        { 80, { play, "bomb_ln_s_2p" } }, // 2pLNエフェクトタイマー 皿
        { 81, { play, "bomb_ln_1_2p" } }, // 2pLNエフェクトタイマー 1鍵
        { 82, { play, "bomb_ln_2_2p" } }, // 2pLNエフェクトタイマー 2鍵
        { 83, { play, "bomb_ln_3_2p" } }, // 2pLNエフェクトタイマー 3鍵
        { 84, { play, "bomb_ln_4_2p" } }, // 2pLNエフェクトタイマー 4鍵
        { 85, { play, "bomb_ln_5_2p" } }, // 2pLNエフェクトタイマー 5鍵
        { 86, { play, "bomb_ln_6_2p" } }, // 2pLNエフェクトタイマー 6鍵
        { 87, { play, "bomb_ln_7_2p" } }, // 2pLNエフェクトタイマー 7鍵
        { 88, { play, "bomb_ln_8_2p" } }, // 2pLNエフェクトタイマー 8鍵
        { 89, { play, "bomb_ln_9_2p" } }, // 2pLNエフェクトタイマー 9鍵
        { 100, { play, "key_on_s_1p" } }, // 1Pキーオンタイマー 皿
        { 101, { play, "key_on_1_1p" } }, // 1Pキーオンタイマー 1鍵
        { 102, { play, "key_on_2_1p" } }, // 1Pキーオンタイマー 2鍵
        { 103, { play, "key_on_3_1p" } }, // 1Pキーオンタイマー 3鍵
        { 104, { play, "key_on_4_1p" } }, // 1Pキーオンタイマー 4鍵
        { 105, { play, "key_on_5_1p" } }, // 1Pキーオンタイマー 5鍵
        { 106, { play, "key_on_6_1p" } }, // 1Pキーオンタイマー 6鍵
        { 107, { play, "key_on_7_1p" } }, // 1Pキーオンタイマー 7鍵
        { 108, { play, "key_on_8_1p" } }, // 1Pキーオンタイマー 8鍵
        { 109, { play, "key_on_9_1p" } }, // 1Pキーオンタイマー 9鍵
        { 110, { play, "key_on_s_2p" } }, // 2pキーオンタイマー 皿
        { 111, { play, "key_on_1_2p" } }, // 2pキーオンタイマー 1鍵
        { 112, { play, "key_on_2_2p" } }, // 2pキーオンタイマー 2鍵
        { 113, { play, "key_on_3_2p" } }, // 2pキーオンタイマー 3鍵
        { 114, { play, "key_on_4_2p" } }, // 2pキーオンタイマー 4鍵
        { 115, { play, "key_on_5_2p" } }, // 2pキーオンタイマー 5鍵
        { 116, { play, "key_on_6_2p" } }, // 2pキーオンタイマー 6鍵
        { 117, { play, "key_on_7_2p" } }, // 2pキーオンタイマー 7鍵
        { 118, { play, "key_on_8_2p" } }, // 2pキーオンタイマー 8鍵
        { 119, { play, "key_on_9_2p" } }, // 2pキーオンタイマー 9鍵
        { 120, { play, "key_off_s_1p" } }, // 1Pキーオフタイマー 皿
        { 121, { play, "key_off_1_1p" } }, // 1Pキーオフタイマー 1鍵
        { 122, { play, "key_off_2_1p" } }, // 1Pキーオフタイマー 2鍵
        { 123, { play, "key_off_3_1p" } }, // 1Pキーオフタイマー 3鍵
        { 124, { play, "key_off_4_1p" } }, // 1Pキーオフタイマー 4鍵
        { 125, { play, "key_off_5_1p" } }, // 1Pキーオフタイマー 5鍵
        { 126, { play, "key_off_6_1p" } }, // 1Pキーオフタイマー 6鍵
        { 127, { play, "key_off_7_1p" } }, // 1Pキーオフタイマー 7鍵
        { 128, { play, "key_off_8_1p" } }, // 1Pキーオフタイマー 8鍵
        { 129, { play, "key_off_9_1p" } }, // 1Pキーオフタイマー 9鍵
        { 130, { play, "key_off_s_2p" } }, // 2pキーオフタイマー 皿
        { 131, { play, "key_off_1_2p" } }, // 2pキーオフタイマー 1鍵
        { 132, { play, "key_off_2_2p" } }, // 2pキーオフタイマー 2鍵
        { 133, { play, "key_off_3_2p" } }, // 2pキーオフタイマー 3鍵
        { 134, { play, "key_off_4_2p" } }, // 2pキーオフタイマー 4鍵
        { 135, { play, "key_off_5_2p" } }, // 2pキーオフタイマー 5鍵
        { 136, { play, "key_off_6_2p" } }, // 2pキーオフタイマー 6鍵
        { 137, { play, "key_off_7_2p" } }, // 2pキーオフタイマー 7鍵
        { 138, { play, "key_off_8_2p" } }, // 2pキーオフタイマー 8鍵
        { 139, { play, "key_off_9_2p" } }, // 2pキーオフタイマー 9鍵
        // 140 リズムタイマー
        { 143, { play, "last_note_1p" } }, // 最終ノートタイマー 1P
        { 144, { play, "last_note_2p" } }, // 最終ノートタイマー 2P
        { 150, { result, "graph_start" } }, // リザルトグラフ描画開始タイマー
        { 151, { result, "graph_end" } }, // リザルトグラフ描画終了タイマー(プレイランク表示タイマー
        { 152, { result, "sub_page" } }, // リザルトハイスコア更新タイマー
        // 170 データベースロード中タイマー
        // 171 データベースロード完了タイマー
        // 172 IR 接続開始タイマー
        // 173 IR 接続成功タイマー
        // 174 IR 接続失敗タイマー
        { 180, { select, "course_edit_start" } }, // コースエディター開始タイマー
        { 181, { select, "course_edit_end" } }, // コースエディター終了タイマー

        { 200, { play, "key_on_sl_1p" } },
        { 201, { play, "key_off_sl_1p" } },
        { 202, { play, "key_on_sr_1p" } },
        { 203, { play, "key_off_sr_1p" } },
        { 204, { play, "key_on_start_1p" } },
        { 205, { play, "key_on_select_1p" } },
        { 206, { play, "key_on_spdup_1p" } },
        { 207, { play, "key_on_spddn_1p" } },
        { 208, { play, "key_on_start_1p" } },
        { 209, { play, "key_on_select_1p" } },
        { 210, { play, "key_on_spdup_1p" } },
        { 211, { play, "key_on_spddn_1p" } },
        { 212, { play, "scratch_axis_move_1p" } },

        { 220, { play, "key_on_sl_2p" } },
        { 221, { play, "key_off_sl_2p" } },
        { 222, { play, "key_on_sr_2p" } },
        { 223, { play, "key_off_sr_2p" } },
        { 224, { play, "key_on_start_2p" } },
        { 225, { play, "key_on_select_2p" } },
        { 226, { play, "key_on_spdup_2p" } },
        { 227, { play, "key_on_spddn_2p" } },
        { 228, { play, "key_on_start_2p" } },
        { 229, { play, "key_on_select_2p" } },
        { 230, { play, "key_on_spdup_2p" } },
        { 231, { play, "key_on_spddn_2p" } },
        { 232, { play, "scratch_axis_move_2p" } },

        { 240, { arena, "show_lobby" } },
        { 241, { arena, "play_finish_wait" } },
        { 242, { arena, "result_wait" } },
    };

    if (n == 140) // 140 リズムタイマー
    {
        return []() -> long long {
            if (SystemData.gNextScene != SceneType::PLAY)
                return LLONG_MAX;
            if (PlayData.player[PLAYER_SLOT_PLAYER].chartObj == nullptr)
                return LLONG_MAX;
            return int(1000 * (PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getCurrentMetre() * 4.0)) % 1000;
        };
    }
    else if (timers.find(n) != timers.end())
    {
        const auto& [category, key] = timers.at(n);
        return std::bind(category, key);
    }
    else
    {
        return [] { return LLONG_MAX; };
    }
}

}