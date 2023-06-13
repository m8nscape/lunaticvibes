#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/data/data_types.h"
#include "db/db_score.h"

namespace lunaticvibes
{

class BargraphConverter
{
protected:

    static std::shared_ptr<ScoreBase> getCurrentSelectedEntryScore()
    {
        auto p = SelectData.songList.getCurrentEntry();
        if (!p)
            return nullptr;
        return p->score;
    }

    static std::shared_ptr<ScoreBMS> getCurrentSelectedEntryScoreBMS()
    {
        return std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
    }

public:

    static Ratio bargraph_arena_0(int player)
    {
        if (!ArenaData.isOnline()) return 0.0;

        auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(player));
        if (r && r->getMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getMaxScore();
        }
        return 0.0;
    }

    static Ratio bargraph_arena_1(int player)
    {
        if (!ArenaData.isOnline()) return 0.0;

        auto r = std::dynamic_pointer_cast<RulesetBMSNetwork>(ArenaData.getPlayerRuleset(player));
        if (r && r->getMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    }

    static Ratio bargraph_0() { return 0.0; };
    static Ratio bargraph_1()
    {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c && c->getTotalLength() != 0 && SystemData.gNextScene == SceneType::PLAY)
        {
            auto t = Time().norm() - PlayData.timers["play_start"];
            return double(t) / c->getTotalLength().norm();
        }
        return 0;
    };
    static Ratio bargraph_2() { return (PlayData.loadProgressWav + PlayData.loadProgressBga) / 2.0; };
    /*
        LEVEL_BAR = 3,

        LEVEL_BAR_BEGINNER = 5,
        LEVEL_BAR_NORMAL,
        LEVEL_BAR_HYPER,
        LEVEL_BAR_ANOTHER,
        LEVEL_BAR_INSANE,
    */
    static Ratio bargraph_10()
    {
        if (ArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_11()
    {
        if (ArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getCurrentMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_12()
    {
        if (ArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_MYBEST].ruleset);
        if (r)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_13()
    {
        if (ArenaData.isOnline()) return 0.0;
        auto pScore = g_pScoreDB->getChartScoreBMS(SelectData.selectedChart.hash);
        if (pScore)
        {
            auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_MYBEST].ruleset);
            if (r && r->getMaxScore() != 0)
            {
                return double(pScore->exscore) / r->getMaxScore();
            }
        }
        return 0.0;
    };
    static Ratio bargraph_14()
    {
        if (ArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getCurrentMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_15()
    {
        if (ArenaData.isOnline()) return 0.0;
        if (PlayData.battleType == PlayModifierBattleType::LocalBattle ||
            PlayData.battleType == PlayModifierBattleType::GhostBattle)
        {
            auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
            if (r && r->getCurrentMaxScore() != 0)
            {
                return double(r->getExScore()) / r->getCurrentMaxScore();
            }
        }
        else
        {
            auto r = std::dynamic_pointer_cast<RulesetBMSAuto>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
            if (r)
            {
                return r->getTargetRate() / 100.0;
            }
        }
        return 0.0;
    };
    static Ratio bargraph_16()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_17()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getCurrentMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_20()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::PERFECT) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_21()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::GREAT) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_22()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::GOOD) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_23()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::BAD) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_24()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_POOR) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_25()
    {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getData().maxCombo / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_26()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getMaxMoneyScore() != 0)
        {
            return r->getScore() / r->getMaxMoneyScore();
        }
        return 0.0;
    };
    static Ratio bargraph_27()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getMaxScore() != 0)
        {
            return (double)r->getExScore() / r->getMaxScore();
        }
        return 0.0;
    };

    static Ratio bargraph_30()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::PERFECT) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_31()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::GREAT) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_32()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::GOOD) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_33()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCount(RulesetBMS::JudgeType::BAD) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_34()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_POOR) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_35()
    {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getData().maxCombo / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_36()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getMaxMoneyScore() != 0)
        {
            return r->getScore() / r->getMaxMoneyScore();
        }
        return 0.0;
    };
    static Ratio bargraph_37()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getMaxScore() != 0)
        {
            return (double)r->getExScore() / r->getMaxScore();
        }
        return 0.0;
    };

    static Ratio bargraph_40()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)r->pgreat / r->notes;
        }
        return 0.0;
    };
    static Ratio bargraph_41()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)r->great / r->notes;
        }
        return 0.0;
    };
    static Ratio bargraph_42()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)r->good / r->notes;
        }
        return 0.0;
    };
    static Ratio bargraph_43()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)r->bad / r->notes;
        }
        return 0.0;
    };
    static Ratio bargraph_44()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)(r->kpoor + r->miss) / r->notes;
        }
        return 0.0;
    };
    static Ratio bargraph_45()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)r->maxcombo / r->notes;
        }
        return 0.0;
    };
    static Ratio bargraph_46()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r)
        {
            return (double)r->score / 200000.0;
        }
        return 0.0;
    };
    static Ratio bargraph_47()
    {
        auto r = getCurrentSelectedEntryScoreBMS();
        if (r && r->notes != 0)
        {
            return (double)r->exscore / (r->notes * 2);
        }
        return 0.0;
    };
    static Ratio bargraph_49()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_EARLY) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_48()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_LATE) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_59()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_EARLY) / r->getNoteCount();
        }
        return 0.0;
    };
    static Ratio bargraph_58()
    {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getNoteCount() != 0)
        {
            return (double)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_LATE) / r->getNoteCount();
        }
        return 0.0;
    };

    static Ratio bargraph_61()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K11];
        }
        return 0.0;
    };
    static Ratio bargraph_62()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K12];
        }
        return 0.0;
    };
    static Ratio bargraph_63()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K13];
        }
        return 0.0;
    };
    static Ratio bargraph_64()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K14];
        }
        return 0.0;
    };
    static Ratio bargraph_65()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K15];
        }
        return 0.0;
    };
    static Ratio bargraph_66()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K16];
        }
        return 0.0;
    };
    static Ratio bargraph_67()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K17];
        }
        return 0.0;
    };
    static Ratio bargraph_68()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K18];
        }
        return 0.0;
    };
    static Ratio bargraph_69()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K19];
        }
        return 0.0;
    };
    static Ratio bargraph_70()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K1START];
        }
        return 0.0;
    };
    static Ratio bargraph_71()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K1SELECT];
        }
        return 0.0;
    };
    static Ratio bargraph_81()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K21];
        }
        return 0.0;
    };
    static Ratio bargraph_82()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K22];
        }
        return 0.0;
    };
    static Ratio bargraph_83()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K23];
        }
        return 0.0;
    };
    static Ratio bargraph_84()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K24];
        }
        return 0.0;
    };
    static Ratio bargraph_85()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K25];
        }
        return 0.0;
    };
    static Ratio bargraph_86()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K26];
        }
        return 0.0;
    };
    static Ratio bargraph_87()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K27];
        }
        return 0.0;
    };
    static Ratio bargraph_88()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K28];
        }
        return 0.0;
    };
    static Ratio bargraph_89()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K29];
        }
        return 0.0;
    };
    static Ratio bargraph_90()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K2START];
        }
        return 0.0;
    };
    static Ratio bargraph_91()
    {
        if (SystemData.gNextScene == SceneType::KEYCONFIG)
        {
            using k = Input::Pad;
            return KeyConfigData.force[k::K2SELECT];
        }
        return 0.0;
    };
};

#pragma region declare_member

declare_member(BargraphConverter, Ratio, bargraph, 0);
declare_member(BargraphConverter, Ratio, bargraph, 1);
declare_member(BargraphConverter, Ratio, bargraph, 2);
declare_member(BargraphConverter, Ratio, bargraph, 3);
declare_member(BargraphConverter, Ratio, bargraph, 4);
declare_member(BargraphConverter, Ratio, bargraph, 5);
declare_member(BargraphConverter, Ratio, bargraph, 6);
declare_member(BargraphConverter, Ratio, bargraph, 7);
declare_member(BargraphConverter, Ratio, bargraph, 8);
declare_member(BargraphConverter, Ratio, bargraph, 9);
declare_member(BargraphConverter, Ratio, bargraph, 10);
declare_member(BargraphConverter, Ratio, bargraph, 11);
declare_member(BargraphConverter, Ratio, bargraph, 12);
declare_member(BargraphConverter, Ratio, bargraph, 13);
declare_member(BargraphConverter, Ratio, bargraph, 14);
declare_member(BargraphConverter, Ratio, bargraph, 15);
declare_member(BargraphConverter, Ratio, bargraph, 16);
declare_member(BargraphConverter, Ratio, bargraph, 17);
declare_member(BargraphConverter, Ratio, bargraph, 18);
declare_member(BargraphConverter, Ratio, bargraph, 19);
declare_member(BargraphConverter, Ratio, bargraph, 20);
declare_member(BargraphConverter, Ratio, bargraph, 21);
declare_member(BargraphConverter, Ratio, bargraph, 22);
declare_member(BargraphConverter, Ratio, bargraph, 23);
declare_member(BargraphConverter, Ratio, bargraph, 24);
declare_member(BargraphConverter, Ratio, bargraph, 25);
declare_member(BargraphConverter, Ratio, bargraph, 26);
declare_member(BargraphConverter, Ratio, bargraph, 27);
declare_member(BargraphConverter, Ratio, bargraph, 28);
declare_member(BargraphConverter, Ratio, bargraph, 29);
declare_member(BargraphConverter, Ratio, bargraph, 30);
declare_member(BargraphConverter, Ratio, bargraph, 31);
declare_member(BargraphConverter, Ratio, bargraph, 32);
declare_member(BargraphConverter, Ratio, bargraph, 33);
declare_member(BargraphConverter, Ratio, bargraph, 34);
declare_member(BargraphConverter, Ratio, bargraph, 35);
declare_member(BargraphConverter, Ratio, bargraph, 36);
declare_member(BargraphConverter, Ratio, bargraph, 37);
declare_member(BargraphConverter, Ratio, bargraph, 38);
declare_member(BargraphConverter, Ratio, bargraph, 39);
declare_member(BargraphConverter, Ratio, bargraph, 40);
declare_member(BargraphConverter, Ratio, bargraph, 41);
declare_member(BargraphConverter, Ratio, bargraph, 42);
declare_member(BargraphConverter, Ratio, bargraph, 43);
declare_member(BargraphConverter, Ratio, bargraph, 44);
declare_member(BargraphConverter, Ratio, bargraph, 45);
declare_member(BargraphConverter, Ratio, bargraph, 46);
declare_member(BargraphConverter, Ratio, bargraph, 47);
declare_member(BargraphConverter, Ratio, bargraph, 48);
declare_member(BargraphConverter, Ratio, bargraph, 49);
declare_member(BargraphConverter, Ratio, bargraph, 50);
declare_member(BargraphConverter, Ratio, bargraph, 51);
declare_member(BargraphConverter, Ratio, bargraph, 52);
declare_member(BargraphConverter, Ratio, bargraph, 53);
declare_member(BargraphConverter, Ratio, bargraph, 54);
declare_member(BargraphConverter, Ratio, bargraph, 55);
declare_member(BargraphConverter, Ratio, bargraph, 56);
declare_member(BargraphConverter, Ratio, bargraph, 57);
declare_member(BargraphConverter, Ratio, bargraph, 58);
declare_member(BargraphConverter, Ratio, bargraph, 59);
declare_member(BargraphConverter, Ratio, bargraph, 60);
declare_member(BargraphConverter, Ratio, bargraph, 61);
declare_member(BargraphConverter, Ratio, bargraph, 62);
declare_member(BargraphConverter, Ratio, bargraph, 63);
declare_member(BargraphConverter, Ratio, bargraph, 64);
declare_member(BargraphConverter, Ratio, bargraph, 65);
declare_member(BargraphConverter, Ratio, bargraph, 66);
declare_member(BargraphConverter, Ratio, bargraph, 67);
declare_member(BargraphConverter, Ratio, bargraph, 68);
declare_member(BargraphConverter, Ratio, bargraph, 69);
declare_member(BargraphConverter, Ratio, bargraph, 70);
declare_member(BargraphConverter, Ratio, bargraph, 71);
declare_member(BargraphConverter, Ratio, bargraph, 72);
declare_member(BargraphConverter, Ratio, bargraph, 73);
declare_member(BargraphConverter, Ratio, bargraph, 74);
declare_member(BargraphConverter, Ratio, bargraph, 75);
declare_member(BargraphConverter, Ratio, bargraph, 76);
declare_member(BargraphConverter, Ratio, bargraph, 77);
declare_member(BargraphConverter, Ratio, bargraph, 78);
declare_member(BargraphConverter, Ratio, bargraph, 79);
declare_member(BargraphConverter, Ratio, bargraph, 80);
declare_member(BargraphConverter, Ratio, bargraph, 81);
declare_member(BargraphConverter, Ratio, bargraph, 82);
declare_member(BargraphConverter, Ratio, bargraph, 83);
declare_member(BargraphConverter, Ratio, bargraph, 84);
declare_member(BargraphConverter, Ratio, bargraph, 85);
declare_member(BargraphConverter, Ratio, bargraph, 86);
declare_member(BargraphConverter, Ratio, bargraph, 87);
declare_member(BargraphConverter, Ratio, bargraph, 88);
declare_member(BargraphConverter, Ratio, bargraph, 89);
declare_member(BargraphConverter, Ratio, bargraph, 90);
declare_member(BargraphConverter, Ratio, bargraph, 91);
declare_member(BargraphConverter, Ratio, bargraph, 92);
declare_member(BargraphConverter, Ratio, bargraph, 93);
declare_member(BargraphConverter, Ratio, bargraph, 94);
declare_member(BargraphConverter, Ratio, bargraph, 95);
declare_member(BargraphConverter, Ratio, bargraph, 96);
declare_member(BargraphConverter, Ratio, bargraph, 97);
declare_member(BargraphConverter, Ratio, bargraph, 98);
declare_member(BargraphConverter, Ratio, bargraph, 99);

#define bargraph(index) member(BargraphConverter, Ratio, bargraph, index)

#pragma endregion

namespace lr2skin
{

std::function<Ratio()> convertBargraphIndex(int n)
{
    if (n >= 100 && n <= 179)
    {
        int player = (n - 100) / 10;
        int index = (n - 100) % 10;

        switch (index)
        {
        case 0: return std::bind(BargraphConverter::bargraph_arena_0, player);
        case 1: return std::bind(BargraphConverter::bargraph_arena_1, player);
        }
    }
    else if (n >= 0 && n <= 99)
    {
        static constexpr Ratio(*kv[100])() =
        {
            bargraph(0),
            bargraph(1),
            bargraph(2),
            bargraph(3),
            bargraph(4),
            bargraph(5),
            bargraph(6),
            bargraph(7),
            bargraph(8),
            bargraph(9),
            bargraph(10),
            bargraph(11),
            bargraph(12),
            bargraph(13),
            bargraph(14),
            bargraph(15),
            bargraph(16),
            bargraph(17),
            bargraph(18),
            bargraph(19),
            bargraph(20),
            bargraph(21),
            bargraph(22),
            bargraph(23),
            bargraph(24),
            bargraph(25),
            bargraph(26),
            bargraph(27),
            bargraph(28),
            bargraph(29),
            bargraph(30),
            bargraph(31),
            bargraph(32),
            bargraph(33),
            bargraph(34),
            bargraph(35),
            bargraph(36),
            bargraph(37),
            bargraph(38),
            bargraph(39),
            bargraph(40),
            bargraph(41),
            bargraph(42),
            bargraph(43),
            bargraph(44),
            bargraph(45),
            bargraph(46),
            bargraph(47),
            bargraph(48),
            bargraph(49),
            bargraph(50),
            bargraph(51),
            bargraph(52),
            bargraph(53),
            bargraph(54),
            bargraph(55),
            bargraph(56),
            bargraph(57),
            bargraph(58),
            bargraph(59),
            bargraph(60),
            bargraph(61),
            bargraph(62),
            bargraph(63),
            bargraph(64),
            bargraph(65),
            bargraph(66),
            bargraph(67),
            bargraph(68),
            bargraph(69),
            bargraph(70),
            bargraph(71),
            bargraph(72),
            bargraph(73),
            bargraph(74),
            bargraph(75),
            bargraph(76),
            bargraph(77),
            bargraph(78),
            bargraph(79),
            bargraph(80),
            bargraph(81),
            bargraph(82),
            bargraph(83),
            bargraph(84),
            bargraph(85),
            bargraph(86),
            bargraph(87),
            bargraph(88),
            bargraph(89),
            bargraph(90),
            bargraph(91),
            bargraph(92),
            bargraph(93),
            bargraph(94),
            bargraph(95),
            bargraph(96),
            bargraph(97),
            bargraph(98),
            bargraph(99),
        };

        return kv[n];
    }

    return [] { return 0.0; };
}

}
}
