#include "common/pch.h"
#include "skin_lr2_converters.h"

#include "common/entry/entry_song.h"
#include "game/chart/chart.h"
#include "game/chart/chart_types.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"
#include "game/arena/arena_data.h"
#include "db/db_score.h"

namespace lv
{

using namespace data;

class NumberConverter
{
public:
    static int num_arena_0(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getScore());
    }
    static int num_arena_1(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getExScore());
    }
    static int num_arena_2(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().combo);
    }
    static int num_arena_3(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().maxCombo);
    }
    static int num_arena_4(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().acc);
    }
    static int num_arena_5(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().acc * 100) % 100;
    }
    static int num_arena_6(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getNoteCount());
    }
    static int num_arena_7(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().total_acc);
    }
    static int num_arena_8(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().total_acc * 100) % 100;
    }
    static int num_arena_9(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_PERFECT));
    }
    static int num_arena_10(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_GOOD));
    }
    static int num_arena_11(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BAD));
    }
    static int num_arena_12(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_POOR));
    }
    static int num_arena_13(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_KPOOR));
    }
    static int num_arena_14(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_MISS));
    }
    static int num_arena_15(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BP));
    }
    static int num_arena_16(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        return int(rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_CB));
    }
    static int num_arena_17(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto r = gArenaData.getPlayerRuleset(player);
        if (!r) return 0;
        return int(r->getData().health);
    }
    static int num_arena_18(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        int score = (int)rbms->getExScore();
        int max = rbms->getMaxScore();
        int section = int(double(score) / max * 9.0);
        int nextRankSection = section == 0 ? 2 : section == 9 ? 9 : section + 1;
        return score - (max * nextRankSection / 9);
    }
    static int num_arena_19(int player)
    {
        if (gArenaData.isOnline()) return 0;
        auto rbms = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(player));
        if (!rbms) return 0;
        auto r1 = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        return r1 ? int(r1->getMaxScore() - rbms->getMaxScore()) : -int(rbms->getMaxScore());
    }

    static int num_0() { return 0; }

    static int num_10() { return int(PlayData.player[PLAYER_SLOT_PLAYER].hispeed * 100); }
    static int num_11() { return int(PlayData.player[PLAYER_SLOT_TARGET].hispeed * 100); }
    static int num_12() { return PlayData.player[PLAYER_SLOT_PLAYER].offsetVisual; }
    static int num_13() { return PlayData.targetRate; }
    static int num_14() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop / 10; }
    static int num_15() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom / 10; }
    static int num_20() { return (int)SystemData.currentRenderFPS; }
    static int num_21() { return (int)SystemData.dateYear; }
    static int num_22() { return (int)SystemData.dateMonthOfYear; }
    static int num_23() { return (int)SystemData.dateDayOfMonth; }
    static int num_24() { return (int)SystemData.timeHour; }
    static int num_25() { return (int)SystemData.timeMin; }
    static int num_26() { return (int)SystemData.timeSec; }
    static int num_30() { return ProfileData.playCount; }
    static int num_31() { return ProfileData.clearCount; }
    static int num_32() { return ProfileData.failCount; }
    static int num_33() { return 1234; }
    static int num_34() { return 5678; }
    static int num_35() { return 111; }
    static int num_36() { return 22; }
    static int num_37() { return 3; }
    static int num_38() { return 0; }
    static int num_39() { return 3076; }
    static int num_40() { return 999; }
    static int num_41() { return 0; }

    static int num_45() { return -1; }
    static int num_46() { return -1; }
    static int num_47() { return -1; }
    static int num_48() { return -1; }
    static int num_49() { return -1; }

    static int num_50() { return SystemData.equalizerVal62_5hz; }
    static int num_51() { return SystemData.equalizerVal160hz; }
    static int num_52() { return SystemData.equalizerVal400hz; }
    static int num_53() { return SystemData.equalizerVal1khz; }
    static int num_54() { return SystemData.equalizerVal2_5khz; }
    static int num_55() { return SystemData.equalizerVal6_25khz; }
    static int num_56() { return SystemData.equalizerVal16khz; }
    static int num_57() { return int(SystemData.volumeMaster * 100); }
    static int num_58() { return int(SystemData.volumeKey * 100); }
    static int num_59() { return int(SystemData.volumeBgm * 100); }
    static int num_60() { return int(SystemData.fxVal * 100); }
    static int num_66() { return int(SystemData.freqVal * 100); }

    static std::shared_ptr<EntryChart> getCurrentSelectedEntryChart()
    {
        if (SelectData.entries.empty())
            return nullptr;
        if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
            return nullptr;
        return std::dynamic_pointer_cast<EntryChart>(SelectData.entries[SelectData.selectedEntryIndex].first);
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


    static int num_100() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getScore();
        }
        return 0;
    }
    static int num_101() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getExScore();
        }
        return 0;
    }
    static int num_102() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().acc);
        }
        return 0;
    }
    static int num_103() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().acc * 100) % 100;
        }
        return 0;
    }
    static int num_104() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getData().combo;
        }
        return 0;
    }
    static int num_105() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getData().maxCombo;
        }
        return 0;
    }
    static int num_106() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getNoteCount();
        }
        return 0;
    }
    static int num_107() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().health * 100.0);
        }
        return 0;
    }
    static int num_108() {
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
    static int num_109() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            int score = (int)r->getExScore();
            int max = r->getMaxScore();
            int section = int(double(score) / max * 9.0);
            int nextRankSection = section == 0 ? 2 : section == 9 ? 9 : section + 1;
            return score - (max * nextRankSection / 9);
        }
    }
    static int num_110() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
        }
        return 0;
    }
    static int num_111() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GREAT);
        }
        return 0;
    }
    static int num_112() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GOOD);
        }
        return 0;
    }
    static int num_113() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::BAD);
        }
        return 0;
    }
    static int num_114() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::KPOOR) + (int)r->getJudgeCount(RulesetBMS::JudgeType::MISS);
        }
        return 0;
    }
    static int num_115() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return (int)r->getData().total_acc;
        }
        return 0;
    }
    static int num_116() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getData().total_acc * 100) % 100;
        }
        return 0;
    }
    static int num_120() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getScore();
        }
        return 0;
    }
    static int num_121() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getExScore();
        }
        return 0;
    }
    static int num_122() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().acc);
        }
        return 0;
    }
    static int num_123() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().acc * 100) % 100;
        }
        return 0;
    }
    static int num_124() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getData().combo;
        }
        return 0;
    }
    static int num_125() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getData().maxCombo;
        }
        return 0;
    }
    static int num_126() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getNoteCount();
        }
        return 0;
    }
    static int num_127() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().health * 100.0);
        }
        return 0;
    }
    static int num_128() {
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
    static int num_129() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            int score = (int)r->getExScore();
            int max = r->getMaxScore();
            int section = int(double(score) / max * 9.0);
            int nextRankSection = section == 0 ? 2 : section == 9 ? 9 : section + 1;
            return score - (max * nextRankSection / 9);
        }
    }
    static int num_130() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
        }
        return 0;
    }
    static int num_131() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GREAT);
        }
        return 0;
    }
    static int num_132() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::GOOD);
        }
        return 0;
    }
    static int num_133() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCount(RulesetBMS::JudgeType::BAD);
        }
        return 0;
    }
    static int num_134() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_POOR);
        }
        return 0;
    }
    static int num_135() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return (int)r->getData().total_acc;
        }
        return 0;
    }
    static int num_136() {
        auto r = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
        if (r)
        {
            return int(r->getData().total_acc * 100) % 100;
        }
        return 0;
    }
    static int num_150() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_MYBEST].ruleset);
        if (r)
        {
            return (int)r->getExScore();
        }
        return 0;
    }
    static constexpr int (*num_151)() = num_121;
    static int num_152() {
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
    static constexpr int (*num_153)() = num_108;
    static constexpr int (*num_154)() = num_109;
    static int num_155() {
        auto r = PlayData.player[PLAYER_SLOT_MYBEST].ruleset;
        if (r)
        {
            return int(r->getData().acc);
        }
        return 0;
    }
    static int num_156() {
        auto r = PlayData.player[PLAYER_SLOT_MYBEST].ruleset;
        if (r)
        {
            return int(r->getData().acc * 100) % 100;
        }
        return 0;
    }
    static constexpr int (*num_157)() = num_122;
    static constexpr int (*num_158)() = num_123;
    static int num_160() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            return (int)c->getCurrentBPM();
        }
        return 0;
    }
    static int num_161() {
        if (SystemData.gNextScene == SceneType::PLAY && PlayData.playStarted)
            return int((Time().norm() - PlayData.timers["play_start"]) / 1000 / 60);
        return 0;
    }
    static int num_162() {
        if (SystemData.gNextScene == SceneType::PLAY && PlayData.playStarted)
            return int((Time().norm() - PlayData.timers["play_start"]) / 1000 % 60);
        return 0;
    }
    static int num_163() {
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
    static int num_164() {
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
    static int num_165() { return int((PlayData.loadProgressWav + PlayData.loadProgressBga) * 100 / 2); };
    static int num_166() { return 100; };
    static int num_167() { return int(PlayData.loadProgressWav * 100); };
    static int num_168() { return int(PlayData.loadProgressBga * 100); };

    static int num_170() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return score->exscore;
        }
        return 0;
    }
    static constexpr int (*num_171)() = num_101;
    static int num_172() {
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
    static int num_173() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return int(score->maxcombo);
        }
        return 0;
    }
    static constexpr int (*num_174)() = num_105;
    static int num_175() {
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
    static int num_176() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return score->bp;
        }
        return 0;
    }
    static int num_177() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BP);
        }
        return 0;
    }
    static int num_178() {
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
    static int num_183() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return (int)score->rate;
        }
        return 0;
    }
    static int num_184() {
        auto c = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
        if (c)
        {
            auto score = g_pScoreDB->getChartScoreBMS(c->getFileHash());
            return int(score->rate * 100) % 100;
        }
        return 0;
    }
    static int num_201() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getLastJudge(0).time.norm();
        }
        return 0;
    }
    static int num_213() {
        int slot = PLAYER_SLOT_PLAYER;
        int side = 1;
        if (PlayData.isBattle)
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
    static int num_210() {
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
    static int num_211() {
        int slot = PLAYER_SLOT_PLAYER;
        int side = 1;
        if (PlayData.isBattle)
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
    static int num_212() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_EARLY);
        }
        return 0;
    }
    static int num_214() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_LATE);
        }
        return 0;
    }
    static int num_217() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getNoteCount() - r->getNotesExpired());
        }
        return 0;
    }
    static int num_218() {
        auto r = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
        if (r)
        {
            return int(r->getNotesExpired());
        }
        return 0;
    }
    static int num_250() {
        if (PlayData.courseStageData.size() >= 1)
            return PlayData.courseStageData[0].level;
        return 0;
    }
    static int num_251() {
        if (PlayData.courseStageData.size() >= 2)
            return PlayData.courseStageData[1].level;
        return 0;
    }
    static int num_252() {
        if (PlayData.courseStageData.size() >= 3)
            return PlayData.courseStageData[2].level;
        return 0;
    }
    static int num_253() {
        if (PlayData.courseStageData.size() >= 4)
            return PlayData.courseStageData[3].level;
        return 0;
    }
    static int num_254() {
        if (PlayData.courseStageData.size() >= 5)
            return PlayData.courseStageData[4].level;
        return 0;
    }
    static int num_301() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return f->total;
    }

    static int num_302() { return int(PlayData.player[PLAYER_SLOT_PLAYER].greenNumber); };
    static int num_304() { return int(PlayData.player[PLAYER_SLOT_PLAYER].greenNumberMinBPM); };
    static int num_305() { return int(PlayData.player[PLAYER_SLOT_PLAYER].greenNumberMaxBPM); };
    static int num_301() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverTop; };
    static int num_306() { return PlayData.player[PLAYER_SLOT_PLAYER].lanecoverBottom; };

    static int num_342() { return int(PlayData.player[PLAYER_SLOT_TARGET].greenNumber); };
    static int num_344() { return int(PlayData.player[PLAYER_SLOT_TARGET].greenNumberMinBPM); };
    static int num_345() { return int(PlayData.player[PLAYER_SLOT_TARGET].greenNumberMaxBPM); };
    static int num_341() { return PlayData.player[PLAYER_SLOT_TARGET].lanecoverTop; };
    static int num_346() { return PlayData.player[PLAYER_SLOT_TARGET].lanecoverBottom; };

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

    static int num_350() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_key;
    }
    static int num_351() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)(f->notes_key_ln + f->notes_scratch_ln);
    }
    static int num_352() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_scratch;
    }
    static int num_353() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_scratch_ln;
    }
    static int num_354() {
        auto f = getCurrentSelectedChartBMSMeta();
        if (f == nullptr)
            return 0;
        return (int)f->notes_mine;
    }

    static int num_360() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_MISS);
        }
        return 0;
    }
    static constexpr int (*num_361)() = num_212;
    static constexpr int (*num_362)() = num_214;
    static int num_363() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_KPOOR);
        }
        return 0;
    }
    static int num_364() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_PLAYER].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_CB);
        }
        return 0;
    }
    static int num_370() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_MISS);
        }
        return 0;
    }
    static int num_371() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_EARLY);
        }
        return 0;
    }
    static int num_372() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_LATE);
        }
        return 0;
    }
    static int num_373() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_KPOOR);
        }
        return 0;
    }
    static int num_374() {
        auto r = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[PLAYER_SLOT_TARGET].ruleset);
        if (r)
        {
            return (int)r->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_CB);
        }
        return 0;
    }

    static int num_380() { return int((360 + int(PlayData.scratchAxis[0]) % 360) % 360 / 360.0 * 256); };
    static int num_381() { return int((360 + int(PlayData.scratchAxis[1]) % 360) % 360 / 360.0 * 256); };

};

#define define_has_member(index)                                                            \
class has_number_##index                                                                    \
{                                                                                           \
private:                                                                                    \
    typedef long yes_type;                                                                  \
    typedef char no_type;                                                                   \
    template <typename U> static yes_type test(decltype(&U::num_##index));                  \
    template <typename U> static no_type  test(...);                                        \
public:                                                                                     \
    static constexpr bool has_func = sizeof(test<NumberConverter>()) == sizeof(yes_type);   \
private:                                                                                    \
    template <typename U, typename = std::enable_if_t<!has_func>>                           \
    static constexpr int(*func())() { return &U::num_0; }                                   \
    template <typename U, typename = std::enable_if_t<has_func>>                            \
    static constexpr int(*func())() { return &U::num_##index; }                             \
public:                                                                                     \
    static constexpr int(*value)() = func<NumberConverter>();                               \
}

#define has_number(index)  has_number_##index::has_func
#define number(index) has_number_##index::value

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
define_has_member(300);
define_has_member(301);
define_has_member(302);
define_has_member(303);
define_has_member(304);
define_has_member(305);
define_has_member(306);
define_has_member(307);
define_has_member(308);
define_has_member(309);
define_has_member(310);
define_has_member(311);
define_has_member(312);
define_has_member(313);
define_has_member(314);
define_has_member(315);
define_has_member(316);
define_has_member(317);
define_has_member(318);
define_has_member(319);
define_has_member(320);
define_has_member(321);
define_has_member(322);
define_has_member(323);
define_has_member(324);
define_has_member(325);
define_has_member(326);
define_has_member(327);
define_has_member(328);
define_has_member(329);
define_has_member(330);
define_has_member(331);
define_has_member(332);
define_has_member(333);
define_has_member(334);
define_has_member(335);
define_has_member(336);
define_has_member(337);
define_has_member(338);
define_has_member(339);
define_has_member(340);
define_has_member(341);
define_has_member(342);
define_has_member(343);
define_has_member(344);
define_has_member(345);
define_has_member(346);
define_has_member(347);
define_has_member(348);
define_has_member(349);
define_has_member(350);
define_has_member(351);
define_has_member(352);
define_has_member(353);
define_has_member(354);
define_has_member(355);
define_has_member(356);
define_has_member(357);
define_has_member(358);
define_has_member(359);
define_has_member(360);
define_has_member(361);
define_has_member(362);
define_has_member(363);
define_has_member(364);
define_has_member(365);
define_has_member(366);
define_has_member(367);
define_has_member(368);
define_has_member(369);
define_has_member(370);
define_has_member(371);
define_has_member(372);
define_has_member(373);
define_has_member(374);
define_has_member(375);
define_has_member(376);
define_has_member(377);
define_has_member(378);
define_has_member(379);
define_has_member(380);
define_has_member(381);
define_has_member(382);
define_has_member(383);
define_has_member(384);
define_has_member(385);
define_has_member(386);
define_has_member(387);
define_has_member(388);
define_has_member(389);
define_has_member(390);
define_has_member(391);
define_has_member(392);
define_has_member(393);
define_has_member(394);
define_has_member(395);
define_has_member(396);
define_has_member(397);
define_has_member(398);
define_has_member(399);

#pragma endregion

}

namespace lr2skin
{

std::function<int()> convertNumberIndex(int n)
{
    using namespace lv;

    if (n >= 450 && n <= 499)
    {
        // ARENA: Exscore difference to Top
    }
    else if (n >= 500 && n <= 739)
    {
        int player = (n - 500) / 30;
        int index = (n - 500) % 30;

        switch (index)
        {
        case 0: return std::bind(NumberConverter::num_arena_0, player);
        case 1: return std::bind(NumberConverter::num_arena_1, player);
        case 2: return std::bind(NumberConverter::num_arena_2, player);
        case 3: return std::bind(NumberConverter::num_arena_3, player);
        case 4: return std::bind(NumberConverter::num_arena_4, player);
        case 5: return std::bind(NumberConverter::num_arena_5, player);
        case 6: return std::bind(NumberConverter::num_arena_6, player);
        case 7: return std::bind(NumberConverter::num_arena_7, player);
        case 8: return std::bind(NumberConverter::num_arena_8, player);
        case 9: return std::bind(NumberConverter::num_arena_9, player);
        case 10: return std::bind(NumberConverter::num_arena_10, player);
        case 11: return std::bind(NumberConverter::num_arena_11, player);
        case 12: return std::bind(NumberConverter::num_arena_12, player);
        case 13: return std::bind(NumberConverter::num_arena_13, player);
        case 14: return std::bind(NumberConverter::num_arena_14, player);
        case 15: return std::bind(NumberConverter::num_arena_15, player);
        case 16: return std::bind(NumberConverter::num_arena_16, player);
        case 17: return std::bind(NumberConverter::num_arena_17, player);
        case 18: return std::bind(NumberConverter::num_arena_18, player);
        case 19: return std::bind(NumberConverter::num_arena_19, player);
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
