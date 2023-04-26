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
        { 0, { system, "scene" } }, // ���C���^�C�}�[
        { 1, { system, "start_input" } }, // ���͊J�n�^�C�}�[
        { 2, { system, "fadeout" } }, // �t�F�[�h�A�E�g�^�C�}�[
        { 3, { play, "fail" } }, // �X�^�C�}�[
        { 4, { select, "input_end" } }, // �������͏I���^�C�}�[
        { 10, { select, "list_move" } }, // �ȃ��X�g�ړ��^�C�}�[
        { 11, { select, "list_entry_change" } }, // �ȕύX�^�C�}�[
        // 12 �ȃ��X�g��ړ��^�C�}�[ (��)
        // 13 �ȃ��X�g���ړ��^�C�}�[ (��)
        // 14 �ȃ��X�g��~�^�C�}�[ (��)
        { 15, { select, "readme_open" } }, // ���[�h�~�[�J�n�^�C�}�[
        { 16, { select, "readme_close" } }, // ���[�h�~�[�I���^�C�}�[
        // 17 �R�[�X�Z���N�g�J�n�^�C�}�[
        // 18 �R�[�X�Z���N�g�I���^�C�}�[
        { 21, { select, "panel1_start" } }, // �p�l��1�N��
        { 22, { select, "panel2_start" } }, // �p�l��2�N��
        { 23, { select, "panel3_start" } }, // �p�l��3�N��
        { 24, { select, "panel4_start" } }, // �p�l��4�N��
        { 25, { select, "panel5_start" } }, // �p�l��5�N��
        { 26, { select, "panel6_start" } }, // �p�l��6�N��
        { 27, { select, "panel7_start" } }, // �p�l��7�N��
        { 28, { select, "panel8_start" } }, // �p�l��8�N��
        { 29, { select, "panel9_start" } }, // �p�l��9�N��
        { 31, { select, "panel1_end" } }, // �p�l��1�I��
        { 32, { select, "panel2_end" } }, // �p�l��2�I��
        { 33, { select, "panel3_end" } }, // �p�l��3�I��
        { 34, { select, "panel4_end" } }, // �p�l��4�I��
        { 35, { select, "panel5_end" } }, // �p�l��5�I��
        { 36, { select, "panel6_end" } }, // �p�l��6�I��
        { 37, { select, "panel7_end" } }, // �p�l��7�I��
        { 38, { select, "panel8_end" } }, // �p�l��8�I��
        { 39, { select, "panel9_end" } }, // �p�l��9�I��
        { 40, { play, "ready" } }, // READY
        { 41, { play, "play_start" } }, // �v���C�J�n
        { 42, { play, "gauge_up_1p" } }, // �Q�[�W�㏸�^�C�}�[ 1P
        { 43, { play, "gauge_up_2p" } }, // �Q�[�W�㏸�^�C�}�[ 2P
        { 44, { play, "gauge_max_1p" } }, // �Q�[�W�}�b�N�X�^�C�}�[ 1P
        { 45, { play, "gauge_max_2p" } }, // �Q�[�W�}�b�N�X�^�C�}�[ 2P
        { 46, { play, "judge_1p" } }, // �W���b�W�^�C�}�[ 1P
        { 47, { play, "judge_2p" } }, // �W���b�W�^�C�}�[ 2P
        { 48, { play, "fullcombo_1p" } }, // �t���R���^�C�}�[ 1P
        { 49, { play, "fullcombo_2p" } }, // �t���R���^�C�}�[ 2P
        { 50, { play, "bomb_s_1p" } }, // 1P�{���^�C�}�[ �M
        { 51, { play, "bomb_1_1p" } }, // 1P�{���^�C�}�[ 1��
        { 52, { play, "bomb_2_1p" } }, // 1P�{���^�C�}�[ 2��
        { 53, { play, "bomb_3_1p" } }, // 1P�{���^�C�}�[ 3��
        { 54, { play, "bomb_4_1p" } }, // 1P�{���^�C�}�[ 4��
        { 55, { play, "bomb_5_1p" } }, // 1P�{���^�C�}�[ 5��
        { 56, { play, "bomb_6_1p" } }, // 1P�{���^�C�}�[ 6��
        { 57, { play, "bomb_7_1p" } }, // 1P�{���^�C�}�[ 7��
        { 58, { play, "bomb_8_1p" } }, // 1P�{���^�C�}�[ 8��
        { 59, { play, "bomb_9_1p" } }, // 1P�{���^�C�}�[ 9��
        { 60, { play, "bomb_s_2p" } }, // 2p�{���^�C�}�[ �M
        { 61, { play, "bomb_1_2p" } }, // 2p�{���^�C�}�[ 1��
        { 62, { play, "bomb_2_2p" } }, // 2p�{���^�C�}�[ 2��
        { 63, { play, "bomb_3_2p" } }, // 2p�{���^�C�}�[ 3��
        { 64, { play, "bomb_4_2p" } }, // 2p�{���^�C�}�[ 4��
        { 65, { play, "bomb_5_2p" } }, // 2p�{���^�C�}�[ 5��
        { 66, { play, "bomb_6_2p" } }, // 2p�{���^�C�}�[ 6��
        { 67, { play, "bomb_7_2p" } }, // 2p�{���^�C�}�[ 7��
        { 68, { play, "bomb_8_2p" } }, // 2p�{���^�C�}�[ 8��
        { 69, { play, "bomb_9_2p" } }, // 2p�{���^�C�}�[ 9��
        { 70, { play, "bomb_ln_s_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ �M
        { 71, { play, "bomb_ln_1_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 1��
        { 72, { play, "bomb_ln_2_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 2��
        { 73, { play, "bomb_ln_3_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 3��
        { 74, { play, "bomb_ln_4_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 4��
        { 75, { play, "bomb_ln_5_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 5��
        { 76, { play, "bomb_ln_6_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 6��
        { 77, { play, "bomb_ln_7_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 7��
        { 78, { play, "bomb_ln_8_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 8��
        { 79, { play, "bomb_ln_9_1p" } }, // 1PLN�G�t�F�N�g�^�C�}�[ 9��
        { 80, { play, "bomb_ln_s_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ �M
        { 81, { play, "bomb_ln_1_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 1��
        { 82, { play, "bomb_ln_2_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 2��
        { 83, { play, "bomb_ln_3_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 3��
        { 84, { play, "bomb_ln_4_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 4��
        { 85, { play, "bomb_ln_5_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 5��
        { 86, { play, "bomb_ln_6_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 6��
        { 87, { play, "bomb_ln_7_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 7��
        { 88, { play, "bomb_ln_8_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 8��
        { 89, { play, "bomb_ln_9_2p" } }, // 2pLN�G�t�F�N�g�^�C�}�[ 9��
        { 100, { play, "key_on_s_1p" } }, // 1P�L�[�I���^�C�}�[ �M
        { 101, { play, "key_on_1_1p" } }, // 1P�L�[�I���^�C�}�[ 1��
        { 102, { play, "key_on_2_1p" } }, // 1P�L�[�I���^�C�}�[ 2��
        { 103, { play, "key_on_3_1p" } }, // 1P�L�[�I���^�C�}�[ 3��
        { 104, { play, "key_on_4_1p" } }, // 1P�L�[�I���^�C�}�[ 4��
        { 105, { play, "key_on_5_1p" } }, // 1P�L�[�I���^�C�}�[ 5��
        { 106, { play, "key_on_6_1p" } }, // 1P�L�[�I���^�C�}�[ 6��
        { 107, { play, "key_on_7_1p" } }, // 1P�L�[�I���^�C�}�[ 7��
        { 108, { play, "key_on_8_1p" } }, // 1P�L�[�I���^�C�}�[ 8��
        { 109, { play, "key_on_9_1p" } }, // 1P�L�[�I���^�C�}�[ 9��
        { 110, { play, "key_on_s_2p" } }, // 2p�L�[�I���^�C�}�[ �M
        { 111, { play, "key_on_1_2p" } }, // 2p�L�[�I���^�C�}�[ 1��
        { 112, { play, "key_on_2_2p" } }, // 2p�L�[�I���^�C�}�[ 2��
        { 113, { play, "key_on_3_2p" } }, // 2p�L�[�I���^�C�}�[ 3��
        { 114, { play, "key_on_4_2p" } }, // 2p�L�[�I���^�C�}�[ 4��
        { 115, { play, "key_on_5_2p" } }, // 2p�L�[�I���^�C�}�[ 5��
        { 116, { play, "key_on_6_2p" } }, // 2p�L�[�I���^�C�}�[ 6��
        { 117, { play, "key_on_7_2p" } }, // 2p�L�[�I���^�C�}�[ 7��
        { 118, { play, "key_on_8_2p" } }, // 2p�L�[�I���^�C�}�[ 8��
        { 119, { play, "key_on_9_2p" } }, // 2p�L�[�I���^�C�}�[ 9��
        { 120, { play, "key_off_s_1p" } }, // 1P�L�[�I�t�^�C�}�[ �M
        { 121, { play, "key_off_1_1p" } }, // 1P�L�[�I�t�^�C�}�[ 1��
        { 122, { play, "key_off_2_1p" } }, // 1P�L�[�I�t�^�C�}�[ 2��
        { 123, { play, "key_off_3_1p" } }, // 1P�L�[�I�t�^�C�}�[ 3��
        { 124, { play, "key_off_4_1p" } }, // 1P�L�[�I�t�^�C�}�[ 4��
        { 125, { play, "key_off_5_1p" } }, // 1P�L�[�I�t�^�C�}�[ 5��
        { 126, { play, "key_off_6_1p" } }, // 1P�L�[�I�t�^�C�}�[ 6��
        { 127, { play, "key_off_7_1p" } }, // 1P�L�[�I�t�^�C�}�[ 7��
        { 128, { play, "key_off_8_1p" } }, // 1P�L�[�I�t�^�C�}�[ 8��
        { 129, { play, "key_off_9_1p" } }, // 1P�L�[�I�t�^�C�}�[ 9��
        { 130, { play, "key_off_s_2p" } }, // 2p�L�[�I�t�^�C�}�[ �M
        { 131, { play, "key_off_1_2p" } }, // 2p�L�[�I�t�^�C�}�[ 1��
        { 132, { play, "key_off_2_2p" } }, // 2p�L�[�I�t�^�C�}�[ 2��
        { 133, { play, "key_off_3_2p" } }, // 2p�L�[�I�t�^�C�}�[ 3��
        { 134, { play, "key_off_4_2p" } }, // 2p�L�[�I�t�^�C�}�[ 4��
        { 135, { play, "key_off_5_2p" } }, // 2p�L�[�I�t�^�C�}�[ 5��
        { 136, { play, "key_off_6_2p" } }, // 2p�L�[�I�t�^�C�}�[ 6��
        { 137, { play, "key_off_7_2p" } }, // 2p�L�[�I�t�^�C�}�[ 7��
        { 138, { play, "key_off_8_2p" } }, // 2p�L�[�I�t�^�C�}�[ 8��
        { 139, { play, "key_off_9_2p" } }, // 2p�L�[�I�t�^�C�}�[ 9��
        // 140 ���Y���^�C�}�[
        { 143, { play, "last_note_1p" } }, // �ŏI�m�[�g�^�C�}�[ 1P
        { 144, { play, "last_note_2p" } }, // �ŏI�m�[�g�^�C�}�[ 2P
        { 150, { result, "graph_start" } }, // ���U���g�O���t�`��J�n�^�C�}�[
        { 151, { result, "graph_end" } }, // ���U���g�O���t�`��I���^�C�}�[(�v���C�����N�\���^�C�}�[
        { 152, { result, "sub_page" } }, // ���U���g�n�C�X�R�A�X�V�^�C�}�[
        // 170 �f�[�^�x�[�X���[�h���^�C�}�[
        // 171 �f�[�^�x�[�X���[�h�����^�C�}�[
        // 172 IR �ڑ��J�n�^�C�}�[
        // 173 IR �ڑ������^�C�}�[
        // 174 IR �ڑ����s�^�C�}�[
        { 180, { select, "course_edit_start" } }, // �R�[�X�G�f�B�^�[�J�n�^�C�}�[
        { 181, { select, "course_edit_end" } }, // �R�[�X�G�f�B�^�[�I���^�C�}�[

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

    if (n == 140) // 140 ���Y���^�C�}�[
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