#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/chart/chart_types.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_network.h"
#include "game/data/data_types.h"
#include "db/db_score.h"

namespace lunaticvibes
{

class NumberConverter
{
public:
    static int number_arena_0(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getScore());
    }
    static int number_arena_1(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getExScore());
    }
    static int number_arena_2(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().combo);
    }
    static int number_arena_3(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().maxCombo);
    }
    static int number_arena_4(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().acc);
    }
    static int number_arena_5(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().acc * 100) % 100;
    }
    static int number_arena_6(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getNoteCount());
    }
    static int number_arena_7(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().total_acc);
    }
    static int number_arena_8(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().total_acc * 100) % 100;
    }
    static int number_arena_9(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_PERFECT));
    }
    static int number_arena_10(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_GOOD));
    }
    static int number_arena_11(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BAD));
    }
    static int number_arena_12(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_POOR));
    }
    static int number_arena_13(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_KPOOR));
    }
    static int number_arena_14(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_MISS));
    }
    static int number_arena_15(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BP));
    }
    static int number_arena_16(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_CB));
    }
    static int number_arena_17(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto r = ArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().health);
    }
    static int number_arena_18(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        int score = (int)rbms->getExScore();
        int max = rbms->getMaxScore();
        int section = int(double(score) / max * 9.0);
        int nextRankSection = section == 0 ? 2 : section == 9 ? 9 : section + 1;
        return score - (max * nextRankSection / 9);
    }
    static int number_arena_19(int player)
    {
        if (ArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        auto r1 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        return r1 ? int(r1->getMaxScore() - rbms->getMaxScore()) : -int(rbms->getMaxScore());
    }

    static int number_0() { return 0; }

    static int number_10() { return int(PlayData.player[PLAYER_SLOT_PLAYER].hispeed * 100); }
    static int number_11() { return int(PlayData.player[PLAYER_SLOT_TARGET].hispeed * 100); }
    static int number_12() { return PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual; }
    static int number_13() { return PlayData.targetRate; }
    static int number_14() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop / 10; }
    static int number_15() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom / 10; }
    static int number_20() { return (int)SystemData.currentRenderFPS; }
    static int number_21() { return (int)SystemData.dateYear; }
    static int number_22() { return (int)SystemData.dateMonthOfYear; }
    static int number_23() { return (int)SystemData.dateDayOfMonth; }
    static int number_24() { return (int)SystemData.timeHour; }
    static int number_25() { return (int)SystemData.timeMin; }
    static int number_26() { return (int)SystemData.timeSec; }
    static int number_30() { return ProfileData.playCount; }
    static int number_31() { return ProfileData.clearCount; }
    static int number_32() { return ProfileData.failCount; }
    static int number_33() { return 1234; }
    static int number_34() { return 5678; }
    static int number_35() { return 111; }
    static int number_36() { return 22; }
    static int number_37() { return 3; }
    static int number_38() { return 0; }
    static int number_39() { return 3076; }
    static int number_40() { return 999; }
    static int number_41() { return 0; }

    static int number_45() { return -1; }
    static int number_46() { return -1; }
    static int number_47() { return -1; }
    static int number_48() { return -1; }
    static int number_49() { return -1; }

    static int number_50() { return SystemData.equalizerVal62_5hz; }
    static int number_51() { return SystemData.equalizerVal160hz; }
    static int number_52() { return SystemData.equalizerVal400hz; }
    static int number_53() { return SystemData.equalizerVal1khz; }
    static int number_54() { return SystemData.equalizerVal2_5khz; }
    static int number_55() { return SystemData.equalizerVal6_25khz; }
    static int number_56() { return SystemData.equalizerVal16khz; }
    static int number_57() { return int(SystemData.volumeMaster * 100); }
    static int number_58() { return int(SystemData.volumeKey * 100); }
    static int number_59() { return int(SystemData.volumeBgm * 100); }
    static int number_60() { return int(SystemData.fxVal * 100); }
    static int number_66() { return int(SystemData.freqVal * 100); }

    static std::shared_ptr<EntryChart> getCurrentSelectedEntryChart()
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        return std::dynamic_pointer_cast<EntryChart>(SelectData.entries[SelectData.selectedEntryIndex].first);
    }

    static std::shared_ptr<ScoreBase> getCurrentSelectedEntryScore()
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        return SelectData.entries[SelectData.selectedEntryIndex].second;
    }

    static std::shared_ptr<ChartFormatBase> getCurrentSelectedChart()
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        auto p = SelectData.entries[SelectData.selectedEntryIndex].first;
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

    static std::shared_ptr<ChartFormatBMSMeta> getCurrentSelectedChartBMSMeta()
    {
        return std::dynamic_pointer_cast<ChartFormatBMSMeta>(getCurrentSelectedChart());
    }

    static std::shared_ptr<EntryFolderSong> getCurrentSelectedSong()
    {
        auto p = getCurrentSelectedEntryChart();
        if (p == nullptr)
            return nullptr;
        return p->getSongEntry();
    }

    static int number_70() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->score;
        }
        return 0;
    }
    static int number_71() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->exscore;
        }
        return 0;
    }
    static int number_72() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->notes * 2;
        }
        return 0;
    }
    static int number_73() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return int(s->rate);
        }
        return 0;
    }
    static int number_74() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->notes * 2;
        }
        return 0;
    }
    static int number_75() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->maxcombo;
        }
        return 0;
    }
    static int number_76() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->bp;
        }
        return 0;
    }
    static int number_77() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->playcount;
        }
        return 0;
    }
    static int number_78() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->clearcount;
        }
        return 0;
    }
    static int number_79() {
        auto s = getCurrentSelectedEntryScore();
        if (s)
        {
            return s->playcount - s->clearcount;
        }
        return 0;
    }

    static int number_80() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->pgreat;
        }
        return 0;
    }
    static int number_81() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->great;
        }
        return 0;
    }
    static int number_82() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->good;
        }
        return 0;
    }
    static int number_83() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->bad;
        }
        return 0;
    }
    static int number_84() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return s->miss + s->kpoor;
        }
        return 0;
    }
    static int number_85() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return int(double(s->pgreat) / s->notes / 2);
        }
        return 0;
    }
    static int number_86() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return int(double(s->great) / s->notes / 2);
        }
        return 0;
    }
    static int number_87() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return int(double(s->good) / s->notes / 2);
        }
        return 0;
    }
    static int number_88() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return int(double(s->bad) / s->notes / 2);
        }
        return 0;
    }
    static int number_89() {
        auto s = std::dynamic_pointer_cast<ScoreBMS>(getCurrentSelectedEntryScore());
        if (s)
        {
            return int(double(s->kpoor + s->miss) / s->notes / 2);
        }
        return 0;
    }

    static int number_90()
    {
        auto c = getCurrentSelectedChart();
        if (c)
        {
            return int(c->maxBPM);
        }
        return 0;
    }
    static int number_91()
    {
        auto c = getCurrentSelectedChart();
        if (c)
        {
            return int(c->minBPM);
        }
        return 0;
    }

    static int number_100() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getScore();
        }
        return 0;
    }
    static int number_101() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getExScore();
        }
        return 0;
    }
    static int number_102() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().acc);
        }
        return 0;
    }
    static int number_103() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().acc * 100) % 100;
        }
        return 0;
    }
    static int number_104() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            auto& d = r->getData();
            return (int)d.combo + d.comboEx;
        }
        return 0;
    }
    static int number_105() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getData().maxCombo;
        }
        return 0;
    }
    static int number_106() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getNoteCount();
        }
        return 0;
    }
    static int number_107() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().health * 100.0);
        }
        return 0;
    }
    static int number_108() {
        auto r1 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        auto r2 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r1)
        {
            int score1 = (int)r1->getExScore();
            int score2 = r2 ? (int)r2->getExScore() : 0;
            return score1 - score2;
        }
        return 0;
    }
    static int number_109() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            int score = (int)r->getExScore();
            int max = r->getMaxScore();
            int section = int(double(score) / max * 9.0);
            int nextRankSection = section == 0 ? 2 : section == 9 ? 9 : section + 1;
            return score - (max * nextRankSection / 9);
        }
        return 0;
    }
    static int number_110() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
        }
        return 0;
    }
    static int number_111() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GREAT);
        }
        return 0;
    }
    static int number_112() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GOOD);
        }
        return 0;
    }
    static int number_113() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::BAD);
        }
        return 0;
    }
    static int number_114() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::KPOOR) + (int)r->getJudgeCount(RulesetBMS::JudgeType::MISS);
        }
        return 0;
    }
    static int number_115() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getData().total_acc;
        }
        return 0;
    }
    static int number_116() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().total_acc * 100) % 100;
        }
        return 0;
    }
    static int number_120() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getScore();
        }
        return 0;
    }
    static int number_121() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getExScore();
        }
        return 0;
    }
    static int number_122() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().acc);
        }
        return 0;
    }
    static int number_123() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().acc * 100) % 100;
        }
        return 0;
    }
    static int number_124() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            auto& d = r->getData();
            return (int)d.combo + d.comboEx;
        }
        return 0;
    }
    static int number_125() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getData().maxCombo;
        }
        return 0;
    }
    static int number_126() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getNoteCount();
        }
        return 0;
    }
    static int number_127() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().health * 100.0);
        }
        return 0;
    }
    static int number_128() {
        auto r1 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        auto r2 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r1)
        {
            int score1 = (int)r1->getExScore();
            int score2 = r2 ? (int)r2->getExScore() : 0;
            return score1 - score2;
        }
        return 0;
    }
    static int number_129() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            int score = (int)r->getExScore();
            int max = r->getMaxScore();
            int section = int(double(score) / max * 9.0);
            int nextRankSection = section == 0 ? 2 : section == 9 ? 9 : section + 1;
            return score - (max * nextRankSection / 9);
        }
        return 0;
    }
    static int number_130() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
        }
        return 0;
    }
    static int number_131() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GREAT);
        }
        return 0;
    }
    static int number_132() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GOOD);
        }
        return 0;
    }
    static int number_133() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::BAD);
        }
        return 0;
    }
    static int number_134() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_POOR);
        }
        return 0;
    }
    static int number_135() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getData().total_acc;
        }
        return 0;
    }
    static int number_136() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().total_acc * 100) % 100;
        }
        return 0;
    }
    static int number_150() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_MYBEST].ruleset);
        if (r)
        {
            return (int)r->getExScore();
        }
        return 0;
    }
    static int number_151() { return number_121(); }
    static int number_152() {
        auto r1 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        auto r2 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_MYBEST].ruleset);
        if (r1)
        {
            int score1 = (int)r1->getExScore();
            int score2 = r2 ? (int)r2->getExScore() : 0;
            return score1 - score2;
        }
        return 0;
    }
    static int number_153() { return number_108(); }
    static int number_154() { return number_109(); }
    static int number_155() {
        auto r = PlayData.player[PLAYER_SLOT_MYBEST].ruleset;
        if (r)
        {
            return int(r->getData().acc);
        }
        return 0;
    }
    static int number_156() {
        auto r = PlayData.player[PLAYER_SLOT_MYBEST].ruleset;
        if (r)
        {
            return int(r->getData().acc * 100) % 100;
        }
        return 0;
    }
    static int number_157() { return number_122(); }
    static int number_158() { return number_123(); }
    static int number_160() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            return (int)c->getCurrentBPM();
        }
        return 0;
    }
    static int number_161() {
        if (SystemData.gNextScene == SceneType::PLAY && PlayData.playStarted)
            return int((Time().norm() - PlayData.timers["play_start"]) / 1000 / 60);
        return 0;
    }
    static int number_162() {
        if (SystemData.gNextScene == SceneType::PLAY && PlayData.playStarted)
            return int((Time().norm() - PlayData.timers["play_start"]) / 1000 % 60);
        return 0;
    }
    static int number_163() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c && SystemData.gNextScene == SceneType::PLAY)
        {
            if (PlayData.playStarted)
                return int((c->getTotalLength().norm() - (Time().norm() - PlayData.timers["play_start"])) / 1000 / 60);
            else
                return int((c->getTotalLength().norm()) / 1000 / 60);
        }
        return 0;
    }
    static int number_164() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c && SystemData.gNextScene == SceneType::PLAY)
        {
            if (PlayData.playStarted)
                return int((c->getTotalLength().norm() - (Time().norm() - PlayData.timers["play_start"])) / 1000 % 60);
            else
                return int((c->getTotalLength().norm()) / 1000 % 60);
        }
        return 0;
    }
    static int number_165() { return int((PlayData.loadProgressWav + PlayData.loadProgressBga) * 100 / 2); };
    static int number_166() { return 100; };
    static int number_167() { return int(PlayData.loadProgressWav * 100); };
    static int number_168() { return int(PlayData.loadProgressBga * 100); };

    static int number_170() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return score->exscore;
        }
        return 0;
    }
    static int number_171() { return number_101(); }
    static int number_172() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            int mybest = 0;
            auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
            if (c)
            {
                auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
                if (score)
                {
                    mybest = score->exscore;
                }
            }
            return (int)r->getExScore() - mybest;
        }
        return 0;
    }
    static int number_173() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return int(score->maxcombo);
        }
        return 0;
    }
    static int number_174() { return number_105(); }
    static int number_175() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            int mybest = 0;
            auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
            if (c)
            {
                auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
                if (score)
                {
                    mybest = score->maxcombo;
                }
            }
            return (int)r->getData().maxCombo - mybest;
        }
        return 0;
    }
    static int number_176() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return score->bp;
        }
        return 0;
    }
    static int number_177() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BP);
        }
        return 0;
    }
    static int number_178() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            int mybest = 0;
            auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
            if (c)
            {
                auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
                if (score)
                {
                    mybest = score->bp;
                }
            }
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BP) - mybest;
        }
        return 0;
    }
    static int number_183() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return (int)score->rate;
        }
        return 0;
    }
    static int number_184() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return int(score->rate * 100) % 100;
        }
        return 0;
    }
    static int number_201() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getLastJudge(0).time.norm();
        }
        return 0;
    }
    static int number_213() {
        int slot = PLAYER_SLOT_PLAYER;
        int side = 1;
        if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
        {
            slot = PLAYER_SLOT_TARGET;
            side = 0;
        }
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[slot].ruleset);
        if (r)
        {
            return (int)r->getLastJudge(side).time.norm();
        }
        return 0;
    }
    static int number_210() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            int fastslow = 0;   // 1:fast 2:slow
            switch (r->getLastJudge(0).area)
            {
            case RulesetBMS::JudgeArea::EARLY_GREAT:
            case RulesetBMS::JudgeArea::EARLY_GOOD:
            case RulesetBMS::JudgeArea::EARLY_BAD:
            case RulesetBMS::JudgeArea::EARLY_KPOOR:
                fastslow = 1;
                break;

            case RulesetBMS::JudgeArea::LATE_GREAT:
            case RulesetBMS::JudgeArea::LATE_GOOD:
            case RulesetBMS::JudgeArea::LATE_BAD:
            case RulesetBMS::JudgeArea::MISS:
            case RulesetBMS::JudgeArea::LATE_KPOOR:
                fastslow = 2;
                break;
            }
            return fastslow;
        }
        return 0;
    }
    static int number_211() {
        int slot = PLAYER_SLOT_PLAYER;
        int side = 1;
        if (PlayData.battleType == PlayModifierBattleType::LocalBattle)
        {
            slot = PLAYER_SLOT_TARGET;
            side = 0;
        }
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[slot].ruleset);
        if (r)
        {
            int fastslow = 0;   // 1:fast 2:slow
            switch (r->getLastJudge(side).area)
            {
            case RulesetBMS::JudgeArea::EARLY_GREAT:
            case RulesetBMS::JudgeArea::EARLY_GOOD:
            case RulesetBMS::JudgeArea::EARLY_BAD:
            case RulesetBMS::JudgeArea::EARLY_KPOOR:
                fastslow = 1;
                break;

            case RulesetBMS::JudgeArea::LATE_GREAT:
            case RulesetBMS::JudgeArea::LATE_GOOD:
            case RulesetBMS::JudgeArea::LATE_BAD:
            case RulesetBMS::JudgeArea::MISS:
            case RulesetBMS::JudgeArea::LATE_KPOOR:
                fastslow = 2;
                break;
            }
            return fastslow;
        }
        return 0;
    }
    static int number_212() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_EARLY);
        }
        return 0;
    }
    static int number_214() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_LATE);
        }
        return 0;
    }
    static int number_217() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getNoteCount() - r->getNotesExpired());
        }
        return 0;
    }
    static int number_218() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getNotesExpired());
        }
        return 0;
    }
    static int number_250() {
        if (PlayData.courseStageData.size() >= 1)
            return PlayData.courseStageData[0].level;
        return 0;
    }
    static int number_251() {
        if (PlayData.courseStageData.size() >= 2)
            return PlayData.courseStageData[1].level;
        return 0;
    }
    static int number_252() {
        if (PlayData.courseStageData.size() >= 3)
            return PlayData.courseStageData[2].level;
        return 0;
    }
    static int number_253() {
        if (PlayData.courseStageData.size() >= 4)
            return PlayData.courseStageData[3].level;
        return 0;
    }
    static int number_254() {
        if (PlayData.courseStageData.size() >= 5)
            return PlayData.courseStageData[4].level;
        return 0;
    }
    static int number_301() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return f->total;
    }

    static int number_302() { return int(PlayData.player[PLAYER_SLOT_PLAYER].greenNumber); };
    static int number_304() { return int(PlayData.player[PLAYER_SLOT_PLAYER].greenNumberMinBPM); };
    static int number_305() { return int(PlayData.player[PLAYER_SLOT_PLAYER].greenNumberMaxBPM); };
    static int number_303() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop; };
    static int number_306() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom; };

    static int number_342() { return int(PlayData.player[PLAYER_SLOT_TARGET].greenNumber); };
    static int number_344() { return int(PlayData.player[PLAYER_SLOT_TARGET].greenNumberMinBPM); };
    static int number_345() { return int(PlayData.player[PLAYER_SLOT_TARGET].greenNumberMaxBPM); };
    static int number_343() { return PlayData.player[PLAYER_SLOT_TARGET].lanecoverTop; };
    static int number_346() { return PlayData.player[PLAYER_SLOT_TARGET].lanecoverBottom; };

    /*

        PLAY_3COL_JUDGE_COMBO_LEFT = 310,
        PLAY_3COL_JUDGE_COMBO_MID,
        PLAY_3COL_JUDGE_COMBO_RIGHT,
        PLAY_3COL_JUDGE_FAST_SLOW_LEFT,	// 1:FAST 2:SLOW
        PLAY_3COL_JUDGE_FAST_SLOW_MID,
        PLAY_3COL_JUDGE_FAST_SLOW_RIGHT,

        HS_FIX_BPM = 316,	// these 4 are not implemented
        HS_FIX_MINBPM,
        HS_FIX_MAXBPM,
        HS_FIX_AVERAGE_BPM = 319,

        KEY_INDIVIDUAL_JUDGE_1P_SCR = 320,	// How these works are unknown
        KEY_INDIVIDUAL_JUDGE_1P_K1,
        KEY_INDIVIDUAL_JUDGE_1P_K2,
        KEY_INDIVIDUAL_JUDGE_1P_K3,
        KEY_INDIVIDUAL_JUDGE_1P_K4,
        KEY_INDIVIDUAL_JUDGE_1P_K5,
        KEY_INDIVIDUAL_JUDGE_1P_K6,
        KEY_INDIVIDUAL_JUDGE_1P_K7,
        KEY_INDIVIDUAL_JUDGE_1P_K8,
        KEY_INDIVIDUAL_JUDGE_1P_K9,

        KEY_INDIVIDUAL_JUDGE_2P_SCR = 330,
        KEY_INDIVIDUAL_JUDGE_2P_K1,
        KEY_INDIVIDUAL_JUDGE_2P_K2,
        KEY_INDIVIDUAL_JUDGE_2P_K3,
        KEY_INDIVIDUAL_JUDGE_2P_K4,
        KEY_INDIVIDUAL_JUDGE_2P_K5,
        KEY_INDIVIDUAL_JUDGE_2P_K6,
        KEY_INDIVIDUAL_JUDGE_2P_K7,
        KEY_INDIVIDUAL_JUDGE_2P_K8,
        KEY_INDIVIDUAL_JUDGE_2P_K9,

    */

    static int number_350() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_key;
    }
    static int number_351() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)(f->notes_key_ln + f->notes_scratch_ln);
    }
    static int number_352() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_scratch;
    }
    static int number_353() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_scratch_ln;
    }
    static int number_354() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_mine;
    }

    static int number_360() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_MISS);
        }
        return 0;
    }
    static int number_361() { return number_212(); }
    static int number_362() { return number_214(); }
    static int number_363() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_KPOOR);
        }
        return 0;
    }
    static int number_364() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_CB);
        }
        return 0;
    }
    static int number_370() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_MISS);
        }
        return 0;
    }
    static int number_371() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_EARLY);
        }
        return 0;
    }
    static int number_372() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_LATE);
        }
        return 0;
    }
    static int number_373() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_KPOOR);
        }
        return 0;
    }
    static int number_374() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_CB);
        }
        return 0;
    }

    static int number_380() { return int((360 + int(PlayData.scratchAxis[0]) % 360) % 360 / 360.0 * 256); };
    static int number_381() { return int((360 + int(PlayData.scratchAxis[1]) % 360) % 360 / 360.0 * 256); };

};

#pragma region declare_member

declare_member(NumberConverter, int, number, 0);
declare_member(NumberConverter, int, number, 1);
declare_member(NumberConverter, int, number, 2);
declare_member(NumberConverter, int, number, 3);
declare_member(NumberConverter, int, number, 4);
declare_member(NumberConverter, int, number, 5);
declare_member(NumberConverter, int, number, 6);
declare_member(NumberConverter, int, number, 7);
declare_member(NumberConverter, int, number, 8);
declare_member(NumberConverter, int, number, 9);
declare_member(NumberConverter, int, number, 10);
declare_member(NumberConverter, int, number, 11);
declare_member(NumberConverter, int, number, 12);
declare_member(NumberConverter, int, number, 13);
declare_member(NumberConverter, int, number, 14);
declare_member(NumberConverter, int, number, 15);
declare_member(NumberConverter, int, number, 16);
declare_member(NumberConverter, int, number, 17);
declare_member(NumberConverter, int, number, 18);
declare_member(NumberConverter, int, number, 19);
declare_member(NumberConverter, int, number, 20);
declare_member(NumberConverter, int, number, 21);
declare_member(NumberConverter, int, number, 22);
declare_member(NumberConverter, int, number, 23);
declare_member(NumberConverter, int, number, 24);
declare_member(NumberConverter, int, number, 25);
declare_member(NumberConverter, int, number, 26);
declare_member(NumberConverter, int, number, 27);
declare_member(NumberConverter, int, number, 28);
declare_member(NumberConverter, int, number, 29);
declare_member(NumberConverter, int, number, 30);
declare_member(NumberConverter, int, number, 31);
declare_member(NumberConverter, int, number, 32);
declare_member(NumberConverter, int, number, 33);
declare_member(NumberConverter, int, number, 34);
declare_member(NumberConverter, int, number, 35);
declare_member(NumberConverter, int, number, 36);
declare_member(NumberConverter, int, number, 37);
declare_member(NumberConverter, int, number, 38);
declare_member(NumberConverter, int, number, 39);
declare_member(NumberConverter, int, number, 40);
declare_member(NumberConverter, int, number, 41);
declare_member(NumberConverter, int, number, 42);
declare_member(NumberConverter, int, number, 43);
declare_member(NumberConverter, int, number, 44);
declare_member(NumberConverter, int, number, 45);
declare_member(NumberConverter, int, number, 46);
declare_member(NumberConverter, int, number, 47);
declare_member(NumberConverter, int, number, 48);
declare_member(NumberConverter, int, number, 49);
declare_member(NumberConverter, int, number, 50);
declare_member(NumberConverter, int, number, 51);
declare_member(NumberConverter, int, number, 52);
declare_member(NumberConverter, int, number, 53);
declare_member(NumberConverter, int, number, 54);
declare_member(NumberConverter, int, number, 55);
declare_member(NumberConverter, int, number, 56);
declare_member(NumberConverter, int, number, 57);
declare_member(NumberConverter, int, number, 58);
declare_member(NumberConverter, int, number, 59);
declare_member(NumberConverter, int, number, 60);
declare_member(NumberConverter, int, number, 61);
declare_member(NumberConverter, int, number, 62);
declare_member(NumberConverter, int, number, 63);
declare_member(NumberConverter, int, number, 64);
declare_member(NumberConverter, int, number, 65);
declare_member(NumberConverter, int, number, 66);
declare_member(NumberConverter, int, number, 67);
declare_member(NumberConverter, int, number, 68);
declare_member(NumberConverter, int, number, 69);
declare_member(NumberConverter, int, number, 70);
declare_member(NumberConverter, int, number, 71);
declare_member(NumberConverter, int, number, 72);
declare_member(NumberConverter, int, number, 73);
declare_member(NumberConverter, int, number, 74);
declare_member(NumberConverter, int, number, 75);
declare_member(NumberConverter, int, number, 76);
declare_member(NumberConverter, int, number, 77);
declare_member(NumberConverter, int, number, 78);
declare_member(NumberConverter, int, number, 79);
declare_member(NumberConverter, int, number, 80);
declare_member(NumberConverter, int, number, 81);
declare_member(NumberConverter, int, number, 82);
declare_member(NumberConverter, int, number, 83);
declare_member(NumberConverter, int, number, 84);
declare_member(NumberConverter, int, number, 85);
declare_member(NumberConverter, int, number, 86);
declare_member(NumberConverter, int, number, 87);
declare_member(NumberConverter, int, number, 88);
declare_member(NumberConverter, int, number, 89);
declare_member(NumberConverter, int, number, 90);
declare_member(NumberConverter, int, number, 91);
declare_member(NumberConverter, int, number, 92);
declare_member(NumberConverter, int, number, 93);
declare_member(NumberConverter, int, number, 94);
declare_member(NumberConverter, int, number, 95);
declare_member(NumberConverter, int, number, 96);
declare_member(NumberConverter, int, number, 97);
declare_member(NumberConverter, int, number, 98);
declare_member(NumberConverter, int, number, 99);
declare_member(NumberConverter, int, number, 100);
declare_member(NumberConverter, int, number, 101);
declare_member(NumberConverter, int, number, 102);
declare_member(NumberConverter, int, number, 103);
declare_member(NumberConverter, int, number, 104);
declare_member(NumberConverter, int, number, 105);
declare_member(NumberConverter, int, number, 106);
declare_member(NumberConverter, int, number, 107);
declare_member(NumberConverter, int, number, 108);
declare_member(NumberConverter, int, number, 109);
declare_member(NumberConverter, int, number, 110);
declare_member(NumberConverter, int, number, 111);
declare_member(NumberConverter, int, number, 112);
declare_member(NumberConverter, int, number, 113);
declare_member(NumberConverter, int, number, 114);
declare_member(NumberConverter, int, number, 115);
declare_member(NumberConverter, int, number, 116);
declare_member(NumberConverter, int, number, 117);
declare_member(NumberConverter, int, number, 118);
declare_member(NumberConverter, int, number, 119);
declare_member(NumberConverter, int, number, 120);
declare_member(NumberConverter, int, number, 121);
declare_member(NumberConverter, int, number, 122);
declare_member(NumberConverter, int, number, 123);
declare_member(NumberConverter, int, number, 124);
declare_member(NumberConverter, int, number, 125);
declare_member(NumberConverter, int, number, 126);
declare_member(NumberConverter, int, number, 127);
declare_member(NumberConverter, int, number, 128);
declare_member(NumberConverter, int, number, 129);
declare_member(NumberConverter, int, number, 130);
declare_member(NumberConverter, int, number, 131);
declare_member(NumberConverter, int, number, 132);
declare_member(NumberConverter, int, number, 133);
declare_member(NumberConverter, int, number, 134);
declare_member(NumberConverter, int, number, 135);
declare_member(NumberConverter, int, number, 136);
declare_member(NumberConverter, int, number, 137);
declare_member(NumberConverter, int, number, 138);
declare_member(NumberConverter, int, number, 139);
declare_member(NumberConverter, int, number, 140);
declare_member(NumberConverter, int, number, 141);
declare_member(NumberConverter, int, number, 142);
declare_member(NumberConverter, int, number, 143);
declare_member(NumberConverter, int, number, 144);
declare_member(NumberConverter, int, number, 145);
declare_member(NumberConverter, int, number, 146);
declare_member(NumberConverter, int, number, 147);
declare_member(NumberConverter, int, number, 148);
declare_member(NumberConverter, int, number, 149);
declare_member(NumberConverter, int, number, 150);
declare_member(NumberConverter, int, number, 151);
declare_member(NumberConverter, int, number, 152);
declare_member(NumberConverter, int, number, 153);
declare_member(NumberConverter, int, number, 154);
declare_member(NumberConverter, int, number, 155);
declare_member(NumberConverter, int, number, 156);
declare_member(NumberConverter, int, number, 157);
declare_member(NumberConverter, int, number, 158);
declare_member(NumberConverter, int, number, 159);
declare_member(NumberConverter, int, number, 160);
declare_member(NumberConverter, int, number, 161);
declare_member(NumberConverter, int, number, 162);
declare_member(NumberConverter, int, number, 163);
declare_member(NumberConverter, int, number, 164);
declare_member(NumberConverter, int, number, 165);
declare_member(NumberConverter, int, number, 166);
declare_member(NumberConverter, int, number, 167);
declare_member(NumberConverter, int, number, 168);
declare_member(NumberConverter, int, number, 169);
declare_member(NumberConverter, int, number, 170);
declare_member(NumberConverter, int, number, 171);
declare_member(NumberConverter, int, number, 172);
declare_member(NumberConverter, int, number, 173);
declare_member(NumberConverter, int, number, 174);
declare_member(NumberConverter, int, number, 175);
declare_member(NumberConverter, int, number, 176);
declare_member(NumberConverter, int, number, 177);
declare_member(NumberConverter, int, number, 178);
declare_member(NumberConverter, int, number, 179);
declare_member(NumberConverter, int, number, 180);
declare_member(NumberConverter, int, number, 181);
declare_member(NumberConverter, int, number, 182);
declare_member(NumberConverter, int, number, 183);
declare_member(NumberConverter, int, number, 184);
declare_member(NumberConverter, int, number, 185);
declare_member(NumberConverter, int, number, 186);
declare_member(NumberConverter, int, number, 187);
declare_member(NumberConverter, int, number, 188);
declare_member(NumberConverter, int, number, 189);
declare_member(NumberConverter, int, number, 190);
declare_member(NumberConverter, int, number, 191);
declare_member(NumberConverter, int, number, 192);
declare_member(NumberConverter, int, number, 193);
declare_member(NumberConverter, int, number, 194);
declare_member(NumberConverter, int, number, 195);
declare_member(NumberConverter, int, number, 196);
declare_member(NumberConverter, int, number, 197);
declare_member(NumberConverter, int, number, 198);
declare_member(NumberConverter, int, number, 199);
declare_member(NumberConverter, int, number, 200);
declare_member(NumberConverter, int, number, 201);
declare_member(NumberConverter, int, number, 202);
declare_member(NumberConverter, int, number, 203);
declare_member(NumberConverter, int, number, 204);
declare_member(NumberConverter, int, number, 205);
declare_member(NumberConverter, int, number, 206);
declare_member(NumberConverter, int, number, 207);
declare_member(NumberConverter, int, number, 208);
declare_member(NumberConverter, int, number, 209);
declare_member(NumberConverter, int, number, 210);
declare_member(NumberConverter, int, number, 211);
declare_member(NumberConverter, int, number, 212);
declare_member(NumberConverter, int, number, 213);
declare_member(NumberConverter, int, number, 214);
declare_member(NumberConverter, int, number, 215);
declare_member(NumberConverter, int, number, 216);
declare_member(NumberConverter, int, number, 217);
declare_member(NumberConverter, int, number, 218);
declare_member(NumberConverter, int, number, 219);
declare_member(NumberConverter, int, number, 220);
declare_member(NumberConverter, int, number, 221);
declare_member(NumberConverter, int, number, 222);
declare_member(NumberConverter, int, number, 223);
declare_member(NumberConverter, int, number, 224);
declare_member(NumberConverter, int, number, 225);
declare_member(NumberConverter, int, number, 226);
declare_member(NumberConverter, int, number, 227);
declare_member(NumberConverter, int, number, 228);
declare_member(NumberConverter, int, number, 229);
declare_member(NumberConverter, int, number, 230);
declare_member(NumberConverter, int, number, 231);
declare_member(NumberConverter, int, number, 232);
declare_member(NumberConverter, int, number, 233);
declare_member(NumberConverter, int, number, 234);
declare_member(NumberConverter, int, number, 235);
declare_member(NumberConverter, int, number, 236);
declare_member(NumberConverter, int, number, 237);
declare_member(NumberConverter, int, number, 238);
declare_member(NumberConverter, int, number, 239);
declare_member(NumberConverter, int, number, 240);
declare_member(NumberConverter, int, number, 241);
declare_member(NumberConverter, int, number, 242);
declare_member(NumberConverter, int, number, 243);
declare_member(NumberConverter, int, number, 244);
declare_member(NumberConverter, int, number, 245);
declare_member(NumberConverter, int, number, 246);
declare_member(NumberConverter, int, number, 247);
declare_member(NumberConverter, int, number, 248);
declare_member(NumberConverter, int, number, 249);
declare_member(NumberConverter, int, number, 250);
declare_member(NumberConverter, int, number, 251);
declare_member(NumberConverter, int, number, 252);
declare_member(NumberConverter, int, number, 253);
declare_member(NumberConverter, int, number, 254);
declare_member(NumberConverter, int, number, 255);
declare_member(NumberConverter, int, number, 256);
declare_member(NumberConverter, int, number, 257);
declare_member(NumberConverter, int, number, 258);
declare_member(NumberConverter, int, number, 259);
declare_member(NumberConverter, int, number, 260);
declare_member(NumberConverter, int, number, 261);
declare_member(NumberConverter, int, number, 262);
declare_member(NumberConverter, int, number, 263);
declare_member(NumberConverter, int, number, 264);
declare_member(NumberConverter, int, number, 265);
declare_member(NumberConverter, int, number, 266);
declare_member(NumberConverter, int, number, 267);
declare_member(NumberConverter, int, number, 268);
declare_member(NumberConverter, int, number, 269);
declare_member(NumberConverter, int, number, 270);
declare_member(NumberConverter, int, number, 271);
declare_member(NumberConverter, int, number, 272);
declare_member(NumberConverter, int, number, 273);
declare_member(NumberConverter, int, number, 274);
declare_member(NumberConverter, int, number, 275);
declare_member(NumberConverter, int, number, 276);
declare_member(NumberConverter, int, number, 277);
declare_member(NumberConverter, int, number, 278);
declare_member(NumberConverter, int, number, 279);
declare_member(NumberConverter, int, number, 280);
declare_member(NumberConverter, int, number, 281);
declare_member(NumberConverter, int, number, 282);
declare_member(NumberConverter, int, number, 283);
declare_member(NumberConverter, int, number, 284);
declare_member(NumberConverter, int, number, 285);
declare_member(NumberConverter, int, number, 286);
declare_member(NumberConverter, int, number, 287);
declare_member(NumberConverter, int, number, 288);
declare_member(NumberConverter, int, number, 289);
declare_member(NumberConverter, int, number, 290);
declare_member(NumberConverter, int, number, 291);
declare_member(NumberConverter, int, number, 292);
declare_member(NumberConverter, int, number, 293);
declare_member(NumberConverter, int, number, 294);
declare_member(NumberConverter, int, number, 295);
declare_member(NumberConverter, int, number, 296);
declare_member(NumberConverter, int, number, 297);
declare_member(NumberConverter, int, number, 298);
declare_member(NumberConverter, int, number, 299);
declare_member(NumberConverter, int, number, 300);
declare_member(NumberConverter, int, number, 301);
declare_member(NumberConverter, int, number, 302);
declare_member(NumberConverter, int, number, 303);
declare_member(NumberConverter, int, number, 304);
declare_member(NumberConverter, int, number, 305);
declare_member(NumberConverter, int, number, 306);
declare_member(NumberConverter, int, number, 307);
declare_member(NumberConverter, int, number, 308);
declare_member(NumberConverter, int, number, 309);
declare_member(NumberConverter, int, number, 310);
declare_member(NumberConverter, int, number, 311);
declare_member(NumberConverter, int, number, 312);
declare_member(NumberConverter, int, number, 313);
declare_member(NumberConverter, int, number, 314);
declare_member(NumberConverter, int, number, 315);
declare_member(NumberConverter, int, number, 316);
declare_member(NumberConverter, int, number, 317);
declare_member(NumberConverter, int, number, 318);
declare_member(NumberConverter, int, number, 319);
declare_member(NumberConverter, int, number, 320);
declare_member(NumberConverter, int, number, 321);
declare_member(NumberConverter, int, number, 322);
declare_member(NumberConverter, int, number, 323);
declare_member(NumberConverter, int, number, 324);
declare_member(NumberConverter, int, number, 325);
declare_member(NumberConverter, int, number, 326);
declare_member(NumberConverter, int, number, 327);
declare_member(NumberConverter, int, number, 328);
declare_member(NumberConverter, int, number, 329);
declare_member(NumberConverter, int, number, 330);
declare_member(NumberConverter, int, number, 331);
declare_member(NumberConverter, int, number, 332);
declare_member(NumberConverter, int, number, 333);
declare_member(NumberConverter, int, number, 334);
declare_member(NumberConverter, int, number, 335);
declare_member(NumberConverter, int, number, 336);
declare_member(NumberConverter, int, number, 337);
declare_member(NumberConverter, int, number, 338);
declare_member(NumberConverter, int, number, 339);
declare_member(NumberConverter, int, number, 340);
declare_member(NumberConverter, int, number, 341);
declare_member(NumberConverter, int, number, 342);
declare_member(NumberConverter, int, number, 343);
declare_member(NumberConverter, int, number, 344);
declare_member(NumberConverter, int, number, 345);
declare_member(NumberConverter, int, number, 346);
declare_member(NumberConverter, int, number, 347);
declare_member(NumberConverter, int, number, 348);
declare_member(NumberConverter, int, number, 349);
declare_member(NumberConverter, int, number, 350);
declare_member(NumberConverter, int, number, 351);
declare_member(NumberConverter, int, number, 352);
declare_member(NumberConverter, int, number, 353);
declare_member(NumberConverter, int, number, 354);
declare_member(NumberConverter, int, number, 355);
declare_member(NumberConverter, int, number, 356);
declare_member(NumberConverter, int, number, 357);
declare_member(NumberConverter, int, number, 358);
declare_member(NumberConverter, int, number, 359);
declare_member(NumberConverter, int, number, 360);
declare_member(NumberConverter, int, number, 361);
declare_member(NumberConverter, int, number, 362);
declare_member(NumberConverter, int, number, 363);
declare_member(NumberConverter, int, number, 364);
declare_member(NumberConverter, int, number, 365);
declare_member(NumberConverter, int, number, 366);
declare_member(NumberConverter, int, number, 367);
declare_member(NumberConverter, int, number, 368);
declare_member(NumberConverter, int, number, 369);
declare_member(NumberConverter, int, number, 370);
declare_member(NumberConverter, int, number, 371);
declare_member(NumberConverter, int, number, 372);
declare_member(NumberConverter, int, number, 373);
declare_member(NumberConverter, int, number, 374);
declare_member(NumberConverter, int, number, 375);
declare_member(NumberConverter, int, number, 376);
declare_member(NumberConverter, int, number, 377);
declare_member(NumberConverter, int, number, 378);
declare_member(NumberConverter, int, number, 379);
declare_member(NumberConverter, int, number, 380);
declare_member(NumberConverter, int, number, 381);
declare_member(NumberConverter, int, number, 382);
declare_member(NumberConverter, int, number, 383);
declare_member(NumberConverter, int, number, 384);
declare_member(NumberConverter, int, number, 385);
declare_member(NumberConverter, int, number, 386);
declare_member(NumberConverter, int, number, 387);
declare_member(NumberConverter, int, number, 388);
declare_member(NumberConverter, int, number, 389);
declare_member(NumberConverter, int, number, 390);
declare_member(NumberConverter, int, number, 391);
declare_member(NumberConverter, int, number, 392);
declare_member(NumberConverter, int, number, 393);
declare_member(NumberConverter, int, number, 394);
declare_member(NumberConverter, int, number, 395);
declare_member(NumberConverter, int, number, 396);
declare_member(NumberConverter, int, number, 397);
declare_member(NumberConverter, int, number, 398);
declare_member(NumberConverter, int, number, 399);

#define number(index) member(NumberConverter, int, number, index)

#pragma endregion

namespace lr2skin
{

std::function<int()> convertNumberIndex(int n)
{
    if (n < 0)
    {
        switch (n)
        {
        case -1: return [] { return PlayData.displayCombo[0]; };
        case -2: return [] { return PlayData.displayCombo[1]; };
        case -3: return [] { return SystemData.scratchAxisValue[0]; };
        case -4: return [] { return SystemData.scratchAxisValue[1]; };
        }
    }
    else if (n >= 450 && n <= 499)
    {
        // ARENA: Exscore difference to Top
    }
    else if (n >= 500 && n <= 739)
    {
        int player = (n - 500) / 30;
        int index = (n - 500) % 30;

        switch (index)
        {
        case 0: return std::bind(NumberConverter::number_arena_0, player);
        case 1: return std::bind(NumberConverter::number_arena_1, player);
        case 2: return std::bind(NumberConverter::number_arena_2, player);
        case 3: return std::bind(NumberConverter::number_arena_3, player);
        case 4: return std::bind(NumberConverter::number_arena_4, player);
        case 5: return std::bind(NumberConverter::number_arena_5, player);
        case 6: return std::bind(NumberConverter::number_arena_6, player);
        case 7: return std::bind(NumberConverter::number_arena_7, player);
        case 8: return std::bind(NumberConverter::number_arena_8, player);
        case 9: return std::bind(NumberConverter::number_arena_9, player);
        case 10: return std::bind(NumberConverter::number_arena_10, player);
        case 11: return std::bind(NumberConverter::number_arena_11, player);
        case 12: return std::bind(NumberConverter::number_arena_12, player);
        case 13: return std::bind(NumberConverter::number_arena_13, player);
        case 14: return std::bind(NumberConverter::number_arena_14, player);
        case 15: return std::bind(NumberConverter::number_arena_15, player);
        case 16: return std::bind(NumberConverter::number_arena_16, player);
        case 17: return std::bind(NumberConverter::number_arena_17, player);
        case 18: return std::bind(NumberConverter::number_arena_18, player);
        case 19: return std::bind(NumberConverter::number_arena_19, player);
        }
    }
    else if (n >= 0 && n <= 399)
    {
        static constexpr int(*kv[400])() =
        {
            number(0),
            number(1),
            number(2),
            number(3),
            number(4),
            number(5),
            number(6),
            number(7),
            number(8),
            number(9),
            number(10),
            number(11),
            number(12),
            number(13),
            number(14),
            number(15),
            number(16),
            number(17),
            number(18),
            number(19),
            number(20),
            number(21),
            number(22),
            number(23),
            number(24),
            number(25),
            number(26),
            number(27),
            number(28),
            number(29),
            number(30),
            number(31),
            number(32),
            number(33),
            number(34),
            number(35),
            number(36),
            number(37),
            number(38),
            number(39),
            number(40),
            number(41),
            number(42),
            number(43),
            number(44),
            number(45),
            number(46),
            number(47),
            number(48),
            number(49),
            number(50),
            number(51),
            number(52),
            number(53),
            number(54),
            number(55),
            number(56),
            number(57),
            number(58),
            number(59),
            number(60),
            number(61),
            number(62),
            number(63),
            number(64),
            number(65),
            number(66),
            number(67),
            number(68),
            number(69),
            number(70),
            number(71),
            number(72),
            number(73),
            number(74),
            number(75),
            number(76),
            number(77),
            number(78),
            number(79),
            number(80),
            number(81),
            number(82),
            number(83),
            number(84),
            number(85),
            number(86),
            number(87),
            number(88),
            number(89),
            number(90),
            number(91),
            number(92),
            number(93),
            number(94),
            number(95),
            number(96),
            number(97),
            number(98),
            number(99),
            number(100),
            number(101),
            number(102),
            number(103),
            number(104),
            number(105),
            number(106),
            number(107),
            number(108),
            number(109),
            number(110),
            number(111),
            number(112),
            number(113),
            number(114),
            number(115),
            number(116),
            number(117),
            number(118),
            number(119),
            number(120),
            number(121),
            number(122),
            number(123),
            number(124),
            number(125),
            number(126),
            number(127),
            number(128),
            number(129),
            number(130),
            number(131),
            number(132),
            number(133),
            number(134),
            number(135),
            number(136),
            number(137),
            number(138),
            number(139),
            number(140),
            number(141),
            number(142),
            number(143),
            number(144),
            number(145),
            number(146),
            number(147),
            number(148),
            number(149),
            number(150),
            number(151),
            number(152),
            number(153),
            number(154),
            number(155),
            number(156),
            number(157),
            number(158),
            number(159),
            number(160),
            number(161),
            number(162),
            number(163),
            number(164),
            number(165),
            number(166),
            number(167),
            number(168),
            number(169),
            number(170),
            number(171),
            number(172),
            number(173),
            number(174),
            number(175),
            number(176),
            number(177),
            number(178),
            number(179),
            number(180),
            number(181),
            number(182),
            number(183),
            number(184),
            number(185),
            number(186),
            number(187),
            number(188),
            number(189),
            number(190),
            number(191),
            number(192),
            number(193),
            number(194),
            number(195),
            number(196),
            number(197),
            number(198),
            number(199),
            number(200),
            number(201),
            number(202),
            number(203),
            number(204),
            number(205),
            number(206),
            number(207),
            number(208),
            number(209),
            number(210),
            number(211),
            number(212),
            number(213),
            number(214),
            number(215),
            number(216),
            number(217),
            number(218),
            number(219),
            number(220),
            number(221),
            number(222),
            number(223),
            number(224),
            number(225),
            number(226),
            number(227),
            number(228),
            number(229),
            number(230),
            number(231),
            number(232),
            number(233),
            number(234),
            number(235),
            number(236),
            number(237),
            number(238),
            number(239),
            number(240),
            number(241),
            number(242),
            number(243),
            number(244),
            number(245),
            number(246),
            number(247),
            number(248),
            number(249),
            number(250),
            number(251),
            number(252),
            number(253),
            number(254),
            number(255),
            number(256),
            number(257),
            number(258),
            number(259),
            number(260),
            number(261),
            number(262),
            number(263),
            number(264),
            number(265),
            number(266),
            number(267),
            number(268),
            number(269),
            number(270),
            number(271),
            number(272),
            number(273),
            number(274),
            number(275),
            number(276),
            number(277),
            number(278),
            number(279),
            number(280),
            number(281),
            number(282),
            number(283),
            number(284),
            number(285),
            number(286),
            number(287),
            number(288),
            number(289),
            number(290),
            number(291),
            number(292),
            number(293),
            number(294),
            number(295),
            number(296),
            number(297),
            number(298),
            number(299),
            number(300),
            number(301),
            number(302),
            number(303),
            number(304),
            number(305),
            number(306),
            number(307),
            number(308),
            number(309),
            number(310),
            number(311),
            number(312),
            number(313),
            number(314),
            number(315),
            number(316),
            number(317),
            number(318),
            number(319),
            number(320),
            number(321),
            number(322),
            number(323),
            number(324),
            number(325),
            number(326),
            number(327),
            number(328),
            number(329),
            number(330),
            number(331),
            number(332),
            number(333),
            number(334),
            number(335),
            number(336),
            number(337),
            number(338),
            number(339),
            number(340),
            number(341),
            number(342),
            number(343),
            number(344),
            number(345),
            number(346),
            number(347),
            number(348),
            number(349),
            number(350),
            number(351),
            number(352),
            number(353),
            number(354),
            number(355),
            number(356),
            number(357),
            number(358),
            number(359),
            number(360),
            number(361),
            number(362),
            number(363),
            number(364),
            number(365),
            number(366),
            number(367),
            number(368),
            number(369),
            number(370),
            number(371),
            number(372),
            number(373),
            number(374),
            number(375),
            number(376),
            number(377),
            number(378),
            number(379),
            number(380),
            number(381),
            number(382),
            number(383),
            number(384),
            number(385),
            number(386),
            number(387),
            number(388),
            number(389),
            number(390),
            number(391),
            number(392),
            number(393),
            number(394),
            number(395),
            number(396),
            number(397),
            number(398),
            number(399),
        };

        return kv[n];
    }

    return [] { return 0; };
}

}
}
