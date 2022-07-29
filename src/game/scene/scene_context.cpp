#include "scene_context.h"
#include "game/data/data.h"
#include "common/chartformat/chartformat_types.h"
#include <random>
#include <mutex>

bool gResetSelectCursor = true;
bool gQuitOnFinish = false;
ChartContextParams gChartContext;
PlayContextParams gPlayContext;
SelectContextParams gSelectContext;
KeyConfigContextParams gKeyconfigContext;
CustomizeContextParams gCustomizeContext;
UpdateContextParams gUpdateContext;
OverlayContextParams gOverlayContext;
std::shared_ptr<SongDB> g_pSongDB;
std::shared_ptr<ScoreDB> g_pScoreDB;


void clearContextPlayForRetry()
{
    if (gPlayContext.chartObj[0] != nullptr) { gPlayContext.chartObj[0]->reset(); gPlayContext.chartObj[0].reset(); }
    if (gPlayContext.chartObj[1] != nullptr) { gPlayContext.chartObj[1]->reset(); gPlayContext.chartObj[1].reset(); }
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        gPlayContext.graphGauge[i].clear();
        gPlayContext.graphScore[i].clear();
        if (gPlayContext.ruleset[i]) gPlayContext.ruleset[i]->reset();
        gPlayContext.ruleset[i].reset();
    }
    gPlayContext.graphScoreTarget.clear();

}

void clearContextPlay()
{
    clearContextPlayForRetry();
    gPlayContext.chartObj[0] = nullptr;
    gPlayContext.chartObj[1] = nullptr;
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        gPlayContext.gaugeType[i] = eGaugeOp::GROOVE;
        gPlayContext.mods[i].clear();
    }
    gPlayContext.remainTime = 0;

    static std::random_device rd;
    gPlayContext.randomSeedChart = rd();
    gPlayContext.randomSeedMod = rd();

    gPlayContext.isAuto = false;
    gPlayContext.isCourse = false;
    gPlayContext.isCourseFirstStage = false;
}

void pushGraphPoints()
{
    gPlayContext.graphGauge[PLAYER_SLOT_1P].push_back(gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().health * 100);

    gPlayContext.graphScore[PLAYER_SLOT_1P].push_back(gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().score2);

    if (gPlayContext.ruleset[PLAYER_SLOT_2P])
        gPlayContext.graphScore[PLAYER_SLOT_2P].push_back(gPlayContext.ruleset[PLAYER_SLOT_2P]->getData().score2);

    gPlayContext.graphScoreTarget.push_back(static_cast<int>(std::floor(
        gPlayContext.ruleset[PLAYER_SLOT_1P]->getCurrentMaxScore() * (0.01 * gNumbers.get(eNumber::DEFAULT_TARGET_RATE)))));
}


void setBarInfo()
{
    const EntryList& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;
    const size_t count = size_t(eText::_SELECT_BAR_TITLE_FULL_MAX) - size_t(eText::_SELECT_BAR_TITLE_FULL_0) + 1;

    auto setSingleBarInfo = [&](size_t list_idx, size_t bar_index)
    {
        auto entry = e[list_idx].first;
        std::shared_ptr<vChartFormat> pf = nullptr;
        switch (entry->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
        {
            auto ps = std::reinterpret_pointer_cast<FolderSong>(entry);
            pf = ps->getCurrentChart();
            break;
        }
        case eEntryType::CHART:
        case eEntryType::RIVAL_CHART:
        {
            pf = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            break;
        }

        default:
            break;
        }

        if (pf != nullptr)
        {
            // chart types. eg. chart, rival_chart
            switch (pf->type())
            {
            case eChartFormat::BMS:
            {
                const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(pf);
                std::string name = entry->_name;
                if (!name.empty()) name += " ";
                if (!entry->_name2.empty()) name += entry->_name2;
                gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + bar_index), name);
                gNumbers.set(eNumber(int(eNumber::_SELECT_BAR_LEVEL_0) + bar_index), bms->playLevel);

                // TODO set bar lamp

                break;
            }

            default:
                gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + bar_index), entry->_name);
                gNumbers.set(eNumber(int(eNumber::_SELECT_BAR_LEVEL_0) + bar_index), 0);
                break;
            }
        }
        else
        {
            // other types. eg. folder, course, etc
            gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + bar_index), entry->_name);
        }
    };
    int list_idx, bar_index;
    for (list_idx = idx, bar_index = cursor; bar_index > 0; list_idx = (--list_idx + e.size()) % e.size(), --bar_index)
    {
        setSingleBarInfo(list_idx, bar_index);
    }
    for (list_idx = (idx + 1) % e.size(), bar_index = cursor + 1; bar_index < count; list_idx = (++list_idx) % e.size(), ++bar_index)
    {
        setSingleBarInfo(list_idx, bar_index);
    }
}

void setEntryInfo()
{
    const EntryList& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;

    // chart parameters
    if (e[idx].first->type() == eEntryType::CHART || e[idx].first->type() == eEntryType::RIVAL_CHART)
    {
        auto ps = std::reinterpret_pointer_cast<EntryChart>(e[idx].first);
        auto pf = std::reinterpret_pointer_cast<vChartFormat>(ps->_file);

        gSwitches.queue(eSwitch::CHART_HAVE_README,
            !(pf->text1.empty() && pf->text2.empty() && pf->text3.empty()));
        gSwitches.queue(eSwitch::CHART_HAVE_BANNER, !pf->banner.empty());
        gSwitches.queue(eSwitch::CHART_HAVE_STAGEFILE, !pf->stagefile.empty());

        gTexts.queue(eText::PLAY_TITLE, pf->title);
        gTexts.queue(eText::PLAY_SUBTITLE, pf->title2);
        if (pf->title2.empty())
            gTexts.queue(eText::PLAY_FULLTITLE, pf->title);
        else
            gTexts.queue(eText::PLAY_FULLTITLE, pf->title + " " + pf->title2);
        gTexts.queue(eText::PLAY_ARTIST, pf->artist);
        gTexts.queue(eText::PLAY_SUBARTIST, pf->artist2);
        gTexts.queue(eText::PLAY_GENRE, pf->genre);
        gTexts.queue(eText::PLAY_DIFFICULTY, pf->version);
        // _level

        // _totalLength_sec
        gNumbers.queue(eNumber::INFO_EXSCORE_MAX, pf->totalNotes * 2);
        gNumbers.queue(eNumber::INFO_TOTALNOTE, pf->totalNotes);

        gNumbers.queue(eNumber::PLAY_BPM, static_cast<int>(std::round(pf->startBPM)));
        gNumbers.queue(eNumber::INFO_BPM_MIN, static_cast<int>(std::round(pf->minBPM)));
        gNumbers.queue(eNumber::INFO_BPM_MAX, static_cast<int>(std::round(pf->maxBPM)));
        if (pf->minBPM != pf->maxBPM)
        {
            gSwitches.queue(eSwitch::CHART_HAVE_BPMCHANGE, true);
        }

        switch (ps->_file->type())
        {
        case eChartFormat::BMS:
        {
            const auto bms = std::reinterpret_pointer_cast<const BMS_prop>(pf);

            // gamemode
            switch (bms->player)
            {
            case 5:
            case 7:
            case 9:
            case 24:
                gOptions.queue(eOption::CHART_PLAY_MODE, Option::PLAY_SINGLE);
                break;

            case 10:
            case 14:
            case 48:
                gOptions.queue(eOption::CHART_PLAY_MODE, Option::PLAY_DOUBLE);
                break;

            default:
                break;
            }

            // gamemode
            unsigned op_keys = Option::KEYS_NOT_PLAYABLE;
            switch (bms->gamemode)
            {
            case 7:  op_keys = Option::KEYS_7; break;
            case 5:  op_keys = Option::KEYS_5; break;
            case 14: op_keys = Option::KEYS_14; break;
            case 10: op_keys = Option::KEYS_10; break;
            case 9:  op_keys = Option::KEYS_9; break;
            case 24: op_keys = Option::KEYS_24; break;
            case 48: op_keys = Option::KEYS_48; break;
            default: break;
            }
            gOptions.queue(eOption::CHART_PLAY_KEYS, op_keys);

            // judge
            unsigned op_judgerank = Option::JUDGE_NORMAL;
            switch (bms->rank)
            {
            case 0: op_judgerank = Option::JUDGE_VHARD; break;
            case 1: op_judgerank = Option::JUDGE_HARD; break;
            case 2: op_judgerank = Option::JUDGE_NORMAL; break;
            case 3: op_judgerank = Option::JUDGE_EASY; break;
            default: break;
            }
            gOptions.queue(eOption::CHART_JUDGE_TYPE, op_judgerank);

            // difficulty
            unsigned op_difficulty = Option::DIFF_0;
            gNumbers.queue(eNumber::MUSIC_BEGINNER_LEVEL, 0);
            gNumbers.queue(eNumber::MUSIC_NORMAL_LEVEL, 0);
            gNumbers.queue(eNumber::MUSIC_HYPER_LEVEL, 0);
            gNumbers.queue(eNumber::MUSIC_ANOTHER_LEVEL, 0);
            gNumbers.queue(eNumber::MUSIC_INSANE_LEVEL, 0);
            gBargraphs.queue(eBargraph::LEVEL_BAR_BEGINNER, 0);
            gBargraphs.queue(eBargraph::LEVEL_BAR_NORMAL, 0);
            gBargraphs.queue(eBargraph::LEVEL_BAR_HYPER, 0);
            gBargraphs.queue(eBargraph::LEVEL_BAR_ANOTHER, 0);
            gBargraphs.queue(eBargraph::LEVEL_BAR_INSANE, 0);
            switch (bms->difficulty)
            {
            case 0:
                op_difficulty = Option::DIFF_0;
                break;
            case 1:
                op_difficulty = Option::DIFF_1;
                gNumbers.queue(eNumber::MUSIC_BEGINNER_LEVEL, bms->playLevel);
                gBargraphs.queue(eBargraph::LEVEL_BAR_BEGINNER, bms->playLevel / 12.0);
                break;
            case 2:
                op_difficulty = Option::DIFF_2;
                gNumbers.queue(eNumber::MUSIC_NORMAL_LEVEL, bms->playLevel);
                gBargraphs.queue(eBargraph::LEVEL_BAR_NORMAL, bms->playLevel / 12.0);
                break;
            case 3:
                op_difficulty = Option::DIFF_3;
                gNumbers.queue(eNumber::MUSIC_HYPER_LEVEL, bms->playLevel);
                gBargraphs.queue(eBargraph::LEVEL_BAR_HYPER, bms->playLevel / 12.0);
                break;
            case 4:
                op_difficulty = Option::DIFF_4;
                gNumbers.queue(eNumber::MUSIC_ANOTHER_LEVEL, bms->playLevel);
                gBargraphs.queue(eBargraph::LEVEL_BAR_ANOTHER, bms->playLevel / 12.0);
                break;
            case 5:
                op_difficulty = Option::DIFF_5;
                gNumbers.queue(eNumber::MUSIC_INSANE_LEVEL, bms->playLevel);
                gBargraphs.queue(eBargraph::LEVEL_BAR_INSANE, bms->playLevel / 12.0);
                break;
            }
            gOptions.queue(eOption::CHART_DIFFICULTY, op_difficulty);

            // TODO TOTAL

            gSwitches.queue(eSwitch::CHART_HAVE_BGA, bms->haveBGA);
            gSwitches.queue(eSwitch::CHART_HAVE_BPMCHANGE, bms->haveBPMChange);
            gSwitches.queue(eSwitch::CHART_HAVE_LN, bms->haveLN);
            gSwitches.queue(eSwitch::CHART_HAVE_RANDOM, bms->haveRandom);

            //gSwitches.queue(eSwitch::CHART_HAVE_BACKBMP, ?);

            //gSwitches.queue(eSwitch::CHART_HAVE_SPEEDCHANGE, ?);

            break;
        }

        default:
            break;
        }
    }
    else
    {
        gTexts.queue(eText::PLAY_TITLE, e[idx].first->_name);
        gTexts.queue(eText::PLAY_SUBTITLE, e[idx].first->_name2);
        if (e[idx].first->_name2.empty())
            gTexts.queue(eText::PLAY_FULLTITLE, e[idx].first->_name);
        else
            gTexts.queue(eText::PLAY_FULLTITLE, e[idx].first->_name + " " + e[idx].first->_name2);
        gTexts.queue(eText::PLAY_ARTIST, "");
        gTexts.queue(eText::PLAY_SUBARTIST, "");
        gTexts.queue(eText::PLAY_GENRE, "");
        gTexts.queue(eText::PLAY_DIFFICULTY, "");
    }

    gOptions.queue(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_FOLDER);
    gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
    gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

    gNumbers.queue(eNumber::INFO_SCORE, 0);
    gNumbers.queue(eNumber::INFO_EXSCORE, 0);
    gNumbers.queue(eNumber::INFO_MAXCOMBO, 0);
    gNumbers.queue(eNumber::INFO_RATE, 0);
    gNumbers.queue(eNumber::INFO_BP, 0);
    gNumbers.queue(eNumber::INFO_PLAYCOUNT, 0);
    gNumbers.queue(eNumber::INFO_CLEARCOUNT, 0);
    gNumbers.queue(eNumber::INFO_FAILCOUNT, 0);

    gNumbers.queue(eNumber::INFO_PERFECT_COUNT, 0);
    gNumbers.queue(eNumber::INFO_GREAT_COUNT, 0);
    gNumbers.queue(eNumber::INFO_GOOD_COUNT, 0);
    gNumbers.queue(eNumber::INFO_BAD_COUNT, 0);
    gNumbers.queue(eNumber::INFO_POOR_COUNT, 0);
    gNumbers.queue(eNumber::INFO_PERFECT_RATE, 0);
    gNumbers.queue(eNumber::INFO_GREAT_RATE, 0);
    gNumbers.queue(eNumber::INFO_GOOD_RATE, 0);
    gNumbers.queue(eNumber::INFO_BAD_RATE, 0);
    gNumbers.queue(eNumber::INFO_POOR_RATE, 0);

    gBargraphs.queue(eBargraph::SELECT_MYBEST_PG, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_GR, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_GD, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_BD, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_PR, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_MAXCOMBO, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_SCORE, 0.);
    gBargraphs.queue(eBargraph::SELECT_MYBEST_EXSCORE, 0.);

    switch (e[idx].first->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
        gOptions.queue(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_SONG);
        gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

        break;

    case eEntryType::RIVAL_CHART:
    {
        // TODO 
        [[fallthrough]];
    }
    case eEntryType::CHART:
    {
        gOptions.queue(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_SONG);
        gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

        auto ps = std::reinterpret_pointer_cast<EntryChart>(e[idx].first);
        auto psc = std::reinterpret_pointer_cast<vScore>(e[idx].second);
        auto pf = std::reinterpret_pointer_cast<vChartFormat>(ps->_file);
        if (psc)
        {
            switch (pf->type())
            {
            case eChartFormat::BMS:
            {
                auto pScore = std::reinterpret_pointer_cast<ScoreBMS>(psc);

                Option::e_lamp_type lamp = Option::LAMP_NOPLAY;
                switch (pScore->lamp)
                {
                case ScoreBMS::Lamp::NOPLAY:    lamp = Option::LAMP_NOPLAY; break;
                case ScoreBMS::Lamp::FAILED:    lamp = Option::LAMP_FAILED; break;
                case ScoreBMS::Lamp::ASSIST:    lamp = Option::LAMP_ASSIST; break;
                case ScoreBMS::Lamp::EASY:      lamp = Option::LAMP_EASY; break;
                case ScoreBMS::Lamp::NORMAL:    lamp = Option::LAMP_NORMAL; break;
                case ScoreBMS::Lamp::HARD:      lamp = Option::LAMP_HARD; break;
                case ScoreBMS::Lamp::EXHARD:    lamp = Option::LAMP_EXHARD; break;
                case ScoreBMS::Lamp::FULLCOMBO: lamp = Option::LAMP_FULLCOMBO; break;
                case ScoreBMS::Lamp::PERFECT:   lamp = Option::LAMP_PERFECT; break;
                case ScoreBMS::Lamp::MAX:       lamp = Option::LAMP_MAX; break;
                }
                gOptions.queue(eOption::SELECT_ENTRY_LAMP, lamp);

                gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::getRankType(pScore->rate));

                gNumbers.queue(eNumber::INFO_SCORE, pScore->score);
                gNumbers.queue(eNumber::INFO_EXSCORE, pScore->exscore);
                gNumbers.queue(eNumber::INFO_EXSCORE_MAX, pScore->notes * 2);
                gNumbers.queue(eNumber::INFO_RATE, static_cast<int>(std::floor(pScore->rate)));
                gNumbers.queue(eNumber::INFO_TOTALNOTE, pScore->notes);
                gNumbers.queue(eNumber::INFO_MAXCOMBO, pScore->maxcombo);
                gNumbers.queue(eNumber::INFO_BP, pScore->bad + pScore->bpoor + pScore->miss);
                gNumbers.queue(eNumber::INFO_PLAYCOUNT, pScore->playcount);
                gNumbers.queue(eNumber::INFO_CLEARCOUNT, 0);    // TODO INFO_CLEARCOUNT
                gNumbers.queue(eNumber::INFO_FAILCOUNT, 0);     // TODO INFO_FAILCOUNT

                gNumbers.queue(eNumber::INFO_PERFECT_COUNT, pScore->pgreat);
                gNumbers.queue(eNumber::INFO_GREAT_COUNT, pScore->great);
                gNumbers.queue(eNumber::INFO_GOOD_COUNT, pScore->good);
                gNumbers.queue(eNumber::INFO_BAD_COUNT, pScore->bad);
                gNumbers.queue(eNumber::INFO_POOR_COUNT, pScore->bpoor + pScore->miss);
                if (pScore->notes != 0)
                {
                    gNumbers.queue(eNumber::INFO_PERFECT_RATE, int(100 * pScore->pgreat / pScore->notes));
                    gNumbers.queue(eNumber::INFO_GREAT_RATE, int(100 * pScore->great / pScore->notes));
                    gNumbers.queue(eNumber::INFO_GOOD_RATE, int(100 * pScore->good / pScore->notes));
                    gNumbers.queue(eNumber::INFO_BAD_RATE, int(100 * pScore->bad / pScore->notes));
                    gNumbers.queue(eNumber::INFO_POOR_RATE, int(100 * (pScore->bpoor + pScore->miss) / pScore->notes));

                    gBargraphs.queue(eBargraph::SELECT_MYBEST_PG, (double)pScore->pgreat / pScore->notes);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_GR, (double)pScore->great / pScore->notes);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_GD, (double)pScore->good / pScore->notes);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_BD, (double)pScore->bad / pScore->notes);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_PR, (double)(pScore->bpoor + pScore->miss) / pScore->notes);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_MAXCOMBO, (double)pScore->maxcombo / pScore->notes);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_SCORE, (double)pScore->score / 200000);
                    gBargraphs.queue(eBargraph::SELECT_MYBEST_EXSCORE, (double)pScore->exscore / (pScore->notes * 2));
                }


                break;
            }
            default:
                break;
            }
        }
        break;
    }

    case eEntryType::COURSE:
        gOptions.queue(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_COURSE);
        gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        // TODO course score
        break;

    case eEntryType::NEW_COURSE:
        gOptions.queue(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_NEW_COURSE);
        gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOT_APPLICIABLE);
        gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        gOptions.queue(eOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;

    case eEntryType::FOLDER:
    case eEntryType::CUSTOM_FOLDER:
    case eEntryType::RIVAL:
    default:
        gOptions.queue(eOption::SELECT_ENTRY_TYPE, Option::ENTRY_FOLDER);
        gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOT_APPLICIABLE);
        gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        gOptions.queue(eOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;
    }

    gTexts.flush();
    gNumbers.flush();
    gSwitches.flush();
    gOptions.flush();
    gBargraphs.flush();
}

void setDynamicTextures()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    const EntryList& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;

    // chart parameters
    auto entry = e[idx].first;
    switch (entry->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
    case eEntryType::CHART:
    case eEntryType::RIVAL_CHART:
    {
        std::shared_ptr<vChartFormat> pf;
        if (entry->type() == eEntryType::SONG || entry->type() == eEntryType::RIVAL_SONG)
        {
            auto ps = std::reinterpret_pointer_cast<FolderSong>(entry);
            pf = std::reinterpret_pointer_cast<vChartFormat>(ps->getCurrentChart());
        }
        else
        {
            auto ps = std::reinterpret_pointer_cast<EntryChart>(entry);
            pf = std::reinterpret_pointer_cast<vChartFormat>(ps->_file);
        }

        // _BG
        if (!pf->stagefile.empty())
            gChartContext.texStagefile.setPath(pf->getDirectory() / pf->stagefile);
        else
            gChartContext.texStagefile.setPath("");

        // backbmp
        if (!pf->backbmp.empty())
            gChartContext.texBackbmp.setPath(pf->getDirectory() / pf->backbmp);
        else
            gChartContext.texBackbmp.setPath("");

        // _banner
        if (!pf->banner.empty())
            gChartContext.texBanner.setPath(pf->getDirectory() / pf->banner);
        else
            gChartContext.texBanner.setPath("");
    }
    break;
    }
}


void createNotification(StringContentView text)
{
    std::unique_lock lock(gOverlayContext._mutex);
    gOverlayContext.notifications.push_back(std::make_pair(Time(), StringContent(text)));
}