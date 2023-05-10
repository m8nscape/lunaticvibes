#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"
#include "game/data/data_types.h"
#include "game/arena/arena_data.h"
#include "db/db_score.h"

namespace lv
{

using namespace data;

class BargraphConverter
{
protected:

    static std::shared_ptr<ScoreBase> getCurrentSelectedEntryScore()
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        return std::dynamic_pointer_cast<ScoreBase>(SelectData.entries[SelectData.selectedEntryIndex].second);
    }

    static std::shared_ptr<ScoreBMS> getCurrentSelectedEntryScoreBMS()
    {
        return std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
    }

public:

    static Ratio bargraph_arena_0(int player)
    {
        if (!gArenaData.isOnline()) return 0.0;

        auto r = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (r && r->getMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getMaxScore();
        }
        return 0.0;
    }

    static Ratio bargraph_arena_1(int player)
    {
        if (!gArenaData.isOnline()) return 0.0;

        auto r = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
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
    static Ratio bargraph_2() { (PlayData.loadProgressWav + PlayData.loadProgressBga) / 2.0; };
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
        if (gArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_11()
    {
        if (gArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r && r->getCurrentMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_12()
    {
        if (gArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_MYBEST].ruleset);
        if (r)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_13()
    {
        if (gArenaData.isOnline()) return 0.0;
        auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
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
        if (gArenaData.isOnline()) return 0.0;
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r && r->getCurrentMaxScore() != 0)
        {
            return double(r->getExScore()) / r->getCurrentMaxScore();
        }
        return 0.0;
    };
    static Ratio bargraph_15()
    {
        if (gArenaData.isOnline()) return 0.0;
        if (PlayData.isBattle)
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

#define define_has_member(index)                                                            \
class has_bargraph_##index                                                                  \
{                                                                                           \
private:                                                                                    \
    typedef long yes_type;                                                                  \
    typedef char no_type;                                                                   \
    template <typename U> static yes_type test(decltype(&U::bargraph_##index));             \
    template <typename U> static no_type  test(...);                                        \
public:                                                                                     \
    static constexpr bool has_func = sizeof(test<BargraphConverter>()) == sizeof(yes_type); \
private:                                                                                    \
    template <typename U, typename = std::enable_if_t<!has_func>>                           \
    static constexpr Ratio(*func())() { return &U::bargraph_0; }                            \
    template <typename U, typename = std::enable_if_t<has_func>>                            \
    static constexpr Ratio(*func())() { return &U::bargraph_##index; }                      \
public:                                                                                     \
    static constexpr Ratio(*value)() = func<BargraphConverter>();                           \
}

#define has_bargraph(index)  has_bargraph_##index::has_func
#define bargraph(index) has_bargraph_##index::value

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

#pragma endregion

}

namespace lr2skin
{

std::function<Ratio()> convertBargraphIndex(int n)
{
    using namespace lv;

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
