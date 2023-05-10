#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"
#include "game/arena/arena_data.h"

namespace lv
{

using namespace data;

class ButtonConverter
{
private:
    static int panel(int n)
    {
        return SelectData.panel[n] ? 1 : 0;
    }

    static int keySlot(int n)
    {
        return KeyConfigData.selecting.second == n ? 1 : 0;
    }

public:
    static int button_0() { return 0; }

    static int button_1() { return panel(0); }
    static int button_2() { return panel(1); }
    static int button_3() { return panel(2); }
    static int button_4() { return panel(3); }
    static int button_5() { return panel(4); }
    static int button_6() { return panel(5); }
    static int button_7() { return panel(6); }
    static int button_8() { return panel(7); }
    static int button_9() { return panel(8); }

    static int button_10()
    {
        static const std::map<FilterDifficultyType, int> kv = {
            { FilterDifficultyType::All, 0 },
            { FilterDifficultyType::B, 1 },
            { FilterDifficultyType::N, 2 },
            { FilterDifficultyType::H, 3 },
            { FilterDifficultyType::A, 4 },
            { FilterDifficultyType::I, 5 },
        };
        auto k = SelectData.filterDifficulty;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_11()
    {
        static const std::map<FilterKeysType, int> kv = {
            { FilterKeysType::All, 0 },
            { FilterKeysType::_5, 1 },
            { FilterKeysType::_7, 2 },
            { FilterKeysType::_9, 3 },
            { FilterKeysType::_10, 4 },
            { FilterKeysType::_14, 5 },
        };
        auto k = SelectData.filterKeys;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_12()
    {
        static const std::map<SongListSortType, int> kv = {
            { SongListSortType::DEFAULT, 0 },
            { SongListSortType::TITLE, 1 },
            { SongListSortType::LEVEL, 2 },
            { SongListSortType::CLEAR, 3 },
            { SongListSortType::RATE, 4 },
        };
        auto k = SelectData.sortType;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_20()
    {
        static const std::map<FXType, int> kv = {
            { FXType::Off, 0 },
            { FXType::SfxReverb, 1 },
            { FXType::Echo, 2 },
            { FXType::LowPass, 3 },
            { FXType::HighPass, 4 },
        };
        auto k = SystemData.fxType;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_23()
    {
        return SystemData.fxType == FXType::Off ? 0 : 1;
    }
    static int button_26()
    {
        // fx target
        return 0;
    }
    static int button_29()
    {
        return SystemData.equalizerEnabled ? 1 : 0;
    }
    static int button_31()
    {
        // volume
        return 1;
    }
    static int button_32()
    {
        return SystemData.freqType == FreqModifierType::Off ? 0 : 1;
    }
    static int button_33()
    {
        static const std::map<FreqModifierType, int> kv = {
            { FreqModifierType::Frequency, 0 },
            { FreqModifierType::PitchOnly, 1 },
            { FreqModifierType::SpeedOnly, 2 },
        };
        auto k = SystemData.freqType;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_40()
    {
        static const std::map<PlayModifierGaugeType, int> kv = {
            { PlayModifierGaugeType::NORMAL, 0 },
            { PlayModifierGaugeType::HARD, 1 },
            { PlayModifierGaugeType::DEATH, 2 },
            { PlayModifierGaugeType::EASY, 3 },
            { PlayModifierGaugeType::EXHARD, 6 },
            { PlayModifierGaugeType::ASSISTEASY, 7 },
        };
        auto k = PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_41()
    {
        static const std::map<PlayModifierGaugeType, int> kv = {
            { PlayModifierGaugeType::NORMAL, 0 },
            { PlayModifierGaugeType::HARD, 1 },
            { PlayModifierGaugeType::DEATH, 2 },
            { PlayModifierGaugeType::EASY, 3 },
            { PlayModifierGaugeType::EXHARD, 6 },
            { PlayModifierGaugeType::ASSISTEASY, 7 },
        };
        auto k = PlayData.player[PLAYER_SLOT_TARGET].mods.gauge;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_42()
    {
        static const std::map<PlayModifierRandomType, int> kv = {
            { PlayModifierRandomType::NONE, 0 },
            { PlayModifierRandomType::MIRROR, 1 },
            { PlayModifierRandomType::RANDOM, 2 },
            { PlayModifierRandomType::SRAN, 3 },
            { PlayModifierRandomType::HRAN, 4 },
            { PlayModifierRandomType::ALLSCR, 5 },
            { PlayModifierRandomType::RRAN, 6 },
            { PlayModifierRandomType::DB_SYNCHRONIZE, 7 },
            { PlayModifierRandomType::DB_SYMMETRY, 8 },
        };
        auto k = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_43()
    {
        static const std::map<PlayModifierRandomType, int> kv = {
            { PlayModifierRandomType::NONE, 0 },
            { PlayModifierRandomType::MIRROR, 1 },
            { PlayModifierRandomType::RANDOM, 2 },
            { PlayModifierRandomType::SRAN, 3 },
            { PlayModifierRandomType::HRAN, 4 },
            { PlayModifierRandomType::ALLSCR, 5 },
            { PlayModifierRandomType::RRAN, 6 },
            { PlayModifierRandomType::DB_SYNCHRONIZE, 7 },
            { PlayModifierRandomType::DB_SYMMETRY, 8 },
        };
        auto k = (PlayData.mode == SkinType::PLAY10 || PlayData.mode == SkinType::PLAY14) ?
            PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight :
            PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_44()
    {
        return (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR) ? 1 : 0;
    }
    static int button_45()
    {
        return (PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR) ? 1 : 0;
    }
    static int button_46()
    {
        static const std::map<PlayModifierLaneEffectType, int> kv = {
            { PlayModifierLaneEffectType::OFF, 0 },
            { PlayModifierLaneEffectType::SUDDEN, 1 },
            { PlayModifierLaneEffectType::HIDDEN, 1 },
            { PlayModifierLaneEffectType::SUDHID, 1 },
            { PlayModifierLaneEffectType::LIFT, 0 },
            { PlayModifierLaneEffectType::LIFTSUD, 1 },
        };
        auto k = PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_50()
    {
        static const std::map<PlayModifierLaneEffectType, int> kv = {
            { PlayModifierLaneEffectType::OFF, 0 },
            { PlayModifierLaneEffectType::HIDDEN, 1 },
            { PlayModifierLaneEffectType::SUDDEN, 2 },
            { PlayModifierLaneEffectType::SUDHID, 3 },
            { PlayModifierLaneEffectType::LIFT, 4 },
            { PlayModifierLaneEffectType::LIFTSUD, 5 },
        };
        auto k = PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_51()
    {
        static const std::map<PlayModifierLaneEffectType, int> kv = {
            { PlayModifierLaneEffectType::OFF, 0 },
            { PlayModifierLaneEffectType::HIDDEN, 1 },
            { PlayModifierLaneEffectType::SUDDEN, 2 },
            { PlayModifierLaneEffectType::SUDHID, 3 },
            { PlayModifierLaneEffectType::LIFT, 4 },
            { PlayModifierLaneEffectType::LIFTSUD, 5 },
        };
        auto k = PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_54()
    {
        return PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip ? 1 : 0;
    }
    static int button_55()
    {
        static const std::map<PlayModifierHispeedFixType, int> kv = {
            { PlayModifierHispeedFixType::NONE, 0 },
            { PlayModifierHispeedFixType::MAXBPM, 1 },
            { PlayModifierHispeedFixType::MINBPM, 2 },
            { PlayModifierHispeedFixType::AVERAGE, 3 },
            { PlayModifierHispeedFixType::CONSTANT, 4 },
            { PlayModifierHispeedFixType::INITIAL, 5 },
            { PlayModifierHispeedFixType::MAIN, 6 },
        };
        auto k = PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_56()
    {
        static const std::map<PlayModifierBattleType, int> kv = {
            { PlayModifierBattleType::Off, 0 },
            { PlayModifierBattleType::LocalBattle, 1 },
            { PlayModifierBattleType::DoubleBattle, 2 },
            { PlayModifierBattleType::GhostBattle, 4 },
        };
        auto k = PlayData.battleType;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_70()
    {
        return !!(PlayData.panelStyle & PANEL_STYLE_GRAPH_MASK) ? 1 : 0;
    }
    static int button_71()
    {
        static const std::map<GhostScorePosition, int> kv = {
            { GhostScorePosition::Off, 0 },
            { GhostScorePosition::AboveJudge, 1 },
            { GhostScorePosition::NearJudge, 2 },
            { GhostScorePosition::NearJudgeLower, 3 },
        };
        auto k = PlayData.ghostType;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_72()
    {
        return !!(PlayData.panelStyle & PANEL_STYLE_BGA_OFF) ? 1 : 0;
    }
    static int button_73()
    {
        switch (PlayData.panelStyle & PANEL_STYLE_BGA_MASK)
        {
        case PANEL_STYLE_BGA_SIDE: return 0;
        case PANEL_STYLE_BGA_SIDE_EXPAND:
        case PANEL_STYLE_BGA_FULLSCREEN: return 1;
        }
        return 0;
    }
    static int button_80()
    {
        static const std::map<GameWindowMode, int> kv = {
            { GameWindowMode::FULLSCREEN, 0 },
            { GameWindowMode::BORDERLESS, 1 },
            { GameWindowMode::WINDOWED, 1 },
        };
        auto k = SystemData.windowMode;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_82()
    {
        static const std::map<GameVsyncMode, int> kv = {
            { GameVsyncMode::OFF, 0 },
            { GameVsyncMode::ON, 1 },
            { GameVsyncMode::ADAPTIVE, 1 },
        };
        auto k = SystemData.vsyncMode;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }
    static int button_83()
    {
        // save replay
        return 2;
    }
    static int button_84()
    {
        // favorite / ignore
        return 0;
    }

    static int button_101()
    {
        return KeyConfigData.selecting.first == Input::Pad::K11 ? 1 : 0;
    }
    static int button_102()
    {
        return KeyConfigData.selecting.first == Input::Pad::K12 ? 1 : 0;
    }
    static int button_103()
    {
        return KeyConfigData.selecting.first == Input::Pad::K13 ? 1 : 0;
    }
    static int button_104()
    {
        return KeyConfigData.selecting.first == Input::Pad::K14 ? 1 : 0;
    }
    static int button_105()
    {
        return KeyConfigData.selecting.first == Input::Pad::K15 ? 1 : 0;
    }
    static int button_106()
    {
        return KeyConfigData.selecting.first == Input::Pad::K16 ? 1 : 0;
    }
    static int button_107()
    {
        return KeyConfigData.selecting.first == Input::Pad::K17 ? 1 : 0;
    }
    static int button_108()
    {
        return KeyConfigData.selecting.first == Input::Pad::K18 ? 1 : 0;
    }
    static int button_109()
    {
        return KeyConfigData.selecting.first == Input::Pad::K19 ? 1 : 0;
    }
    static int button_110()
    {
        return KeyConfigData.selecting.first == Input::Pad::S1L ? 1 : 0;
    }
    static int button_111()
    {
        return KeyConfigData.selecting.first == Input::Pad::S1R ? 1 : 0;
    }
    static int button_112()
    {
        return KeyConfigData.selecting.first == Input::Pad::K1START ? 1 : 0;
    }
    static int button_113()
    {
        return KeyConfigData.selecting.first == Input::Pad::K1SELECT ? 1 : 0;
    }
    static int button_121()
    {
        return KeyConfigData.selecting.first == Input::Pad::K21 ? 1 : 0;
    }
    static int button_122()
    {
        return KeyConfigData.selecting.first == Input::Pad::K22 ? 1 : 0;
    }
    static int button_123()
    {
        return KeyConfigData.selecting.first == Input::Pad::K23 ? 1 : 0;
    }
    static int button_124()
    {
        return KeyConfigData.selecting.first == Input::Pad::K24 ? 1 : 0;
    }
    static int button_125()
    {
        return KeyConfigData.selecting.first == Input::Pad::K25 ? 1 : 0;
    }
    static int button_126()
    {
        return KeyConfigData.selecting.first == Input::Pad::K26 ? 1 : 0;
    }
    static int button_127()
    {
        return KeyConfigData.selecting.first == Input::Pad::K27 ? 1 : 0;
    }
    static int button_128()
    {
        return KeyConfigData.selecting.first == Input::Pad::K28 ? 1 : 0;
    }
    static int button_129()
    {
        return KeyConfigData.selecting.first == Input::Pad::K29 ? 1 : 0;
    }
    static int button_130()
    {
        return KeyConfigData.selecting.first == Input::Pad::S1L ? 1 : 0;
    }
    static int button_131()
    {
        return KeyConfigData.selecting.first == Input::Pad::S1R ? 1 : 0;
    }
    static int button_132()
    {
        return KeyConfigData.selecting.first == Input::Pad::K2START ? 1 : 0;
    }
    static int button_133()
    {
        return KeyConfigData.selecting.first == Input::Pad::K2SELECT ? 1 : 0;
    }
    static int button_140()
    {
        int idx = 0;
        switch (KeyConfigData.selecting.first)
        {
        case Input::Pad::K11:      idx = 1; break;
        case Input::Pad::K12:      idx = 2; break;
        case Input::Pad::K13:      idx = 3; break;
        case Input::Pad::K14:      idx = 4; break;
        case Input::Pad::K15:      idx = 5; break;
        case Input::Pad::K16:      idx = 6; break;
        case Input::Pad::K17:      idx = 7; break;
        case Input::Pad::S1L:      idx = 8; break;
        case Input::Pad::S1R:      idx = 9; break;
        case Input::Pad::K1START:  idx = 10; break;
        case Input::Pad::K1SELECT: idx = 11; break;
        case Input::Pad::K21:      idx = 12; break;
        case Input::Pad::K22:      idx = 13; break;
        case Input::Pad::K23:      idx = 14; break;
        case Input::Pad::K24:      idx = 15; break;
        case Input::Pad::K25:      idx = 16; break;
        case Input::Pad::K26:      idx = 17; break;
        case Input::Pad::K27:      idx = 18; break;
        case Input::Pad::S2L:      idx = 19; break;
        case Input::Pad::S2R:      idx = 20; break;
        case Input::Pad::K2START:  idx = 21; break;
        case Input::Pad::K2SELECT: idx = 22; break;
        case Input::Pad::S1A:      idx = 23; break;
        case Input::Pad::S2A:      idx = 24; break;
        default: break;
        }
        return idx;
    }
    static int button_141()
    {
        int idx = 0;
        switch (KeyConfigData.selecting.first)
        {
        case Input::Pad::K11:      idx = 1; break;
        case Input::Pad::K12:      idx = 2; break;
        case Input::Pad::K13:      idx = 3; break;
        case Input::Pad::K14:      idx = 4; break;
        case Input::Pad::K15:      idx = 5; break;
        case Input::Pad::K16:      idx = 6; break;
        case Input::Pad::K17:      idx = 7; break;
        case Input::Pad::K18:      idx = 8; break;
        case Input::Pad::K19:      idx = 9; break;
        case Input::Pad::K1START:  idx = 10; break;
        case Input::Pad::K1SELECT: idx = 11; break;
        default: break;
        }
        return idx;
    }
    static int button_142()
    {
        int idx = 0;
        switch (KeyConfigData.selecting.first)
        {
        case Input::Pad::K11:      idx = 1; break;
        case Input::Pad::K12:      idx = 2; break;
        case Input::Pad::K13:      idx = 3; break;
        case Input::Pad::K14:      idx = 4; break;
        case Input::Pad::K15:      idx = 5; break;
        case Input::Pad::S1L:      idx = 6; break;
        case Input::Pad::S1R:      idx = 7; break;
        case Input::Pad::K1START:  idx = 8; break;
        case Input::Pad::K1SELECT: idx = 9; break;
        case Input::Pad::K21:      idx = 10; break;
        case Input::Pad::K22:      idx = 11; break;
        case Input::Pad::K23:      idx = 12; break;
        case Input::Pad::K24:      idx = 13; break;
        case Input::Pad::K25:      idx = 14; break;
        case Input::Pad::S2L:      idx = 15; break;
        case Input::Pad::S2R:      idx = 16; break;
        case Input::Pad::K2START:  idx = 17; break;
        case Input::Pad::K2SELECT: idx = 18; break;
        case Input::Pad::S1A:      idx = 19; break;
        case Input::Pad::S2A:      idx = 20; break;
        default: break;
        }
        return idx;
    }
    static int button_143()
    {
        static const std::map<GameModeKeys, int> kv = {
            { 5, 2 },
            { 7, 0 },
            { 9, 1 },
            { 10, 2 },
            { 14, 0 },
        };
        auto k = KeyConfigData.currentMode;
        return kv.find(k) == kv.end() ? 0 : kv.at(k);
    }

    static int button_150() { return keySlot(0); }
    static int button_151() { return keySlot(1); }
    static int button_152() { return keySlot(2); }
    static int button_153() { return keySlot(3); }
    static int button_154() { return keySlot(4); }
    static int button_155() { return keySlot(5); }
    static int button_156() { return keySlot(6); }
    static int button_157() { return keySlot(7); }
    static int button_158() { return keySlot(8); }
    static int button_159() { return keySlot(9); }
};

#define define_has_member(index)                                                            \
class has_button_##index                                                                    \
{                                                                                           \
private:                                                                                    \
    typedef long yes_type;                                                                  \
    typedef char no_type;                                                                   \
    template <typename U> static yes_type test(decltype(&U::button_##index));               \
    template <typename U> static no_type  test(...);                                        \
public:                                                                                     \
    static constexpr bool has_func = sizeof(test<ButtonConverter>()) == sizeof(yes_type);   \
private:                                                                                    \
    template <typename U, typename = std::enable_if_t<!has_func>>                           \
    static constexpr int(*func())() { return &U::button_0; }                                \
    template <typename U, typename = std::enable_if_t<has_func>>                            \
    static constexpr int(*func())() { return &U::button_##index; }                          \
public:                                                                                     \
    static constexpr int(*value)() = func<ButtonConverter>();                               \
}

#define has_button(index)  has_button_##index::has_func
#define button(index) has_button_##index::value

#pragma region define_has_member

define_has_member(0);
define_has_member(1);
define_has_member(2);
define_has_member(3);
define_has_member(4);
define_has_member(5);
define_has_member(6);
define_has_member(7);
define_has_member(8);
define_has_member(9);
define_has_member(10);
define_has_member(11);
define_has_member(12);
define_has_member(13);
define_has_member(14);
define_has_member(15);
define_has_member(16);
define_has_member(17);
define_has_member(18);
define_has_member(19);
define_has_member(20);
define_has_member(21);
define_has_member(22);
define_has_member(23);
define_has_member(24);
define_has_member(25);
define_has_member(26);
define_has_member(27);
define_has_member(28);
define_has_member(29);
define_has_member(30);
define_has_member(31);
define_has_member(32);
define_has_member(33);
define_has_member(34);
define_has_member(35);
define_has_member(36);
define_has_member(37);
define_has_member(38);
define_has_member(39);
define_has_member(40);
define_has_member(41);
define_has_member(42);
define_has_member(43);
define_has_member(44);
define_has_member(45);
define_has_member(46);
define_has_member(47);
define_has_member(48);
define_has_member(49);
define_has_member(50);
define_has_member(51);
define_has_member(52);
define_has_member(53);
define_has_member(54);
define_has_member(55);
define_has_member(56);
define_has_member(57);
define_has_member(58);
define_has_member(59);
define_has_member(60);
define_has_member(61);
define_has_member(62);
define_has_member(63);
define_has_member(64);
define_has_member(65);
define_has_member(66);
define_has_member(67);
define_has_member(68);
define_has_member(69);
define_has_member(70);
define_has_member(71);
define_has_member(72);
define_has_member(73);
define_has_member(74);
define_has_member(75);
define_has_member(76);
define_has_member(77);
define_has_member(78);
define_has_member(79);
define_has_member(80);
define_has_member(81);
define_has_member(82);
define_has_member(83);
define_has_member(84);
define_has_member(85);
define_has_member(86);
define_has_member(87);
define_has_member(88);
define_has_member(89);
define_has_member(90);
define_has_member(91);
define_has_member(92);
define_has_member(93);
define_has_member(94);
define_has_member(95);
define_has_member(96);
define_has_member(97);
define_has_member(98);
define_has_member(99);
define_has_member(100);
define_has_member(101);
define_has_member(102);
define_has_member(103);
define_has_member(104);
define_has_member(105);
define_has_member(106);
define_has_member(107);
define_has_member(108);
define_has_member(109);
define_has_member(110);
define_has_member(111);
define_has_member(112);
define_has_member(113);
define_has_member(114);
define_has_member(115);
define_has_member(116);
define_has_member(117);
define_has_member(118);
define_has_member(119);
define_has_member(120);
define_has_member(121);
define_has_member(122);
define_has_member(123);
define_has_member(124);
define_has_member(125);
define_has_member(126);
define_has_member(127);
define_has_member(128);
define_has_member(129);
define_has_member(130);
define_has_member(131);
define_has_member(132);
define_has_member(133);
define_has_member(134);
define_has_member(135);
define_has_member(136);
define_has_member(137);
define_has_member(138);
define_has_member(139);
define_has_member(140);
define_has_member(141);
define_has_member(142);
define_has_member(143);
define_has_member(144);
define_has_member(145);
define_has_member(146);
define_has_member(147);
define_has_member(148);
define_has_member(149);
define_has_member(150);
define_has_member(151);
define_has_member(152);
define_has_member(153);
define_has_member(154);
define_has_member(155);
define_has_member(156);
define_has_member(157);
define_has_member(158);
define_has_member(159);
define_has_member(160);
define_has_member(161);
define_has_member(162);
define_has_member(163);
define_has_member(164);
define_has_member(165);
define_has_member(166);
define_has_member(167);
define_has_member(168);
define_has_member(169);
define_has_member(170);
define_has_member(171);
define_has_member(172);
define_has_member(173);
define_has_member(174);
define_has_member(175);
define_has_member(176);
define_has_member(177);
define_has_member(178);
define_has_member(179);
define_has_member(180);
define_has_member(181);
define_has_member(182);
define_has_member(183);
define_has_member(184);
define_has_member(185);
define_has_member(186);
define_has_member(187);
define_has_member(188);
define_has_member(189);
define_has_member(190);
define_has_member(191);
define_has_member(192);
define_has_member(193);
define_has_member(194);
define_has_member(195);
define_has_member(196);
define_has_member(197);
define_has_member(198);
define_has_member(199);
define_has_member(200);
define_has_member(201);
define_has_member(202);
define_has_member(203);
define_has_member(204);
define_has_member(205);
define_has_member(206);
define_has_member(207);
define_has_member(208);
define_has_member(209);
define_has_member(210);
define_has_member(211);
define_has_member(212);
define_has_member(213);
define_has_member(214);
define_has_member(215);
define_has_member(216);
define_has_member(217);
define_has_member(218);
define_has_member(219);
define_has_member(220);
define_has_member(221);
define_has_member(222);
define_has_member(223);
define_has_member(224);
define_has_member(225);
define_has_member(226);
define_has_member(227);
define_has_member(228);
define_has_member(229);
define_has_member(230);
define_has_member(231);
define_has_member(232);
define_has_member(233);
define_has_member(234);
define_has_member(235);
define_has_member(236);
define_has_member(237);
define_has_member(238);
define_has_member(239);
define_has_member(240);
define_has_member(241);
define_has_member(242);
define_has_member(243);
define_has_member(244);
define_has_member(245);
define_has_member(246);
define_has_member(247);
define_has_member(248);
define_has_member(249);
define_has_member(250);
define_has_member(251);
define_has_member(252);
define_has_member(253);
define_has_member(254);
define_has_member(255);
define_has_member(256);
define_has_member(257);
define_has_member(258);
define_has_member(259);
define_has_member(260);
define_has_member(261);
define_has_member(262);
define_has_member(263);
define_has_member(264);
define_has_member(265);
define_has_member(266);
define_has_member(267);
define_has_member(268);
define_has_member(269);
define_has_member(270);
define_has_member(271);
define_has_member(272);
define_has_member(273);
define_has_member(274);
define_has_member(275);
define_has_member(276);
define_has_member(277);
define_has_member(278);
define_has_member(279);
define_has_member(280);
define_has_member(281);
define_has_member(282);
define_has_member(283);
define_has_member(284);
define_has_member(285);
define_has_member(286);
define_has_member(287);
define_has_member(288);
define_has_member(289);
define_has_member(290);
define_has_member(291);
define_has_member(292);
define_has_member(293);
define_has_member(294);
define_has_member(295);
define_has_member(296);
define_has_member(297);
define_has_member(298);
define_has_member(299);

#pragma endregion


}

namespace lr2skin
{

std::function<int()> convertButtonIndex(int n)
{
    using namespace lv;

    if (n >= 300)
    {
        // TODO 
    }
    else if (n >= 0 && n <= 299)
    {
        static constexpr int(*kv[300])() =
        {
            button(0),
            button(1),
            button(2),
            button(3),
            button(4),
            button(5),
            button(6),
            button(7),
            button(8),
            button(9),
            button(10),
            button(11),
            button(12),
            button(13),
            button(14),
            button(15),
            button(16),
            button(17),
            button(18),
            button(19),
            button(20),
            button(21),
            button(22),
            button(23),
            button(24),
            button(25),
            button(26),
            button(27),
            button(28),
            button(29),
            button(30),
            button(31),
            button(32),
            button(33),
            button(34),
            button(35),
            button(36),
            button(37),
            button(38),
            button(39),
            button(40),
            button(41),
            button(42),
            button(43),
            button(44),
            button(45),
            button(46),
            button(47),
            button(48),
            button(49),
            button(50),
            button(51),
            button(52),
            button(53),
            button(54),
            button(55),
            button(56),
            button(57),
            button(58),
            button(59),
            button(60),
            button(61),
            button(62),
            button(63),
            button(64),
            button(65),
            button(66),
            button(67),
            button(68),
            button(69),
            button(70),
            button(71),
            button(72),
            button(73),
            button(74),
            button(75),
            button(76),
            button(77),
            button(78),
            button(79),
            button(80),
            button(81),
            button(82),
            button(83),
            button(84),
            button(85),
            button(86),
            button(87),
            button(88),
            button(89),
            button(90),
            button(91),
            button(92),
            button(93),
            button(94),
            button(95),
            button(96),
            button(97),
            button(98),
            button(99),
            button(100),
            button(101),
            button(102),
            button(103),
            button(104),
            button(105),
            button(106),
            button(107),
            button(108),
            button(109),
            button(110),
            button(111),
            button(112),
            button(113),
            button(114),
            button(115),
            button(116),
            button(117),
            button(118),
            button(119),
            button(120),
            button(121),
            button(122),
            button(123),
            button(124),
            button(125),
            button(126),
            button(127),
            button(128),
            button(129),
            button(130),
            button(131),
            button(132),
            button(133),
            button(134),
            button(135),
            button(136),
            button(137),
            button(138),
            button(139),
            button(140),
            button(141),
            button(142),
            button(143),
            button(144),
            button(145),
            button(146),
            button(147),
            button(148),
            button(149),
            button(150),
            button(151),
            button(152),
            button(153),
            button(154),
            button(155),
            button(156),
            button(157),
            button(158),
            button(159),
            button(160),
            button(161),
            button(162),
            button(163),
            button(164),
            button(165),
            button(166),
            button(167),
            button(168),
            button(169),
            button(170),
            button(171),
            button(172),
            button(173),
            button(174),
            button(175),
            button(176),
            button(177),
            button(178),
            button(179),
            button(180),
            button(181),
            button(182),
            button(183),
            button(184),
            button(185),
            button(186),
            button(187),
            button(188),
            button(189),
            button(190),
            button(191),
            button(192),
            button(193),
            button(194),
            button(195),
            button(196),
            button(197),
            button(198),
            button(199),
            button(200),
            button(201),
            button(202),
            button(203),
            button(204),
            button(205),
            button(206),
            button(207),
            button(208),
            button(209),
            button(210),
            button(211),
            button(212),
            button(213),
            button(214),
            button(215),
            button(216),
            button(217),
            button(218),
            button(219),
            button(220),
            button(221),
            button(222),
            button(223),
            button(224),
            button(225),
            button(226),
            button(227),
            button(228),
            button(229),
            button(230),
            button(231),
            button(232),
            button(233),
            button(234),
            button(235),
            button(236),
            button(237),
            button(238),
            button(239),
            button(240),
            button(241),
            button(242),
            button(243),
            button(244),
            button(245),
            button(246),
            button(247),
            button(248),
            button(249),
            button(250),
            button(251),
            button(252),
            button(253),
            button(254),
            button(255),
            button(256),
            button(257),
            button(258),
            button(259),
            button(260),
            button(261),
            button(262),
            button(263),
            button(264),
            button(265),
            button(266),
            button(267),
            button(268),
            button(269),
            button(270),
            button(271),
            button(272),
            button(273),
            button(274),
            button(275),
            button(276),
            button(277),
            button(278),
            button(279),
            button(280),
            button(281),
            button(282),
            button(283),
            button(284),
            button(285),
            button(286),
            button(287),
            button(288),
            button(289),
            button(290),
            button(291),
            button(292),
            button(293),
            button(294),
            button(295),
            button(296),
            button(297),
            button(298),
            button(299),
        };

        return kv[n];
    }

    return [] { return 0; };
}

}
