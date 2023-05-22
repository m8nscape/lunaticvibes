#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

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

    static int getClearType(std::shared_ptr<RulesetBMS> r)
    {
        LampType t = r ? r->getClearType() : LampType::NOPLAY;
        switch (t)
        {

            case LampType::NOPLAY: return 0;
            case LampType::FAILED: return 1;
            case LampType::ASSIST: return 2;
            case LampType::EASY: return 3;
            case LampType::NORMAL: return 4;
            case LampType::HARD: return 5;
            case LampType::EXHARD: return 6;
            case LampType::FULLCOMBO: return 7;
            case LampType::PERFECT: return 8;
            case LampType::MAX: return 9;
        }
        return 0;
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

    static int button_270() { return getClearType(std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset)); }
    static int button_271() { return getClearType(std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset)); }

    static int button_301() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(0))); }
    static int button_302() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(1))); }
    static int button_303() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(2))); }
    static int button_304() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(3))); }
    static int button_305() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(4))); }
    static int button_306() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(5))); }
    static int button_307() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(6))); }
    static int button_308() { return getClearType(std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(7))); }

    static int button_310() { return ArenaData.myRanking; }
    static int button_311() { return ArenaData.playerRanking[0]; }
    static int button_312() { return ArenaData.playerRanking[1]; }
    static int button_313() { return ArenaData.playerRanking[2]; }
    static int button_314() { return ArenaData.playerRanking[3]; }
    static int button_315() { return ArenaData.playerRanking[4]; }
    static int button_316() { return ArenaData.playerRanking[5]; }
    static int button_317() { return ArenaData.playerRanking[6]; }
    static int button_318() { return ArenaData.playerRanking[7]; }
};

#pragma region declare_member

declare_member(ButtonConverter, int, button, 0);
declare_member(ButtonConverter, int, button, 1);
declare_member(ButtonConverter, int, button, 2);
declare_member(ButtonConverter, int, button, 3);
declare_member(ButtonConverter, int, button, 4);
declare_member(ButtonConverter, int, button, 5);
declare_member(ButtonConverter, int, button, 6);
declare_member(ButtonConverter, int, button, 7);
declare_member(ButtonConverter, int, button, 8);
declare_member(ButtonConverter, int, button, 9);
declare_member(ButtonConverter, int, button, 10);
declare_member(ButtonConverter, int, button, 11);
declare_member(ButtonConverter, int, button, 12);
declare_member(ButtonConverter, int, button, 13);
declare_member(ButtonConverter, int, button, 14);
declare_member(ButtonConverter, int, button, 15);
declare_member(ButtonConverter, int, button, 16);
declare_member(ButtonConverter, int, button, 17);
declare_member(ButtonConverter, int, button, 18);
declare_member(ButtonConverter, int, button, 19);
declare_member(ButtonConverter, int, button, 20);
declare_member(ButtonConverter, int, button, 21);
declare_member(ButtonConverter, int, button, 22);
declare_member(ButtonConverter, int, button, 23);
declare_member(ButtonConverter, int, button, 24);
declare_member(ButtonConverter, int, button, 25);
declare_member(ButtonConverter, int, button, 26);
declare_member(ButtonConverter, int, button, 27);
declare_member(ButtonConverter, int, button, 28);
declare_member(ButtonConverter, int, button, 29);
declare_member(ButtonConverter, int, button, 30);
declare_member(ButtonConverter, int, button, 31);
declare_member(ButtonConverter, int, button, 32);
declare_member(ButtonConverter, int, button, 33);
declare_member(ButtonConverter, int, button, 34);
declare_member(ButtonConverter, int, button, 35);
declare_member(ButtonConverter, int, button, 36);
declare_member(ButtonConverter, int, button, 37);
declare_member(ButtonConverter, int, button, 38);
declare_member(ButtonConverter, int, button, 39);
declare_member(ButtonConverter, int, button, 40);
declare_member(ButtonConverter, int, button, 41);
declare_member(ButtonConverter, int, button, 42);
declare_member(ButtonConverter, int, button, 43);
declare_member(ButtonConverter, int, button, 44);
declare_member(ButtonConverter, int, button, 45);
declare_member(ButtonConverter, int, button, 46);
declare_member(ButtonConverter, int, button, 47);
declare_member(ButtonConverter, int, button, 48);
declare_member(ButtonConverter, int, button, 49);
declare_member(ButtonConverter, int, button, 50);
declare_member(ButtonConverter, int, button, 51);
declare_member(ButtonConverter, int, button, 52);
declare_member(ButtonConverter, int, button, 53);
declare_member(ButtonConverter, int, button, 54);
declare_member(ButtonConverter, int, button, 55);
declare_member(ButtonConverter, int, button, 56);
declare_member(ButtonConverter, int, button, 57);
declare_member(ButtonConverter, int, button, 58);
declare_member(ButtonConverter, int, button, 59);
declare_member(ButtonConverter, int, button, 60);
declare_member(ButtonConverter, int, button, 61);
declare_member(ButtonConverter, int, button, 62);
declare_member(ButtonConverter, int, button, 63);
declare_member(ButtonConverter, int, button, 64);
declare_member(ButtonConverter, int, button, 65);
declare_member(ButtonConverter, int, button, 66);
declare_member(ButtonConverter, int, button, 67);
declare_member(ButtonConverter, int, button, 68);
declare_member(ButtonConverter, int, button, 69);
declare_member(ButtonConverter, int, button, 70);
declare_member(ButtonConverter, int, button, 71);
declare_member(ButtonConverter, int, button, 72);
declare_member(ButtonConverter, int, button, 73);
declare_member(ButtonConverter, int, button, 74);
declare_member(ButtonConverter, int, button, 75);
declare_member(ButtonConverter, int, button, 76);
declare_member(ButtonConverter, int, button, 77);
declare_member(ButtonConverter, int, button, 78);
declare_member(ButtonConverter, int, button, 79);
declare_member(ButtonConverter, int, button, 80);
declare_member(ButtonConverter, int, button, 81);
declare_member(ButtonConverter, int, button, 82);
declare_member(ButtonConverter, int, button, 83);
declare_member(ButtonConverter, int, button, 84);
declare_member(ButtonConverter, int, button, 85);
declare_member(ButtonConverter, int, button, 86);
declare_member(ButtonConverter, int, button, 87);
declare_member(ButtonConverter, int, button, 88);
declare_member(ButtonConverter, int, button, 89);
declare_member(ButtonConverter, int, button, 90);
declare_member(ButtonConverter, int, button, 91);
declare_member(ButtonConverter, int, button, 92);
declare_member(ButtonConverter, int, button, 93);
declare_member(ButtonConverter, int, button, 94);
declare_member(ButtonConverter, int, button, 95);
declare_member(ButtonConverter, int, button, 96);
declare_member(ButtonConverter, int, button, 97);
declare_member(ButtonConverter, int, button, 98);
declare_member(ButtonConverter, int, button, 99);
declare_member(ButtonConverter, int, button, 100);
declare_member(ButtonConverter, int, button, 101);
declare_member(ButtonConverter, int, button, 102);
declare_member(ButtonConverter, int, button, 103);
declare_member(ButtonConverter, int, button, 104);
declare_member(ButtonConverter, int, button, 105);
declare_member(ButtonConverter, int, button, 106);
declare_member(ButtonConverter, int, button, 107);
declare_member(ButtonConverter, int, button, 108);
declare_member(ButtonConverter, int, button, 109);
declare_member(ButtonConverter, int, button, 110);
declare_member(ButtonConverter, int, button, 111);
declare_member(ButtonConverter, int, button, 112);
declare_member(ButtonConverter, int, button, 113);
declare_member(ButtonConverter, int, button, 114);
declare_member(ButtonConverter, int, button, 115);
declare_member(ButtonConverter, int, button, 116);
declare_member(ButtonConverter, int, button, 117);
declare_member(ButtonConverter, int, button, 118);
declare_member(ButtonConverter, int, button, 119);
declare_member(ButtonConverter, int, button, 120);
declare_member(ButtonConverter, int, button, 121);
declare_member(ButtonConverter, int, button, 122);
declare_member(ButtonConverter, int, button, 123);
declare_member(ButtonConverter, int, button, 124);
declare_member(ButtonConverter, int, button, 125);
declare_member(ButtonConverter, int, button, 126);
declare_member(ButtonConverter, int, button, 127);
declare_member(ButtonConverter, int, button, 128);
declare_member(ButtonConverter, int, button, 129);
declare_member(ButtonConverter, int, button, 130);
declare_member(ButtonConverter, int, button, 131);
declare_member(ButtonConverter, int, button, 132);
declare_member(ButtonConverter, int, button, 133);
declare_member(ButtonConverter, int, button, 134);
declare_member(ButtonConverter, int, button, 135);
declare_member(ButtonConverter, int, button, 136);
declare_member(ButtonConverter, int, button, 137);
declare_member(ButtonConverter, int, button, 138);
declare_member(ButtonConverter, int, button, 139);
declare_member(ButtonConverter, int, button, 140);
declare_member(ButtonConverter, int, button, 141);
declare_member(ButtonConverter, int, button, 142);
declare_member(ButtonConverter, int, button, 143);
declare_member(ButtonConverter, int, button, 144);
declare_member(ButtonConverter, int, button, 145);
declare_member(ButtonConverter, int, button, 146);
declare_member(ButtonConverter, int, button, 147);
declare_member(ButtonConverter, int, button, 148);
declare_member(ButtonConverter, int, button, 149);
declare_member(ButtonConverter, int, button, 150);
declare_member(ButtonConverter, int, button, 151);
declare_member(ButtonConverter, int, button, 152);
declare_member(ButtonConverter, int, button, 153);
declare_member(ButtonConverter, int, button, 154);
declare_member(ButtonConverter, int, button, 155);
declare_member(ButtonConverter, int, button, 156);
declare_member(ButtonConverter, int, button, 157);
declare_member(ButtonConverter, int, button, 158);
declare_member(ButtonConverter, int, button, 159);
declare_member(ButtonConverter, int, button, 160);
declare_member(ButtonConverter, int, button, 161);
declare_member(ButtonConverter, int, button, 162);
declare_member(ButtonConverter, int, button, 163);
declare_member(ButtonConverter, int, button, 164);
declare_member(ButtonConverter, int, button, 165);
declare_member(ButtonConverter, int, button, 166);
declare_member(ButtonConverter, int, button, 167);
declare_member(ButtonConverter, int, button, 168);
declare_member(ButtonConverter, int, button, 169);
declare_member(ButtonConverter, int, button, 170);
declare_member(ButtonConverter, int, button, 171);
declare_member(ButtonConverter, int, button, 172);
declare_member(ButtonConverter, int, button, 173);
declare_member(ButtonConverter, int, button, 174);
declare_member(ButtonConverter, int, button, 175);
declare_member(ButtonConverter, int, button, 176);
declare_member(ButtonConverter, int, button, 177);
declare_member(ButtonConverter, int, button, 178);
declare_member(ButtonConverter, int, button, 179);
declare_member(ButtonConverter, int, button, 180);
declare_member(ButtonConverter, int, button, 181);
declare_member(ButtonConverter, int, button, 182);
declare_member(ButtonConverter, int, button, 183);
declare_member(ButtonConverter, int, button, 184);
declare_member(ButtonConverter, int, button, 185);
declare_member(ButtonConverter, int, button, 186);
declare_member(ButtonConverter, int, button, 187);
declare_member(ButtonConverter, int, button, 188);
declare_member(ButtonConverter, int, button, 189);
declare_member(ButtonConverter, int, button, 190);
declare_member(ButtonConverter, int, button, 191);
declare_member(ButtonConverter, int, button, 192);
declare_member(ButtonConverter, int, button, 193);
declare_member(ButtonConverter, int, button, 194);
declare_member(ButtonConverter, int, button, 195);
declare_member(ButtonConverter, int, button, 196);
declare_member(ButtonConverter, int, button, 197);
declare_member(ButtonConverter, int, button, 198);
declare_member(ButtonConverter, int, button, 199);
declare_member(ButtonConverter, int, button, 200);
declare_member(ButtonConverter, int, button, 201);
declare_member(ButtonConverter, int, button, 202);
declare_member(ButtonConverter, int, button, 203);
declare_member(ButtonConverter, int, button, 204);
declare_member(ButtonConverter, int, button, 205);
declare_member(ButtonConverter, int, button, 206);
declare_member(ButtonConverter, int, button, 207);
declare_member(ButtonConverter, int, button, 208);
declare_member(ButtonConverter, int, button, 209);
declare_member(ButtonConverter, int, button, 210);
declare_member(ButtonConverter, int, button, 211);
declare_member(ButtonConverter, int, button, 212);
declare_member(ButtonConverter, int, button, 213);
declare_member(ButtonConverter, int, button, 214);
declare_member(ButtonConverter, int, button, 215);
declare_member(ButtonConverter, int, button, 216);
declare_member(ButtonConverter, int, button, 217);
declare_member(ButtonConverter, int, button, 218);
declare_member(ButtonConverter, int, button, 219);
declare_member(ButtonConverter, int, button, 220);
declare_member(ButtonConverter, int, button, 221);
declare_member(ButtonConverter, int, button, 222);
declare_member(ButtonConverter, int, button, 223);
declare_member(ButtonConverter, int, button, 224);
declare_member(ButtonConverter, int, button, 225);
declare_member(ButtonConverter, int, button, 226);
declare_member(ButtonConverter, int, button, 227);
declare_member(ButtonConverter, int, button, 228);
declare_member(ButtonConverter, int, button, 229);
declare_member(ButtonConverter, int, button, 230);
declare_member(ButtonConverter, int, button, 231);
declare_member(ButtonConverter, int, button, 232);
declare_member(ButtonConverter, int, button, 233);
declare_member(ButtonConverter, int, button, 234);
declare_member(ButtonConverter, int, button, 235);
declare_member(ButtonConverter, int, button, 236);
declare_member(ButtonConverter, int, button, 237);
declare_member(ButtonConverter, int, button, 238);
declare_member(ButtonConverter, int, button, 239);
declare_member(ButtonConverter, int, button, 240);
declare_member(ButtonConverter, int, button, 241);
declare_member(ButtonConverter, int, button, 242);
declare_member(ButtonConverter, int, button, 243);
declare_member(ButtonConverter, int, button, 244);
declare_member(ButtonConverter, int, button, 245);
declare_member(ButtonConverter, int, button, 246);
declare_member(ButtonConverter, int, button, 247);
declare_member(ButtonConverter, int, button, 248);
declare_member(ButtonConverter, int, button, 249);
declare_member(ButtonConverter, int, button, 250);
declare_member(ButtonConverter, int, button, 251);
declare_member(ButtonConverter, int, button, 252);
declare_member(ButtonConverter, int, button, 253);
declare_member(ButtonConverter, int, button, 254);
declare_member(ButtonConverter, int, button, 255);
declare_member(ButtonConverter, int, button, 256);
declare_member(ButtonConverter, int, button, 257);
declare_member(ButtonConverter, int, button, 258);
declare_member(ButtonConverter, int, button, 259);
declare_member(ButtonConverter, int, button, 260);
declare_member(ButtonConverter, int, button, 261);
declare_member(ButtonConverter, int, button, 262);
declare_member(ButtonConverter, int, button, 263);
declare_member(ButtonConverter, int, button, 264);
declare_member(ButtonConverter, int, button, 265);
declare_member(ButtonConverter, int, button, 266);
declare_member(ButtonConverter, int, button, 267);
declare_member(ButtonConverter, int, button, 268);
declare_member(ButtonConverter, int, button, 269);
declare_member(ButtonConverter, int, button, 270);
declare_member(ButtonConverter, int, button, 271);
declare_member(ButtonConverter, int, button, 272);
declare_member(ButtonConverter, int, button, 273);
declare_member(ButtonConverter, int, button, 274);
declare_member(ButtonConverter, int, button, 275);
declare_member(ButtonConverter, int, button, 276);
declare_member(ButtonConverter, int, button, 277);
declare_member(ButtonConverter, int, button, 278);
declare_member(ButtonConverter, int, button, 279);
declare_member(ButtonConverter, int, button, 280);
declare_member(ButtonConverter, int, button, 281);
declare_member(ButtonConverter, int, button, 282);
declare_member(ButtonConverter, int, button, 283);
declare_member(ButtonConverter, int, button, 284);
declare_member(ButtonConverter, int, button, 285);
declare_member(ButtonConverter, int, button, 286);
declare_member(ButtonConverter, int, button, 287);
declare_member(ButtonConverter, int, button, 288);
declare_member(ButtonConverter, int, button, 289);
declare_member(ButtonConverter, int, button, 290);
declare_member(ButtonConverter, int, button, 291);
declare_member(ButtonConverter, int, button, 292);
declare_member(ButtonConverter, int, button, 293);
declare_member(ButtonConverter, int, button, 294);
declare_member(ButtonConverter, int, button, 295);
declare_member(ButtonConverter, int, button, 296);
declare_member(ButtonConverter, int, button, 297);
declare_member(ButtonConverter, int, button, 298);
declare_member(ButtonConverter, int, button, 299);
declare_member(ButtonConverter, int, button, 300);
declare_member(ButtonConverter, int, button, 301);
declare_member(ButtonConverter, int, button, 302);
declare_member(ButtonConverter, int, button, 303);
declare_member(ButtonConverter, int, button, 304);
declare_member(ButtonConverter, int, button, 305);
declare_member(ButtonConverter, int, button, 306);
declare_member(ButtonConverter, int, button, 307);
declare_member(ButtonConverter, int, button, 308);
declare_member(ButtonConverter, int, button, 309);
declare_member(ButtonConverter, int, button, 310);
declare_member(ButtonConverter, int, button, 311);
declare_member(ButtonConverter, int, button, 312);
declare_member(ButtonConverter, int, button, 313);
declare_member(ButtonConverter, int, button, 314);
declare_member(ButtonConverter, int, button, 315);
declare_member(ButtonConverter, int, button, 316);
declare_member(ButtonConverter, int, button, 317);
declare_member(ButtonConverter, int, button, 318);
declare_member(ButtonConverter, int, button, 319);

#define button(index) member(ButtonConverter, int, button, index)

#pragma endregion


namespace lr2skin
{

std::function<int()> convertButtonIndex(int n)
{
    if (n >= 0 && n <= 319)
    {
        static constexpr int(*kv[320])() =
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
            button(300),
            button(301),
            button(302),
            button(303),
            button(304),
            button(305),
            button(306),
            button(307),
            button(308),
            button(309),
            button(310),
            button(311),
            button(312),
            button(313),
            button(314),
            button(315),
            button(316),
            button(317),
            button(318),
            button(319),
        };

        return kv[n];
    }

    return [] { return 0; };
}

}
}
