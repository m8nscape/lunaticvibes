#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/data/data_types.h"
#include "game/runtime/i18n.h"

namespace lunaticvibes
{

class TextConverter
{
private:
    static std::shared_ptr<EntryBase> getCurrentSelectedEntry()
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        return SelectData.entries[SelectData.selectedEntryIndex].first;
    }

    static std::shared_ptr<ChartFormatBase> getCurrentSelectedChart()
    {
        auto p = getCurrentSelectedEntry();
        if (p == nullptr)
            return nullptr;
        switch (p->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
            return std::dynamic_pointer_cast<EntryFolderSong>(p)->getCurrentChart();
        case eEntryType::CHART:
        case eEntryType::RIVAL_CHART:
            return std::dynamic_pointer_cast<EntryChart>(p)->getChart();
        }
        return nullptr;
    }

    static std::string_view playModRandom(PlayModifierRandomType t)
    {
        switch (t)
        {
        case PlayModifierRandomType::NONE: return "NORMAL";
        case PlayModifierRandomType::MIRROR: return "MIRROR";
        case PlayModifierRandomType::RANDOM: return "RANDOM";
        case PlayModifierRandomType::SRAN: return "S-RANDOM";
        case PlayModifierRandomType::HRAN: return "H-RANDOM";
        case PlayModifierRandomType::ALLSCR: return "ALL-SCR";
        case PlayModifierRandomType::RRAN: return "R-RANDOM";
        case PlayModifierRandomType::DB_SYNCHRONIZE: return "SYNCHRONIZE";
        case PlayModifierRandomType::DB_SYMMETRY: return "SYMMETRY";
        }
        return "";
    }

    static std::string_view playModGauge(PlayModifierGaugeType t)
    {
        switch (t)
        {
        case PlayModifierGaugeType::NORMAL: return "NORMAL";
        case PlayModifierGaugeType::HARD: return "HARD";
        case PlayModifierGaugeType::DEATH: return "DEATH";
        case PlayModifierGaugeType::EASY: return "EASY";
        case PlayModifierGaugeType::EXHARD: return "EX-HARD";
        case PlayModifierGaugeType::ASSISTEASY: return "ASSIST-EASY";
        }
        return "";
    }

    static std::string_view playModAssist(uint8_t m)
    {
        switch (m)
        {
        case 0: return "NONE";
        case PLAY_MOD_ASSIST_AUTOSCR: return "AUTO-SCR";
        }
        return "";
    }

    static std::string_view playModLaneEffect(PlayModifierLaneEffectType t)
    {
        switch (t)
        {
            case PlayModifierLaneEffectType::OFF: return "OFF";
            case PlayModifierLaneEffectType::HIDDEN: return "HIDDEN+";
            case PlayModifierLaneEffectType::SUDDEN: return "SUDDEN+";
            case PlayModifierLaneEffectType::SUDHID: return "SUD+&HID+";
            case PlayModifierLaneEffectType::LIFT: return "LIFT";
            case PlayModifierLaneEffectType::LIFTSUD: return "LIFT&SUD+";
        }
        return "";
    }

    //static std::shared_ptr<Struct_LR2Customize::Option> getCustomizeOption(int i)
    //{
    //    const auto& d = LR2CustomizeData;
    //    if (i >= d.optionsKeyList.size())
    //        return nullptr;

    //    auto& key = d.optionsKeyList[d.topOptionIndex];
    //    return d.optionsMap.at(key);
    //}
    //static std::string_view getCustomizeOptionName(int i)
    //{
    //    auto p = getCustomizeOption(LR2CustomizeData.topOptionIndex + i);
    //    return p ? p->displayName : "";
    //}
    //static std::string_view getCustomizeOptionEntry(int i)
    //{
    //    auto p = getCustomizeOption(LR2CustomizeData.topOptionIndex + i);
    //    return p ? p->entries[p->selectedEntry] : "";
    //}

    static std::string_view getCourseStageTitle(int i)
    {
        if (i < 0 || PlayData.courseStageData.size() <= i) return "";
        return PlayData.courseStageData[i].title;
    }
    static std::string_view getCourseStageSubTitle(int i)
    {
        if (i < 0 || PlayData.courseStageData.size() <= i) return "";
        return PlayData.courseStageData[i].subTitle;
    }

private:

    inline static std::string targetName;
    inline static std::string fullTitle;
    inline static std::string levelEstimated;

public:
    static std::string_view text_0() { return ""; }

    static std::string_view text_1()
    {
        if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
        {
            return "Player";
        }
        else
        {
            switch (PlayData.targetType)
            {
            case TargetType::Zero: return "RATE 0%";
            case TargetType::RankAAA: return "RANK AAA";
            case TargetType::RankAA: return "RANK AA";
            case TargetType::RankA: return "RANK A";
            case TargetType::UseTargetRate:
            {
                switch (PlayData.targetRate)
                {
                case 22:  return "RANK E";
                case 33:  return "RANK D";
                case 44:  return "RANK C";
                case 55:  return "RANK B";
                case 66:  return "RANK A";
                case 77:  return "RANK AA";
                case 88:  return "RANK AAA";
                case 100: return "DJ AUTO";
                }
                targetName = "RATE "s + std::to_string(PlayData.targetRate) + "%";
                return targetName;
            }
            }
        }
        return "";
    }
    static std::string_view text_2() { return SystemData.playerName; };

    std::string_view text_10()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            return std::string_view(chart ? chart->title : p->_name);
        }
        return "";
    }
    std::string_view text_11()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            return std::string_view(chart ? chart->title2 : p->_name2);
        }
        return "";
    }
    std::string_view text_12()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            auto title = chart ? chart->title : p->_name;
            auto title2 = chart ? chart->title : p->_name;
            fullTitle = title + (!title.empty() ? " " : "") + title2;
            return fullTitle;
        }
        return "";
    }
    std::string_view text_13()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            return chart ? chart->genre : "";
        }
        return "";
    }
    std::string_view text_14()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            return chart ? chart->artist : "";
        }
        return "";
    }
    std::string_view text_15()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            return chart ? chart->artist2 : "";
        }
        return "";
    }
    // 16 TAG
    std::string_view text_17()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            levelEstimated = std::to_string(chart ? (int)chart->levelEstimated : 0);
            return levelEstimated;
        }
        return "";
    }
    std::string_view text_18()
    {
        auto p = getCurrentSelectedEntry();
        if (p)
        {
            auto chart = getCurrentSelectedChart();
            return chart ? chart->version : "";
        }
        return "";
    }

    /*
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
    */

    static std::string_view text_30() { return SelectData.jukeboxName; }

    static std::string_view text_40() { return KeyConfigData.bindName[KeyConfigData.selecting.first]; };
    static std::string_view text_41() { return "-"; };
    static std::string_view text_42() { return "-"; };
    static std::string_view text_43() { return "-"; };
    static std::string_view text_44() { return "-"; };
    static std::string_view text_45() { return "-"; };
    static std::string_view text_46() { return "-"; };
    static std::string_view text_47() { return "-"; };
    static std::string_view text_48() { return "-"; };
    static std::string_view text_49() { return "-"; };

    //static std::string_view text_50() { return LR2CustomizeData.currentSkinName; }
    //static std::string_view text_51() { return LR2CustomizeData.currentSkinMakerName; }

    static std::string_view text_60()
    {
        switch (SelectData.filterKeys)
        {
        case FilterKeysType::All: return "ALL";
        case FilterKeysType::Single: return "SINGLE";
        case FilterKeysType::_7: return "7K";
        case FilterKeysType::_5: return "5K";
        case FilterKeysType::Double: return "DOUBLE";
        case FilterKeysType::_14: return "14K";
        case FilterKeysType::_10: return "10K";
        case FilterKeysType::_9: return "9K";
        }
        return "";
    }

    static std::string_view text_61()
    {
        switch (SelectData.sortType)
        {
        case SongListSortType::DEFAULT: return "DIRECTORY";
        case SongListSortType::TITLE: return "TITLE";
        case SongListSortType::LEVEL: return "LEVEL";
        case SongListSortType::CLEAR: return "CLEAR";
        case SongListSortType::RATE: return "RANK";
        }
        return "";
    }

    static std::string_view text_62()
    {
        switch (SelectData.filterDifficulty)
        {
        case FilterDifficultyType::All: return "ALL";
        case FilterDifficultyType::B: return "BEGINNER";
        case FilterDifficultyType::N: return "NORMAL";
        case FilterDifficultyType::H: return "HYPER";
        case FilterDifficultyType::A: return "ANOTHER";
        case FilterDifficultyType::I: return "INSANE";
        }
        return "";
    }

    static std::string_view text_63()
    {
        return playModRandom(PlayData.player[PLAYER_SLOT_PLAYER].mods.randomLeft);
    }
    static std::string_view text_64()
    {
        PlayModifierRandomType ran = PlayModifierRandomType::NONE;
        if (PlayData.battleType != PlayModifierBattleType::LocalBattle)
        {
            ran = PlayData.player[PLAYER_SLOT_PLAYER].mods.randomRight;
        }
        else
        {
            ran = PlayData.player[PLAYER_SLOT_TARGET].mods.randomLeft;
        }
        return playModRandom(ran);
    }

    static std::string_view text_65()
    {
        return playModGauge(PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge);
    }
    static std::string_view text_66()
    {
        return playModGauge(PlayData.player[PLAYER_SLOT_TARGET].mods.gauge);
    }

    static std::string_view text_67()
    {
        return playModAssist(PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask);
    }
    static std::string_view text_68()
    {
        return playModAssist(PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask);
    }

    static std::string_view text_69()
    {
        switch (PlayData.battleType)
        {
        case PlayModifierBattleType::Off: return "OFF";
        case PlayModifierBattleType::LocalBattle: return "BATTLE";
        case PlayModifierBattleType::DoubleBattle: return "DOUBLE";
        case PlayModifierBattleType::GhostBattle: return "GHOST";
        }
        return "";
    }

    static std::string_view text_70()
    {
        if (PlayData.mode != SkinType::PLAY10 && PlayData.mode != SkinType::PLAY14)
            return "OFF";

        return PlayData.player[PLAYER_SLOT_PLAYER].mods.DPFlip ? "ON" : "OFF";
    }

    static std::string_view text_71()
    {
        switch (PlayData.panelStyle & PANEL_STYLE_GRAPH_MASK)
        {
        case PANEL_STYLE_GRAPH_OFF: return "OFF";
        default: return "ON";
        }
    }

    static std::string_view text_72()
    {
        switch (PlayData.ghostType)
        {
        case GhostScorePosition::Off: return "OFF";
        case GhostScorePosition::AboveJudge: return "TYPE A";
        case GhostScorePosition::NearJudge: return "TYPE B";
        case GhostScorePosition::NearJudgeLower: return "TYPE C";
        }
        return "";
    }


    static std::string_view text_73()
    {
        auto t = PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect;
        return (t == PlayModifierLaneEffectType::OFF || t == PlayModifierLaneEffectType::LIFT) ? "OFF" : "ON";
    }

    static std::string_view text_74()
    {
        switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix)
        {
        case PlayModifierHispeedFixType::NONE: return "OFF";
        case PlayModifierHispeedFixType::MINBPM: return "MIN FIX";
        case PlayModifierHispeedFixType::MAXBPM: return "MAX FIX";
        case PlayModifierHispeedFixType::AVERAGE: return "AVERAGE";
        case PlayModifierHispeedFixType::CONSTANT: return "CONSTANT";
        case PlayModifierHispeedFixType::INITIAL: return "START FIX";
        case PlayModifierHispeedFixType::MAIN: return "MAIN FIX";
        }
        return "";
    }

    static std::string_view text_75()
    {
        switch (PlayData.panelStyle & PANEL_STYLE_BGA_MASK)
        {
        case PANEL_STYLE_BGA_OFF:
        case PANEL_STYLE_BGA_SIDE: return "NORMAL";
        case PANEL_STYLE_BGA_SIDE_EXPAND: 
        case PANEL_STYLE_BGA_FULLSCREEN: return "EXTEND";
        }
        return "";
    }

    static std::string_view text_76()
    {
        return (PlayData.panelStyle & PANEL_STYLE_BGA_MASK) == PANEL_STYLE_BGA_OFF ? "OFF" : "ON";
    }

    static std::string_view text_77()
    {
        return "32 BITS";
    }

    static std::string_view text_78()
    {
        switch (SystemData.vsyncMode)
        {
        case GameVsyncMode::OFF: return "OFF";
        case GameVsyncMode::ON: return "ON";
        case GameVsyncMode::ADAPTIVE: return "ADAPTIVE";
        }
        return "";
    }

    static std::string_view text_79()
    {
        switch (SystemData.windowMode)
        {
        case GameWindowMode::FULLSCREEN: return "FULL";
        case GameWindowMode::BORDERLESS: return "BORDERLESS";
        case GameWindowMode::WINDOWED: return "WINDOW";
        }
        return "UNKNOWN";
    }

    static std::string_view text_80()
    {
        // judge auto adjust
        return "NOT SUPPORTED";
    }

    static std::string_view text_81()
    {
        return "EXSCORE";
    }

    static std::string_view text_82()
    {
        return PROJECT_NAME " " PROJECT_VERSION;
    }

    static std::string_view text_83()
    {
        return "Thank you for playing!";
    }

    static std::string_view text_84()
    {
        return playModLaneEffect(PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect);
    }
    static std::string_view text_85()
    {
        return playModLaneEffect(PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect);
    }

    //static std::string_view text_100() { return getCustomizeOptionName(0); }
    //static std::string_view text_101() { return getCustomizeOptionName(1); }
    //static std::string_view text_102() { return getCustomizeOptionName(2); }
    //static std::string_view text_103() { return getCustomizeOptionName(3); }
    //static std::string_view text_104() { return getCustomizeOptionName(4); }
    //static std::string_view text_105() { return getCustomizeOptionName(5); }
    //static std::string_view text_106() { return getCustomizeOptionName(6); }
    //static std::string_view text_107() { return getCustomizeOptionName(7); }
    //static std::string_view text_108() { return getCustomizeOptionName(8); }
    //static std::string_view text_109() { return getCustomizeOptionName(9); }

    //static std::string_view text_110() { return getCustomizeOptionEntry(0); }
    //static std::string_view text_111() { return getCustomizeOptionEntry(1); }
    //static std::string_view text_112() { return getCustomizeOptionEntry(2); }
    //static std::string_view text_113() { return getCustomizeOptionEntry(3); }
    //static std::string_view text_114() { return getCustomizeOptionEntry(4); }
    //static std::string_view text_115() { return getCustomizeOptionEntry(5); }
    //static std::string_view text_116() { return getCustomizeOptionEntry(6); }
    //static std::string_view text_117() { return getCustomizeOptionEntry(7); }
    //static std::string_view text_118() { return getCustomizeOptionEntry(8); }
    //static std::string_view text_119() { return getCustomizeOptionEntry(9); }

    static std::string_view text_150() { return getCourseStageTitle(0); }
    static std::string_view text_151() { return getCourseStageTitle(1); }
    static std::string_view text_152() { return getCourseStageTitle(2); }
    static std::string_view text_153() { return getCourseStageTitle(3); }
    static std::string_view text_154() { return getCourseStageTitle(4); }
    static std::string_view text_155() { return getCourseStageTitle(5); }
    static std::string_view text_156() { return getCourseStageTitle(6); }
    static std::string_view text_157() { return getCourseStageTitle(7); }
    static std::string_view text_158() { return getCourseStageTitle(8); }
    static std::string_view text_159() { return getCourseStageTitle(9); }

    static std::string_view text_160() { return getCourseStageSubTitle(0); }
    static std::string_view text_161() { return getCourseStageSubTitle(1); }
    static std::string_view text_162() { return getCourseStageSubTitle(2); }
    static std::string_view text_163() { return getCourseStageSubTitle(3); }
    static std::string_view text_164() { return getCourseStageSubTitle(4); }
    static std::string_view text_165() { return getCourseStageSubTitle(5); }
    static std::string_view text_166() { return getCourseStageSubTitle(6); }
    static std::string_view text_167() { return getCourseStageSubTitle(7); }
    static std::string_view text_168() { return getCourseStageSubTitle(8); }
    static std::string_view text_169() { return getCourseStageSubTitle(9); }

    static std::string_view text_260() { return ArenaData.isOnline() ? "ARENA LOBBY" : ""; };
    static std::string_view text_261() { return ArenaData.getPlayerName(0); };
    static std::string_view text_262() { return ArenaData.getPlayerName(1); };
    static std::string_view text_263() { return ArenaData.getPlayerName(2); };
    static std::string_view text_264() { return ArenaData.getPlayerName(3); };
    static std::string_view text_265() { return ArenaData.getPlayerName(4); };
    static std::string_view text_266() { return ArenaData.getPlayerName(5); };
    static std::string_view text_267() { return ArenaData.getPlayerName(6); };
    static std::string_view text_268() { return ArenaData.getPlayerName(7); };
    static std::string_view text_270() { auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset); return r ? r->getModifierText() : ""; };
    static std::string_view text_271() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(0)); return r ? r->getModifierText() : ""; };
    static std::string_view text_272() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(1)); return r ? r->getModifierText() : ""; };
    static std::string_view text_273() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(2)); return r ? r->getModifierText() : ""; };
    static std::string_view text_274() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(3)); return r ? r->getModifierText() : ""; };
    static std::string_view text_275() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(4)); return r ? r->getModifierText() : ""; };
    static std::string_view text_276() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(5)); return r ? r->getModifierText() : ""; };
    static std::string_view text_277() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(6)); return r ? r->getModifierText() : ""; };
    static std::string_view text_278() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(7)); return r ? r->getModifierText() : ""; };
    static std::string_view text_280() { auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_281() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(0)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_282() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(1)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_283() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(2)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_284() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(3)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_285() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(4)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_286() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(5)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_287() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(6)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_288() { auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(7)); return r ? r->getModifierTextShort() : ""; };
    static std::string_view text_300() { return i18n::c(i18nText::KEYCONFIG_HINT_KEY); };
    static std::string_view text_301() { return i18n::c(i18nText::KEYCONFIG_HINT_BIND); };
    static std::string_view text_302() { return i18n::c(i18nText::KEYCONFIG_HINT_DEADZONE); };
    static std::string_view text_303() { return i18n::c(i18nText::KEYCONFIG_HINT_F1); };
    static std::string_view text_304() { return i18n::c(i18nText::KEYCONFIG_HINT_F2); };
    static std::string_view text_305() { return i18n::c(i18nText::KEYCONFIG_HINT_DEL); };
    static std::string_view text_306() { return i18n::c(i18nText::KEYCONFIG_HINT_SCRATCH_ABS); };

    static std::string_view text_311() { return "1P 1"; };
    static std::string_view text_312() { return "1P 2"; };
    static std::string_view text_313() { return "1P 3"; };
    static std::string_view text_314() { return "1P 4"; };
    static std::string_view text_315() { return "1P 5"; };
    static std::string_view text_316() { return "1P 6"; };
    static std::string_view text_317() { return "1P 7"; };
    static std::string_view text_318() { return "1P 8"; };
    static std::string_view text_319() { return "1P 9"; };
    static std::string_view text_320() { return "1P SC-L"; };
    static std::string_view text_321() { return "1P SC-R"; };
    static std::string_view text_322() { return "1P START"; };
    static std::string_view text_323() { return "1P SELECT"; };
    static std::string_view text_324() { return "1P SC ABS"; };
    static std::string_view text_331() { return "2P 1"; };
    static std::string_view text_332() { return "2P 2"; };
    static std::string_view text_333() { return "2P 3"; };
    static std::string_view text_334() { return "2P 4"; };
    static std::string_view text_335() { return "2P 5"; };
    static std::string_view text_336() { return "2P 6"; };
    static std::string_view text_337() { return "2P 7"; };
    static std::string_view text_338() { return "2P 8"; };
    static std::string_view text_339() { return "2P 9"; };
    static std::string_view text_340() { return "2P SC-L"; };
    static std::string_view text_341() { return "2P SC-R"; };
    static std::string_view text_342() { return "2P START"; };
    static std::string_view text_343() { return "2P SELECT"; };
    static std::string_view text_344() { return "2P SC ABS"; };

    static std::string_view text_351()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K11];
        }
        return "";
    }
    static std::string_view text_352()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K12];
        }
        return "";
    }
    static std::string_view text_353()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K13];
        }
        return "";
    }
    static std::string_view text_354()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K14];
        }
        return "";
    }
    static std::string_view text_355()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K15];
        }
        return "";
    }
    static std::string_view text_356()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K16];
        }
        return "";
    }
    static std::string_view text_357()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K17];
        }
        return "";
    }
    static std::string_view text_358()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K18];
        }
        return "";
    }
    static std::string_view text_359()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K19];
        }
        return "";
    }
    static std::string_view text_362()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K1START];
        }
        return "";
    }
    static std::string_view text_363()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K1SELECT];
        }
        return "";
    }
    static std::string_view text_360()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::S1L];
        }
        return "";
    }
    static std::string_view text_361()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::S1R];
        }
        return "";
    }
    static std::string_view text_364()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.scratchAxisValueText[0];
        }
        return "";
    }
    static std::string_view text_371()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K21];
        }
        return "";
    }
    static std::string_view text_372()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K22];
        }
        return "";
    }
    static std::string_view text_373()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K23];
        }
        return "";
    }
    static std::string_view text_374()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K24];
        }
        return "";
    }
    static std::string_view text_375()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K25];
        }
        return "";
    }
    static std::string_view text_376()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K26];
        }
        return "";
    }
    static std::string_view text_377()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K27];
        }
        return "";
    }
    static std::string_view text_378()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K28];
        }
        return "";
    }
    static std::string_view text_379()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K29];
        }
        return "";
    }
    static std::string_view text_382()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K2START];
        }
        return "";
    }
    static std::string_view text_383()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::K2SELECT];
        }
        return "";
    }
    static std::string_view text_380()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::S2L];
        }
        return "";
    }
    static std::string_view text_381()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.bindName[k::S2R];
        }
        return "";
    }
    static std::string_view text_384()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.scratchAxisValueText[1];
        }
        return "";
    }
};


#pragma region declare_member

declare_member(TextConverter, std::string_view, text, 0);
declare_member(TextConverter, std::string_view, text, 1);
declare_member(TextConverter, std::string_view, text, 2);
declare_member(TextConverter, std::string_view, text, 3);
declare_member(TextConverter, std::string_view, text, 4);
declare_member(TextConverter, std::string_view, text, 5);
declare_member(TextConverter, std::string_view, text, 6);
declare_member(TextConverter, std::string_view, text, 7);
declare_member(TextConverter, std::string_view, text, 8);
declare_member(TextConverter, std::string_view, text, 9);
declare_member(TextConverter, std::string_view, text, 10);
declare_member(TextConverter, std::string_view, text, 11);
declare_member(TextConverter, std::string_view, text, 12);
declare_member(TextConverter, std::string_view, text, 13);
declare_member(TextConverter, std::string_view, text, 14);
declare_member(TextConverter, std::string_view, text, 15);
declare_member(TextConverter, std::string_view, text, 16);
declare_member(TextConverter, std::string_view, text, 17);
declare_member(TextConverter, std::string_view, text, 18);
declare_member(TextConverter, std::string_view, text, 19);
declare_member(TextConverter, std::string_view, text, 20);
declare_member(TextConverter, std::string_view, text, 21);
declare_member(TextConverter, std::string_view, text, 22);
declare_member(TextConverter, std::string_view, text, 23);
declare_member(TextConverter, std::string_view, text, 24);
declare_member(TextConverter, std::string_view, text, 25);
declare_member(TextConverter, std::string_view, text, 26);
declare_member(TextConverter, std::string_view, text, 27);
declare_member(TextConverter, std::string_view, text, 28);
declare_member(TextConverter, std::string_view, text, 29);
declare_member(TextConverter, std::string_view, text, 30);
declare_member(TextConverter, std::string_view, text, 31);
declare_member(TextConverter, std::string_view, text, 32);
declare_member(TextConverter, std::string_view, text, 33);
declare_member(TextConverter, std::string_view, text, 34);
declare_member(TextConverter, std::string_view, text, 35);
declare_member(TextConverter, std::string_view, text, 36);
declare_member(TextConverter, std::string_view, text, 37);
declare_member(TextConverter, std::string_view, text, 38);
declare_member(TextConverter, std::string_view, text, 39);
declare_member(TextConverter, std::string_view, text, 40);
declare_member(TextConverter, std::string_view, text, 41);
declare_member(TextConverter, std::string_view, text, 42);
declare_member(TextConverter, std::string_view, text, 43);
declare_member(TextConverter, std::string_view, text, 44);
declare_member(TextConverter, std::string_view, text, 45);
declare_member(TextConverter, std::string_view, text, 46);
declare_member(TextConverter, std::string_view, text, 47);
declare_member(TextConverter, std::string_view, text, 48);
declare_member(TextConverter, std::string_view, text, 49);
declare_member(TextConverter, std::string_view, text, 50);
declare_member(TextConverter, std::string_view, text, 51);
declare_member(TextConverter, std::string_view, text, 52);
declare_member(TextConverter, std::string_view, text, 53);
declare_member(TextConverter, std::string_view, text, 54);
declare_member(TextConverter, std::string_view, text, 55);
declare_member(TextConverter, std::string_view, text, 56);
declare_member(TextConverter, std::string_view, text, 57);
declare_member(TextConverter, std::string_view, text, 58);
declare_member(TextConverter, std::string_view, text, 59);
declare_member(TextConverter, std::string_view, text, 60);
declare_member(TextConverter, std::string_view, text, 61);
declare_member(TextConverter, std::string_view, text, 62);
declare_member(TextConverter, std::string_view, text, 63);
declare_member(TextConverter, std::string_view, text, 64);
declare_member(TextConverter, std::string_view, text, 65);
declare_member(TextConverter, std::string_view, text, 66);
declare_member(TextConverter, std::string_view, text, 67);
declare_member(TextConverter, std::string_view, text, 68);
declare_member(TextConverter, std::string_view, text, 69);
declare_member(TextConverter, std::string_view, text, 70);
declare_member(TextConverter, std::string_view, text, 71);
declare_member(TextConverter, std::string_view, text, 72);
declare_member(TextConverter, std::string_view, text, 73);
declare_member(TextConverter, std::string_view, text, 74);
declare_member(TextConverter, std::string_view, text, 75);
declare_member(TextConverter, std::string_view, text, 76);
declare_member(TextConverter, std::string_view, text, 77);
declare_member(TextConverter, std::string_view, text, 78);
declare_member(TextConverter, std::string_view, text, 79);
declare_member(TextConverter, std::string_view, text, 80);
declare_member(TextConverter, std::string_view, text, 81);
declare_member(TextConverter, std::string_view, text, 82);
declare_member(TextConverter, std::string_view, text, 83);
declare_member(TextConverter, std::string_view, text, 84);
declare_member(TextConverter, std::string_view, text, 85);
declare_member(TextConverter, std::string_view, text, 86);
declare_member(TextConverter, std::string_view, text, 87);
declare_member(TextConverter, std::string_view, text, 88);
declare_member(TextConverter, std::string_view, text, 89);
declare_member(TextConverter, std::string_view, text, 90);
declare_member(TextConverter, std::string_view, text, 91);
declare_member(TextConverter, std::string_view, text, 92);
declare_member(TextConverter, std::string_view, text, 93);
declare_member(TextConverter, std::string_view, text, 94);
declare_member(TextConverter, std::string_view, text, 95);
declare_member(TextConverter, std::string_view, text, 96);
declare_member(TextConverter, std::string_view, text, 97);
declare_member(TextConverter, std::string_view, text, 98);
declare_member(TextConverter, std::string_view, text, 99);
declare_member(TextConverter, std::string_view, text, 100);
declare_member(TextConverter, std::string_view, text, 101);
declare_member(TextConverter, std::string_view, text, 102);
declare_member(TextConverter, std::string_view, text, 103);
declare_member(TextConverter, std::string_view, text, 104);
declare_member(TextConverter, std::string_view, text, 105);
declare_member(TextConverter, std::string_view, text, 106);
declare_member(TextConverter, std::string_view, text, 107);
declare_member(TextConverter, std::string_view, text, 108);
declare_member(TextConverter, std::string_view, text, 109);
declare_member(TextConverter, std::string_view, text, 110);
declare_member(TextConverter, std::string_view, text, 111);
declare_member(TextConverter, std::string_view, text, 112);
declare_member(TextConverter, std::string_view, text, 113);
declare_member(TextConverter, std::string_view, text, 114);
declare_member(TextConverter, std::string_view, text, 115);
declare_member(TextConverter, std::string_view, text, 116);
declare_member(TextConverter, std::string_view, text, 117);
declare_member(TextConverter, std::string_view, text, 118);
declare_member(TextConverter, std::string_view, text, 119);
declare_member(TextConverter, std::string_view, text, 120);
declare_member(TextConverter, std::string_view, text, 121);
declare_member(TextConverter, std::string_view, text, 122);
declare_member(TextConverter, std::string_view, text, 123);
declare_member(TextConverter, std::string_view, text, 124);
declare_member(TextConverter, std::string_view, text, 125);
declare_member(TextConverter, std::string_view, text, 126);
declare_member(TextConverter, std::string_view, text, 127);
declare_member(TextConverter, std::string_view, text, 128);
declare_member(TextConverter, std::string_view, text, 129);
declare_member(TextConverter, std::string_view, text, 130);
declare_member(TextConverter, std::string_view, text, 131);
declare_member(TextConverter, std::string_view, text, 132);
declare_member(TextConverter, std::string_view, text, 133);
declare_member(TextConverter, std::string_view, text, 134);
declare_member(TextConverter, std::string_view, text, 135);
declare_member(TextConverter, std::string_view, text, 136);
declare_member(TextConverter, std::string_view, text, 137);
declare_member(TextConverter, std::string_view, text, 138);
declare_member(TextConverter, std::string_view, text, 139);
declare_member(TextConverter, std::string_view, text, 140);
declare_member(TextConverter, std::string_view, text, 141);
declare_member(TextConverter, std::string_view, text, 142);
declare_member(TextConverter, std::string_view, text, 143);
declare_member(TextConverter, std::string_view, text, 144);
declare_member(TextConverter, std::string_view, text, 145);
declare_member(TextConverter, std::string_view, text, 146);
declare_member(TextConverter, std::string_view, text, 147);
declare_member(TextConverter, std::string_view, text, 148);
declare_member(TextConverter, std::string_view, text, 149);
declare_member(TextConverter, std::string_view, text, 150);
declare_member(TextConverter, std::string_view, text, 151);
declare_member(TextConverter, std::string_view, text, 152);
declare_member(TextConverter, std::string_view, text, 153);
declare_member(TextConverter, std::string_view, text, 154);
declare_member(TextConverter, std::string_view, text, 155);
declare_member(TextConverter, std::string_view, text, 156);
declare_member(TextConverter, std::string_view, text, 157);
declare_member(TextConverter, std::string_view, text, 158);
declare_member(TextConverter, std::string_view, text, 159);
declare_member(TextConverter, std::string_view, text, 160);
declare_member(TextConverter, std::string_view, text, 161);
declare_member(TextConverter, std::string_view, text, 162);
declare_member(TextConverter, std::string_view, text, 163);
declare_member(TextConverter, std::string_view, text, 164);
declare_member(TextConverter, std::string_view, text, 165);
declare_member(TextConverter, std::string_view, text, 166);
declare_member(TextConverter, std::string_view, text, 167);
declare_member(TextConverter, std::string_view, text, 168);
declare_member(TextConverter, std::string_view, text, 169);
declare_member(TextConverter, std::string_view, text, 170);
declare_member(TextConverter, std::string_view, text, 171);
declare_member(TextConverter, std::string_view, text, 172);
declare_member(TextConverter, std::string_view, text, 173);
declare_member(TextConverter, std::string_view, text, 174);
declare_member(TextConverter, std::string_view, text, 175);
declare_member(TextConverter, std::string_view, text, 176);
declare_member(TextConverter, std::string_view, text, 177);
declare_member(TextConverter, std::string_view, text, 178);
declare_member(TextConverter, std::string_view, text, 179);
declare_member(TextConverter, std::string_view, text, 180);
declare_member(TextConverter, std::string_view, text, 181);
declare_member(TextConverter, std::string_view, text, 182);
declare_member(TextConverter, std::string_view, text, 183);
declare_member(TextConverter, std::string_view, text, 184);
declare_member(TextConverter, std::string_view, text, 185);
declare_member(TextConverter, std::string_view, text, 186);
declare_member(TextConverter, std::string_view, text, 187);
declare_member(TextConverter, std::string_view, text, 188);
declare_member(TextConverter, std::string_view, text, 189);
declare_member(TextConverter, std::string_view, text, 190);
declare_member(TextConverter, std::string_view, text, 191);
declare_member(TextConverter, std::string_view, text, 192);
declare_member(TextConverter, std::string_view, text, 193);
declare_member(TextConverter, std::string_view, text, 194);
declare_member(TextConverter, std::string_view, text, 195);
declare_member(TextConverter, std::string_view, text, 196);
declare_member(TextConverter, std::string_view, text, 197);
declare_member(TextConverter, std::string_view, text, 198);
declare_member(TextConverter, std::string_view, text, 199);
declare_member(TextConverter, std::string_view, text, 200);
declare_member(TextConverter, std::string_view, text, 201);
declare_member(TextConverter, std::string_view, text, 202);
declare_member(TextConverter, std::string_view, text, 203);
declare_member(TextConverter, std::string_view, text, 204);
declare_member(TextConverter, std::string_view, text, 205);
declare_member(TextConverter, std::string_view, text, 206);
declare_member(TextConverter, std::string_view, text, 207);
declare_member(TextConverter, std::string_view, text, 208);
declare_member(TextConverter, std::string_view, text, 209);
declare_member(TextConverter, std::string_view, text, 210);
declare_member(TextConverter, std::string_view, text, 211);
declare_member(TextConverter, std::string_view, text, 212);
declare_member(TextConverter, std::string_view, text, 213);
declare_member(TextConverter, std::string_view, text, 214);
declare_member(TextConverter, std::string_view, text, 215);
declare_member(TextConverter, std::string_view, text, 216);
declare_member(TextConverter, std::string_view, text, 217);
declare_member(TextConverter, std::string_view, text, 218);
declare_member(TextConverter, std::string_view, text, 219);
declare_member(TextConverter, std::string_view, text, 220);
declare_member(TextConverter, std::string_view, text, 221);
declare_member(TextConverter, std::string_view, text, 222);
declare_member(TextConverter, std::string_view, text, 223);
declare_member(TextConverter, std::string_view, text, 224);
declare_member(TextConverter, std::string_view, text, 225);
declare_member(TextConverter, std::string_view, text, 226);
declare_member(TextConverter, std::string_view, text, 227);
declare_member(TextConverter, std::string_view, text, 228);
declare_member(TextConverter, std::string_view, text, 229);
declare_member(TextConverter, std::string_view, text, 230);
declare_member(TextConverter, std::string_view, text, 231);
declare_member(TextConverter, std::string_view, text, 232);
declare_member(TextConverter, std::string_view, text, 233);
declare_member(TextConverter, std::string_view, text, 234);
declare_member(TextConverter, std::string_view, text, 235);
declare_member(TextConverter, std::string_view, text, 236);
declare_member(TextConverter, std::string_view, text, 237);
declare_member(TextConverter, std::string_view, text, 238);
declare_member(TextConverter, std::string_view, text, 239);
declare_member(TextConverter, std::string_view, text, 240);
declare_member(TextConverter, std::string_view, text, 241);
declare_member(TextConverter, std::string_view, text, 242);
declare_member(TextConverter, std::string_view, text, 243);
declare_member(TextConverter, std::string_view, text, 244);
declare_member(TextConverter, std::string_view, text, 245);
declare_member(TextConverter, std::string_view, text, 246);
declare_member(TextConverter, std::string_view, text, 247);
declare_member(TextConverter, std::string_view, text, 248);
declare_member(TextConverter, std::string_view, text, 249);
declare_member(TextConverter, std::string_view, text, 250);
declare_member(TextConverter, std::string_view, text, 251);
declare_member(TextConverter, std::string_view, text, 252);
declare_member(TextConverter, std::string_view, text, 253);
declare_member(TextConverter, std::string_view, text, 254);
declare_member(TextConverter, std::string_view, text, 255);
declare_member(TextConverter, std::string_view, text, 256);
declare_member(TextConverter, std::string_view, text, 257);
declare_member(TextConverter, std::string_view, text, 258);
declare_member(TextConverter, std::string_view, text, 259);
declare_member(TextConverter, std::string_view, text, 260);
declare_member(TextConverter, std::string_view, text, 261);
declare_member(TextConverter, std::string_view, text, 262);
declare_member(TextConverter, std::string_view, text, 263);
declare_member(TextConverter, std::string_view, text, 264);
declare_member(TextConverter, std::string_view, text, 265);
declare_member(TextConverter, std::string_view, text, 266);
declare_member(TextConverter, std::string_view, text, 267);
declare_member(TextConverter, std::string_view, text, 268);
declare_member(TextConverter, std::string_view, text, 269);
declare_member(TextConverter, std::string_view, text, 270);
declare_member(TextConverter, std::string_view, text, 271);
declare_member(TextConverter, std::string_view, text, 272);
declare_member(TextConverter, std::string_view, text, 273);
declare_member(TextConverter, std::string_view, text, 274);
declare_member(TextConverter, std::string_view, text, 275);
declare_member(TextConverter, std::string_view, text, 276);
declare_member(TextConverter, std::string_view, text, 277);
declare_member(TextConverter, std::string_view, text, 278);
declare_member(TextConverter, std::string_view, text, 279);
declare_member(TextConverter, std::string_view, text, 280);
declare_member(TextConverter, std::string_view, text, 281);
declare_member(TextConverter, std::string_view, text, 282);
declare_member(TextConverter, std::string_view, text, 283);
declare_member(TextConverter, std::string_view, text, 284);
declare_member(TextConverter, std::string_view, text, 285);
declare_member(TextConverter, std::string_view, text, 286);
declare_member(TextConverter, std::string_view, text, 287);
declare_member(TextConverter, std::string_view, text, 288);
declare_member(TextConverter, std::string_view, text, 289);
declare_member(TextConverter, std::string_view, text, 290);
declare_member(TextConverter, std::string_view, text, 291);
declare_member(TextConverter, std::string_view, text, 292);
declare_member(TextConverter, std::string_view, text, 293);
declare_member(TextConverter, std::string_view, text, 294);
declare_member(TextConverter, std::string_view, text, 295);
declare_member(TextConverter, std::string_view, text, 296);
declare_member(TextConverter, std::string_view, text, 297);
declare_member(TextConverter, std::string_view, text, 298);
declare_member(TextConverter, std::string_view, text, 299);
declare_member(TextConverter, std::string_view, text, 300);
declare_member(TextConverter, std::string_view, text, 301);
declare_member(TextConverter, std::string_view, text, 302);
declare_member(TextConverter, std::string_view, text, 303);
declare_member(TextConverter, std::string_view, text, 304);
declare_member(TextConverter, std::string_view, text, 305);
declare_member(TextConverter, std::string_view, text, 306);
declare_member(TextConverter, std::string_view, text, 307);
declare_member(TextConverter, std::string_view, text, 308);
declare_member(TextConverter, std::string_view, text, 309);
declare_member(TextConverter, std::string_view, text, 310);
declare_member(TextConverter, std::string_view, text, 311);
declare_member(TextConverter, std::string_view, text, 312);
declare_member(TextConverter, std::string_view, text, 313);
declare_member(TextConverter, std::string_view, text, 314);
declare_member(TextConverter, std::string_view, text, 315);
declare_member(TextConverter, std::string_view, text, 316);
declare_member(TextConverter, std::string_view, text, 317);
declare_member(TextConverter, std::string_view, text, 318);
declare_member(TextConverter, std::string_view, text, 319);
declare_member(TextConverter, std::string_view, text, 320);
declare_member(TextConverter, std::string_view, text, 321);
declare_member(TextConverter, std::string_view, text, 322);
declare_member(TextConverter, std::string_view, text, 323);
declare_member(TextConverter, std::string_view, text, 324);
declare_member(TextConverter, std::string_view, text, 325);
declare_member(TextConverter, std::string_view, text, 326);
declare_member(TextConverter, std::string_view, text, 327);
declare_member(TextConverter, std::string_view, text, 328);
declare_member(TextConverter, std::string_view, text, 329);
declare_member(TextConverter, std::string_view, text, 330);
declare_member(TextConverter, std::string_view, text, 331);
declare_member(TextConverter, std::string_view, text, 332);
declare_member(TextConverter, std::string_view, text, 333);
declare_member(TextConverter, std::string_view, text, 334);
declare_member(TextConverter, std::string_view, text, 335);
declare_member(TextConverter, std::string_view, text, 336);
declare_member(TextConverter, std::string_view, text, 337);
declare_member(TextConverter, std::string_view, text, 338);
declare_member(TextConverter, std::string_view, text, 339);
declare_member(TextConverter, std::string_view, text, 340);
declare_member(TextConverter, std::string_view, text, 341);
declare_member(TextConverter, std::string_view, text, 342);
declare_member(TextConverter, std::string_view, text, 343);
declare_member(TextConverter, std::string_view, text, 344);
declare_member(TextConverter, std::string_view, text, 345);
declare_member(TextConverter, std::string_view, text, 346);
declare_member(TextConverter, std::string_view, text, 347);
declare_member(TextConverter, std::string_view, text, 348);
declare_member(TextConverter, std::string_view, text, 349);
declare_member(TextConverter, std::string_view, text, 350);
declare_member(TextConverter, std::string_view, text, 351);
declare_member(TextConverter, std::string_view, text, 352);
declare_member(TextConverter, std::string_view, text, 353);
declare_member(TextConverter, std::string_view, text, 354);
declare_member(TextConverter, std::string_view, text, 355);
declare_member(TextConverter, std::string_view, text, 356);
declare_member(TextConverter, std::string_view, text, 357);
declare_member(TextConverter, std::string_view, text, 358);
declare_member(TextConverter, std::string_view, text, 359);
declare_member(TextConverter, std::string_view, text, 360);
declare_member(TextConverter, std::string_view, text, 361);
declare_member(TextConverter, std::string_view, text, 362);
declare_member(TextConverter, std::string_view, text, 363);
declare_member(TextConverter, std::string_view, text, 364);
declare_member(TextConverter, std::string_view, text, 365);
declare_member(TextConverter, std::string_view, text, 366);
declare_member(TextConverter, std::string_view, text, 367);
declare_member(TextConverter, std::string_view, text, 368);
declare_member(TextConverter, std::string_view, text, 369);
declare_member(TextConverter, std::string_view, text, 370);
declare_member(TextConverter, std::string_view, text, 371);
declare_member(TextConverter, std::string_view, text, 372);
declare_member(TextConverter, std::string_view, text, 373);
declare_member(TextConverter, std::string_view, text, 374);
declare_member(TextConverter, std::string_view, text, 375);
declare_member(TextConverter, std::string_view, text, 376);
declare_member(TextConverter, std::string_view, text, 377);
declare_member(TextConverter, std::string_view, text, 378);
declare_member(TextConverter, std::string_view, text, 379);
declare_member(TextConverter, std::string_view, text, 380);
declare_member(TextConverter, std::string_view, text, 381);
declare_member(TextConverter, std::string_view, text, 382);
declare_member(TextConverter, std::string_view, text, 383);
declare_member(TextConverter, std::string_view, text, 384);
declare_member(TextConverter, std::string_view, text, 385);
declare_member(TextConverter, std::string_view, text, 386);
declare_member(TextConverter, std::string_view, text, 387);
declare_member(TextConverter, std::string_view, text, 388);
declare_member(TextConverter, std::string_view, text, 389);
declare_member(TextConverter, std::string_view, text, 390);
declare_member(TextConverter, std::string_view, text, 391);
declare_member(TextConverter, std::string_view, text, 392);
declare_member(TextConverter, std::string_view, text, 393);
declare_member(TextConverter, std::string_view, text, 394);
declare_member(TextConverter, std::string_view, text, 395);
declare_member(TextConverter, std::string_view, text, 396);
declare_member(TextConverter, std::string_view, text, 397);
declare_member(TextConverter, std::string_view, text, 398);
declare_member(TextConverter, std::string_view, text, 399);

#define text(index) member(TextConverter, std::string_view, text, index)

#pragma endregion

namespace lr2skin
{

std::function<std::string_view()> convertTextIndex(int n)
{
    if (n >= 0 && n <= 399)
    {
        static constexpr std::string_view(*kv[400])() =
        {
            text(0),
            text(1),
            text(2),
            text(3),
            text(4),
            text(5),
            text(6),
            text(7),
            text(8),
            text(9),
            text(10),
            text(11),
            text(12),
            text(13),
            text(14),
            text(15),
            text(16),
            text(17),
            text(18),
            text(19),
            text(20),
            text(21),
            text(22),
            text(23),
            text(24),
            text(25),
            text(26),
            text(27),
            text(28),
            text(29),
            text(30),
            text(31),
            text(32),
            text(33),
            text(34),
            text(35),
            text(36),
            text(37),
            text(38),
            text(39),
            text(40),
            text(41),
            text(42),
            text(43),
            text(44),
            text(45),
            text(46),
            text(47),
            text(48),
            text(49),
            text(50),
            text(51),
            text(52),
            text(53),
            text(54),
            text(55),
            text(56),
            text(57),
            text(58),
            text(59),
            text(60),
            text(61),
            text(62),
            text(63),
            text(64),
            text(65),
            text(66),
            text(67),
            text(68),
            text(69),
            text(70),
            text(71),
            text(72),
            text(73),
            text(74),
            text(75),
            text(76),
            text(77),
            text(78),
            text(79),
            text(80),
            text(81),
            text(82),
            text(83),
            text(84),
            text(85),
            text(86),
            text(87),
            text(88),
            text(89),
            text(90),
            text(91),
            text(92),
            text(93),
            text(94),
            text(95),
            text(96),
            text(97),
            text(98),
            text(99),
            text(100),
            text(101),
            text(102),
            text(103),
            text(104),
            text(105),
            text(106),
            text(107),
            text(108),
            text(109),
            text(110),
            text(111),
            text(112),
            text(113),
            text(114),
            text(115),
            text(116),
            text(117),
            text(118),
            text(119),
            text(120),
            text(121),
            text(122),
            text(123),
            text(124),
            text(125),
            text(126),
            text(127),
            text(128),
            text(129),
            text(130),
            text(131),
            text(132),
            text(133),
            text(134),
            text(135),
            text(136),
            text(137),
            text(138),
            text(139),
            text(140),
            text(141),
            text(142),
            text(143),
            text(144),
            text(145),
            text(146),
            text(147),
            text(148),
            text(149),
            text(150),
            text(151),
            text(152),
            text(153),
            text(154),
            text(155),
            text(156),
            text(157),
            text(158),
            text(159),
            text(160),
            text(161),
            text(162),
            text(163),
            text(164),
            text(165),
            text(166),
            text(167),
            text(168),
            text(169),
            text(170),
            text(171),
            text(172),
            text(173),
            text(174),
            text(175),
            text(176),
            text(177),
            text(178),
            text(179),
            text(180),
            text(181),
            text(182),
            text(183),
            text(184),
            text(185),
            text(186),
            text(187),
            text(188),
            text(189),
            text(190),
            text(191),
            text(192),
            text(193),
            text(194),
            text(195),
            text(196),
            text(197),
            text(198),
            text(199),
            text(200),
            text(201),
            text(202),
            text(203),
            text(204),
            text(205),
            text(206),
            text(207),
            text(208),
            text(209),
            text(210),
            text(211),
            text(212),
            text(213),
            text(214),
            text(215),
            text(216),
            text(217),
            text(218),
            text(219),
            text(220),
            text(221),
            text(222),
            text(223),
            text(224),
            text(225),
            text(226),
            text(227),
            text(228),
            text(229),
            text(230),
            text(231),
            text(232),
            text(233),
            text(234),
            text(235),
            text(236),
            text(237),
            text(238),
            text(239),
            text(240),
            text(241),
            text(242),
            text(243),
            text(244),
            text(245),
            text(246),
            text(247),
            text(248),
            text(249),
            text(250),
            text(251),
            text(252),
            text(253),
            text(254),
            text(255),
            text(256),
            text(257),
            text(258),
            text(259),
            text(260),
            text(261),
            text(262),
            text(263),
            text(264),
            text(265),
            text(266),
            text(267),
            text(268),
            text(269),
            text(270),
            text(271),
            text(272),
            text(273),
            text(274),
            text(275),
            text(276),
            text(277),
            text(278),
            text(279),
            text(280),
            text(281),
            text(282),
            text(283),
            text(284),
            text(285),
            text(286),
            text(287),
            text(288),
            text(289),
            text(290),
            text(291),
            text(292),
            text(293),
            text(294),
            text(295),
            text(296),
            text(297),
            text(298),
            text(299),
            text(300),
            text(301),
            text(302),
            text(303),
            text(304),
            text(305),
            text(306),
            text(307),
            text(308),
            text(309),
            text(310),
            text(311),
            text(312),
            text(313),
            text(314),
            text(315),
            text(316),
            text(317),
            text(318),
            text(319),
            text(320),
            text(321),
            text(322),
            text(323),
            text(324),
            text(325),
            text(326),
            text(327),
            text(328),
            text(329),
            text(330),
            text(331),
            text(332),
            text(333),
            text(334),
            text(335),
            text(336),
            text(337),
            text(338),
            text(339),
            text(340),
            text(341),
            text(342),
            text(343),
            text(344),
            text(345),
            text(346),
            text(347),
            text(348),
            text(349),
            text(350),
            text(351),
            text(352),
            text(353),
            text(354),
            text(355),
            text(356),
            text(357),
            text(358),
            text(359),
            text(360),
            text(361),
            text(362),
            text(363),
            text(364),
            text(365),
            text(366),
            text(367),
            text(368),
            text(369),
            text(370),
            text(371),
            text(372),
            text(373),
            text(374),
            text(375),
            text(376),
            text(377),
            text(378),
            text(379),
            text(380),
            text(381),
            text(382),
            text(383),
            text(384),
            text(385),
            text(386),
            text(387),
            text(388),
            text(389),
            text(390),
            text(391),
            text(392),
            text(393),
            text(394),
            text(395),
            text(396),
            text(397),
            text(398),
            text(399),
        };

        return kv[n];
    }

    return []() -> std::string_view { return ""; };
}

}
}