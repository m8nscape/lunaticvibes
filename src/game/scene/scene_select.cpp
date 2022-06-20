#include <algorithm>
#include <memory>

#include "scene_select.h"
#include "scene_context.h"
#include "common/chartformat/chartformat_types.h"
#include "common/entry/entry_song.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"

#include "game/skin/skin_lr2_button_callbacks.h"
#include "game/scene/scene_context.h"
#include "game/scene/scene_mgr.h"

#include "imgui.h"

////////////////////////////////////////////////////////////////////////////////

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
                gTexts.set(eText(int(eText::_SELECT_BAR_TITLE_FULL_0) + bar_index), entry->_name);
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

            gNumbers.queue(eNumber::PLAY_BPM, static_cast<int>(std::round(bms->bpm)));

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
        gTexts.queue(eText::PLAY_ARTIST, "");
        gTexts.queue(eText::PLAY_SUBARTIST, "");
        gTexts.queue(eText::PLAY_GENRE, "");
        gTexts.queue(eText::PLAY_DIFFICULTY, "");
     }

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

                Option::e_rank_type rank = Option::RANK_NONE;
                if (pScore->rate >= 100.0) rank = Option::RANK_0;
                else if (pScore->rate >= 88.88) rank = Option::RANK_1;
                else if (pScore->rate >= 77.77) rank = Option::RANK_2;
                else if (pScore->rate >= 66.66) rank = Option::RANK_3;
                else if (pScore->rate >= 55.55) rank = Option::RANK_4;
                else if (pScore->rate >= 44.44) rank = Option::RANK_5;
                else if (pScore->rate >= 33.33) rank = Option::RANK_6;
                else if (pScore->rate >= 22.22) rank = Option::RANK_7;
                else if (pScore->rate != 0)     rank = Option::RANK_8;
                gOptions.queue(eOption::SELECT_ENTRY_RANK, rank);

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
                gNumbers.queue(eNumber::INFO_PERFECT_RATE, int(100 * pScore->pgreat / pScore->notes));
                gNumbers.queue(eNumber::INFO_GREAT_RATE, int(100 * pScore->great / pScore->notes));
                gNumbers.queue(eNumber::INFO_GOOD_RATE, int(100 * pScore->good / pScore->notes));
                gNumbers.queue(eNumber::INFO_BAD_RATE, int(100 * pScore->bad / pScore->notes));
                gNumbers.queue(eNumber::INFO_POOR_RATE, int(100 * (pScore->bpoor + pScore->miss) / pScore->notes));


                break;
            }
            default:
                break;
            }
        }
        else
        {
            gOptions.queue(eOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
            gOptions.queue(eOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

            gNumbers.queue(eNumber::INFO_SCORE, 0);
            gNumbers.queue(eNumber::INFO_EXSCORE, 0);
            gNumbers.queue(eNumber::INFO_EXSCORE_MAX, 0);
            gNumbers.queue(eNumber::INFO_RATE, 0);
            gNumbers.queue(eNumber::INFO_TOTALNOTE, 0);
            gNumbers.queue(eNumber::INFO_MAXCOMBO, 0);
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

        // backbmp
        if (!pf->backbmp.empty())
            gChartContext.texBackbmp.setPath(pf->getDirectory() / pf->backbmp);

        // _banner
        if (!pf->banner.empty())
            gChartContext.texBanner.setPath(pf->getDirectory() / pf->banner);
    }
    break;
    }
}

void config_sys()
{
    using namespace cfg;

    switch (gOptions.get(eOption::SYS_WINDOWED))
    {
    case Option::WIN_FULLSCREEN: ConfigMgr::set('C',V_WINMODE, V_WINMODE_FULL); break;
    case Option::WIN_BORDERLESS: ConfigMgr::set('C',V_WINMODE, V_WINMODE_BORDERLESS); break;
    case Option::WIN_WINDOWED: 
    default:                     ConfigMgr::set('C',V_WINMODE, V_WINMODE_WINDOWED); break;
    }

    
}

void config_player()
{
    using namespace cfg;

    ConfigMgr::set('P',P_HISPEED, gNumbers.get(eNumber::HS_1P) / 100.0);
    switch (gOptions.get(eOption::PLAY_HSFIX_TYPE_1P))
    {
    case Option::SPEED_FIX_MAX:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_MAX); break;
    case Option::SPEED_FIX_MIN:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_MIN); break;
    case Option::SPEED_FIX_AVG:      ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_AVG); break;
    case Option::SPEED_FIX_CONSTANT: ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_CONSTANT); break;
    default:                         ConfigMgr::set('P',P_SPEED_TYPE, P_SPEED_TYPE_NORMAL); break;
    }

    ConfigMgr::set('P',P_LOAD_BGA, gOptions.get(eOption::PLAY_BGA_TYPE) != Option::BGA_OFF);
    ConfigMgr::set('P',P_LANECOVER, gNumbers.get(eNumber::LANECOVER_1P));


    switch (gOptions.get(eOption::PLAY_RANDOM_TYPE_1P))
    {
    case Option::RAN_MIRROR: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_MIRROR); break;
    case Option::RAN_RANDOM: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_RANDOM); break;
    case Option::RAN_SRAN:   ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_SRAN); break;
    case Option::RAN_HRAN:   ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_HRAN); break;
    case Option::RAN_ALLSCR: ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_ALLSCR); break;
    default:                 ConfigMgr::set('P',P_CHART_OP, P_CHART_OP_NORMAL); break;
    }

    switch (gOptions.get(eOption::PLAY_GAUGE_TYPE_1P))
    {
    case Option::GAUGE_HARD:   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_HARD); break;
    case Option::GAUGE_EASY:   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_EASY); break;
    case Option::GAUGE_DEATH:  ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_DEATH); break;
    case Option::GAUGE_EXHARD: ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_EXHARD); break;
    case Option::GAUGE_ASSIST: ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_ASSIST); break;
    default:                   ConfigMgr::set('P',P_GAUGE_OP, P_GAUGE_OP_NORMAL); break;
    }

    switch (gOptions.get(eOption::PLAY_GHOST_TYPE_1P))
    {
    case Option::GHOST_TOP:         ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_A); break;
    case Option::GHOST_SIDE:        ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_B); break;
    case Option::GHOST_SIDE_BOTTOM: ConfigMgr::set('P',P_GHOST_TYPE, P_GHOST_TYPE_C); break;
    default:                        ConfigMgr::set('P',P_GHOST_TYPE, "OFF"); break;
    }

    ConfigMgr::set('P',P_JUDGE_OFFSET, gNumbers.get(eNumber::TIMING_ADJUST_VISUAL));
    ConfigMgr::set('P',P_GHOST_TARGET, gNumbers.get(eNumber::DEFAULT_TARGET_RATE));

    switch (gOptions.get(eOption::SELECT_FILTER_KEYS))
    {
    case Option::KEYS_7:  ConfigMgr::set('P',P_PLAY_MODE, P_PLAY_MODE_7K); break;
    case Option::KEYS_5:  ConfigMgr::set('P',P_PLAY_MODE, P_PLAY_MODE_5K); break;
    case Option::KEYS_14: ConfigMgr::set('P',P_PLAY_MODE, P_PLAY_MODE_14K); break;
    case Option::KEYS_10: ConfigMgr::set('P',P_PLAY_MODE, P_PLAY_MODE_10K); break;
    case Option::KEYS_9:  ConfigMgr::set('P',P_PLAY_MODE, P_PLAY_MODE_9K); break;
    default:              ConfigMgr::set('P',P_PLAY_MODE, P_PLAY_MODE_ALL); break;
    }

    switch (gOptions.get(eOption::SELECT_SORT))
    {
    case Option::SORT_TITLE: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_TITLE); break;
    case Option::SORT_LEVEL: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_LEVEL); break;
    case Option::SORT_CLEAR: ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_CLEAR); break;
    case Option::SORT_RATE:  ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_RATE); break;
    default:                 ConfigMgr::set('P',P_SORT_MODE, P_SORT_MODE_FOLDER); break;
    }

    switch (gOptions.get(eOption::SELECT_FILTER_DIFF))
    {
    case Option::DIFF_BEGINNER: ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_BEGINNER); break;
    case Option::DIFF_NORMAL:   ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_NORMAL); break;
    case Option::DIFF_HYPER:    ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_HYPER); break;
    case Option::DIFF_ANOTHER:  ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ANOTHER); break;
    case Option::DIFF_INSANE:   ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_INSANE); break;
    default:                    ConfigMgr::set('P',P_DIFFICULTY_FILTER, P_DIFFICULTY_FILTER_ALL); break;
    }

    ConfigMgr::set('P',P_BATTLE, gOptions.get(eOption::PLAY_BATTLE_TYPE) != 0);
    ConfigMgr::set('P',P_FLIP, gSwitches.get(eSwitch::PLAY_OPTION_DP_FLIP));
    ConfigMgr::set('P',P_SCORE_GRAPH, gSwitches.get(eSwitch::SYSTEM_SCOREGRAPH));
}

void config_vol()
{
    using namespace cfg;

    ConfigMgr::set('P',P_VOL_MASTER, gSliders.get(eSlider::VOLUME_MASTER));
    ConfigMgr::set('P',P_VOL_KEY, gSliders.get(eSlider::VOLUME_KEY));
    ConfigMgr::set('P',P_VOL_BGM, gSliders.get(eSlider::VOLUME_BGM));
}

void config_eq()
{
    using namespace cfg;

    ConfigMgr::set('P',P_EQ, gSwitches.get(eSwitch::SOUND_EQ));
    ConfigMgr::set('P',P_EQ0, gNumbers.get(eNumber::EQ0));
    ConfigMgr::set('P',P_EQ1, gNumbers.get(eNumber::EQ1));
    ConfigMgr::set('P',P_EQ2, gNumbers.get(eNumber::EQ2));
    ConfigMgr::set('P',P_EQ3, gNumbers.get(eNumber::EQ3));
    ConfigMgr::set('P',P_EQ4, gNumbers.get(eNumber::EQ4));
    ConfigMgr::set('P',P_EQ5, gNumbers.get(eNumber::EQ5));
    ConfigMgr::set('P',P_EQ6, gNumbers.get(eNumber::EQ6));
}

void config_freq()
{
    using namespace cfg;

    ConfigMgr::set('P',P_FREQ, gSwitches.get(eSwitch::SOUND_PITCH));
    switch (gOptions.get(eOption::SOUND_PITCH_TYPE))
    {
    case Option::FREQ_FREQ: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_FREQ); break;
    case Option::FREQ_PITCH: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_PITCH); break;
    case Option::FREQ_SPEED: ConfigMgr::set('P',P_FREQ_TYPE, P_FREQ_TYPE_SPEED); break;
    default: break;
    }
    ConfigMgr::set('P',P_FREQ_VAL, gNumbers.get(eNumber::PITCH));
}

void config_fx()
{
    using namespace cfg;

    ConfigMgr::set('P',P_FX0, gSwitches.get(eSwitch::SOUND_FX0));
    switch (gOptions.get(eOption::SOUND_TARGET_FX0))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX0_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX0))
    {
    case Option::FX_OFF:        ConfigMgr::set('P',P_FX0_TYPE, "OFF"); break;
    case Option::FX_REVERB:     ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY:      ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS:    ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS:   ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER:    ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS:     ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: ConfigMgr::set('P',P_FX0_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    ConfigMgr::set('P',P_FX0_P1, gNumbers.get(eNumber::FX0_P1));
    ConfigMgr::set('P', P_FX0_P2, gNumbers.get(eNumber::FX0_P2));

    ConfigMgr::set('P',P_FX1, gSwitches.get(eSwitch::SOUND_FX1));
    switch (gOptions.get(eOption::SOUND_TARGET_FX1))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX1_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX1))
    {
    case Option::FX_OFF:        ConfigMgr::set('P',P_FX1_TYPE, "OFF"); break;
    case Option::FX_REVERB:     ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY:      ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS:    ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS:   ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER:    ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS:     ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: ConfigMgr::set('P',P_FX1_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    ConfigMgr::set('P',P_FX1_P1, gNumbers.get(eNumber::FX1_P1));
    ConfigMgr::set('P',P_FX1_P2, gNumbers.get(eNumber::FX1_P2));

    ConfigMgr::set('P',P_FX2, gSwitches.get(eSwitch::SOUND_FX2));
    switch (gOptions.get(eOption::SOUND_TARGET_FX2))
    {
    case Option::FX_MASTER: ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_MASTER); break;
    case Option::FX_KEY:    ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_KEY); break;
    case Option::FX_BGM:    ConfigMgr::set('P',P_FX2_TARGET, P_FX_TARGET_BGM); break;
    default: break;
    }
    switch (gOptions.get(eOption::SOUND_FX2))
    {
    case Option::FX_OFF:        ConfigMgr::set('P',P_FX2_TYPE, "OFF"); break;
    case Option::FX_REVERB:     ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_REVERB); break;
    case Option::FX_DELAY:      ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_DELAY); break;
    case Option::FX_LOWPASS:    ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_LOWPASS); break;
    case Option::FX_HIGHPASS:   ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_HIGHPASS); break;
    case Option::FX_FLANGER:    ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_FLANGER); break;
    case Option::FX_CHORUS:     ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_CHORUS); break;
    case Option::FX_DISTORTION: ConfigMgr::set('P',P_FX2_TYPE, P_FX_TYPE_DIST); break;
    default: break;
    }
    ConfigMgr::set('P',P_FX2_P1, gNumbers.get(eNumber::FX2_P1));
    ConfigMgr::set('P',P_FX2_P2, gNumbers.get(eNumber::FX2_P2));
}


////////////////////////////////////////////////////////////////////////////////

SceneSelect::SceneSelect() : vScene(eMode::MUSIC_SELECT, 1000)
{
    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P;
    _inputAvailable |= INPUT_MASK_2P;

    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);
        loadSongList();
        setBarInfo();
        setEntryInfo();
        _skin->reset_bar_animation();
    }

    gSelectContext.isGoingToKeyConfig = false;
    gSelectContext.isGoingToSkinSelect = false;

    _state = eSelectState::PREPARE;
    _updateCallback = std::bind(&SceneSelect::updatePrepare, this);

    loopStart();

    SoundMgr::stopSamples();
    SoundMgr::playSample(eSoundSample::BGM_SELECT);

    // init imgui
    _imguiRefreshProfileList();
    old_profile_index = imgui_profile_index;

    _imguiRefreshFolderList();

    _imguiRefreshVideoResolutionList();
    auto resolutionY = ConfigMgr::get("V", cfg::V_RES_Y, CANVAS_HEIGHT);
    if (resolutionY == 720)
        imgui_video_resolution_index = 1;
    else if (resolutionY == 1080)
        imgui_video_resolution_index = 2;
    else if (resolutionY == 1440)
        imgui_video_resolution_index = 3;
    else if (resolutionY == 2160)
        imgui_video_resolution_index = 4;
    else
        imgui_video_resolution_index = 0;
    old_video_resolution_index = imgui_video_resolution_index;

    auto winMode = ConfigMgr::get("V", cfg::V_WINMODE, cfg::V_WINMODE_WINDOWED);
    if (winMode == cfg::V_WINMODE_FULL)
        imgui_video_mode = 1;
    else if (winMode == cfg::V_WINMODE_BORDERLESS)
        imgui_video_mode = 2;
    else
        imgui_video_mode = 0;
    old_video_mode = imgui_video_mode;

    imgui_video_vsync = ConfigMgr::get("V", cfg::V_VSYNC, false);
    imgui_video_maxFPS = ConfigMgr::get("V", cfg::V_MAXFPS, 240);

    imgui_audio_checkASIODevices = ConfigMgr::get("A", cfg::A_MODE, cfg::A_MODE_ASIO) == cfg::A_MODE_ASIO;
    imgui_audio_bufferCount = ConfigMgr::get("A", cfg::A_BUFCOUNT, 2);
    imgui_audio_bufferSize = ConfigMgr::get("A", cfg::A_BUFLEN, 256);
    _imguiRefreshAudioDevices();
    old_audio_device_index = imgui_audio_device_index;

    imgui_adv_scrollSpeed[0] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    imgui_adv_scrollSpeed[1] = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
    imgui_adv_minInputInterval = ConfigMgr::get("P", cfg::P_MIN_INPUT_INTERVAL, 16);
    imgui_adv_newSongDuration = ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24);
    imgui_adv_mouseAnalog = ConfigMgr::get("P", cfg::P_MOUSE_ANALOG, false);
    imgui_adv_relativeAxis = ConfigMgr::get("P", cfg::P_RELATIVE_AXIS, false);
}

SceneSelect::~SceneSelect()
{
    config_sys();
    config_player();
    config_vol();
    config_eq();
    config_freq();
    config_fx();
    ConfigMgr::save();
}

void SceneSelect::_updateAsync()
{
    _updateCallback();
}

void SceneSelect::updatePrepare()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeIntro)
    {
        _state = eSelectState::SELECT;
        _updateCallback = std::bind(&SceneSelect::updateSelect, this);

        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneSelect::inputGamePress, this, _1, _2));
        _input.register_h("SCENE_HOLD", std::bind(&SceneSelect::inputGameHold, this, _1, _2));
        _input.register_r("SCENE_RELEASE", std::bind(&SceneSelect::inputGameRelease, this, _1, _2));
        _input.register_a("SCENE_AXIS", std::bind(&SceneSelect::inputGameAxisSelect, this, _1, _2));
        _input.loopStart();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        gTimers.set(eTimer::LIST_MOVE_STOP, t.norm());

        // restore panel stat
        for (int i = 1; i <= 9; ++i)
        {
            eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
            if (gSwitches.get(p))
            {
                eTimer tm = static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + i);
                gTimers.set(tm, t.norm());
                SoundMgr::playSample(eSoundSample::SOUND_O_OPEN);
            }
        }

        LOG_DEBUG << "[Select] State changed to SELECT";
    }
}

void SceneSelect::updateSelect()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    pushMainThreadTask(std::bind(&SceneSelect::_imguiSettings, this));

    if ((t - scrollTimestamp).norm() >= gSelectContext.scrollTime)
    {
        if (!isHoldingUp && !isHoldingDown)
            scrollTimestamp = -1;
    }
    if (gSelectContext.isGoingToKeyConfig || gSelectContext.isGoingToSkinSelect)
    {
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        _state = eSelectState::FADEOUT;
        _updateCallback = std::bind(&SceneSelect::updateFadeout, this);
    }
}

void SceneSelect::updateSearch()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updatePanel(unsigned idx)
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneSelect::updateFadeout()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::SCENE_START);
    Time ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        if (gSelectContext.isGoingToKeyConfig)
        {
            SoundMgr::stopSamples();
            gNextScene = eScene::KEYCONFIG;
        }
        else if (gSelectContext.isGoingToSkinSelect)
        {
            SoundMgr::stopSamples();
            gNextScene = eScene::CUSTOMIZE;
        }
        else
        {
            gNextScene = eScene::EXIT;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneSelect::inputGamePress(InputMask& m, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    if (m[Input::Pad::F9])
    {
        imguiShow = !imguiShow;
        _skin->setHandleMouseEvents(!imguiShow);
    }
    if (imguiShow)
    {
        if (m[Input::Pad::ESC])
        {
            imguiShow = false;
            _skin->setHandleMouseEvents(true);
        }
        return;
    }

    if (m[Input::Pad::ESC])
    {
        // close panels if opened. exit if no panel is opened
        bool hasPanelOpened = _closeAllPanels(t);
        if (!hasPanelOpened)
        {
            LOG_DEBUG << "[Select] ESC";
            gNextScene = eScene::EXIT;
            return;
        }
    }

    if (m[Pad::M2])
    {
        // close panels if opened
        _closeAllPanels(t);
    }

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            inputGamePressPanel(input, t);
        }
        else
        {
            switch (_state)
            {
            case eSelectState::SELECT:
                inputGamePressSelect(input, t);
                break;

            case eSelectState::FADEOUT:
                break;

            default:
                break;
            }
        }

        // lights
        for (size_t k = Pad::K11; k <= Pad::K19; ++k)
        {
            if (input[k])
            {
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K11), t.norm());
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K11), TIMER_NEVER);
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_DOWN) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_UP) + k - Pad::K21), TIMER_NEVER);
                }
                else
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
    }
}

// CALLBACK
void SceneSelect::inputGameHold(InputMask& m, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            inputGameHoldPanel(input, t);
        }
        else
        {
            switch (_state)
            {
            case eSelectState::SELECT:
                inputGameHoldSelect(input, t);
                break;

            default:
                break;
            }
        }
    }
}

// CALLBACK
void SceneSelect::inputGameRelease(InputMask& m, const Time& t)
{
    Time rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() < _skin->info.timeIntro) return;

    using namespace Input;

    auto input = _inputAvailable & m;
    if (input.any())
    {
        // sub callbacks
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            inputGameReleasePanel(input, t);
        }
        else
        {

            switch (_state)
            {
            case eSelectState::SELECT:
                inputGameReleaseSelect(input, t);
                break;

            default:
                break;
            }
        }

        // lights
        for (size_t k = Pad::K11; k <= Pad::K19; ++k)
        {
            if (input[k])
            {
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K11), t.norm());
                gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K11), TIMER_NEVER);
            }
        }
        for (size_t k = Pad::K21; k <= Pad::K29; ++k)
        {
            if (input[k])
            {
                if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_UP) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K21_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
                else
                {
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_UP) + k - Pad::K21), t.norm());
                    gTimers.set(static_cast<eTimer>(static_cast<size_t>(eTimer::K11_DOWN) + k - Pad::K21), TIMER_NEVER);
                }
            }
        }
    }
}


void SceneSelect::inputGamePressSelect(InputMask& input, const Time& t)
{
    if (_skin->type() == eSkinType::LR2)
    {
        if (input[Input::Pad::K1START] || input[Input::Pad::K2START])
        {
            // close other panels
            _closeAllPanels(t);

            // open panel 1
            gSwitches.set(eSwitch::SELECT_PANEL1, true);
            gTimers.set(eTimer::PANEL1_START, t.norm());
            gTimers.set(eTimer::PANEL1_END, TIMER_NEVER);
            SoundMgr::playSample(eSoundSample::SOUND_O_OPEN);
            return;
        }
        if (selectDownTimestamp == -1 && (input[Input::Pad::K1SELECT || input[Input::Pad::K2SELECT]]))
        {
            switch (gSelectContext.entries[gSelectContext.idx].first->type())
            {
            case eEntryType::SONG:
            case eEntryType::RIVAL_SONG:
            {
                selectDownTimestamp = t;
                break;
            }
            }
        }
    }

    // navigate
    if (!gSelectContext.entries.empty())
    {
        switch (gSelectContext.entries[gSelectContext.idx].first->type())
        {
        case eEntryType::FOLDER:
        case eEntryType::CUSTOM_FOLDER:
            if ((input & INPUT_MASK_DECIDE).any())
                return _navigateEnter(t);
            break;

        case eEntryType::SONG:
        case eEntryType::CHART:
        case eEntryType::RIVAL_SONG:
        case eEntryType::RIVAL_CHART:
        case eEntryType::COURSE:
            if ((input & INPUT_MASK_DECIDE).any())
                return _decide();
            break;

        default:
            break;
        }
        if ((input & INPUT_MASK_CANCEL).any())
            return _navigateBack(t);

        if ((input & INPUT_MASK_NAV_UP).any())
        {
            isHoldingUp = true;
            if (scrollTimestamp == -1)
            {
                scrollTimestamp = t.norm();
                _navigateUpBy1(t);
            }
        }
        if ((input & INPUT_MASK_NAV_DN).any())
        {
            isHoldingDown = true;
            if (scrollTimestamp == -1)
            {
                scrollTimestamp = t.norm();
                _navigateDownBy1(t);
            }
        }
    }
}

void SceneSelect::inputGameHoldSelect(InputMask& input, const Time& t)
{
    // navigate
    if (isHoldingUp && (t - scrollTimestamp).norm() >= gSelectContext.scrollTime)
    {
        gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollTimestamp = t;
        _navigateUpBy1(t);
    }
    if (isHoldingDown && (t - scrollTimestamp).norm() >= gSelectContext.scrollTime)
    {
        gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150);
        scrollTimestamp = t;
        _navigateDownBy1(t);
    }
    if ((t - selectDownTimestamp).norm() >= 233 && !isInVersionList && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]))
    {
        _navigateVersionEnter(t);
    }
}

void SceneSelect::inputGameReleaseSelect(InputMask& input, const Time& t)
{
    if (_skin->type() == eSkinType::LR2)
    {
        if (selectDownTimestamp != -1 && (input[Input::Pad::K1SELECT] || input[Input::Pad::K2SELECT]))
        {
            if (isInVersionList)
            {
                _navigateVersionBack(t);
            }
            else
            {
                // short press on song, inc version by 1
                // TODO play some animation
                auto pSong = std::dynamic_pointer_cast<FolderSong>(gSelectContext.entries[gSelectContext.idx].first);
                pSong->incCurrentChart();
                setBarInfo();
                SoundMgr::playSample(eSoundSample::SOUND_DIFFICULTY);
            }
            selectDownTimestamp = -1;
            return;
        }
    }

    // navigate
    if ((input & INPUT_MASK_NAV_UP).any())
    {
        isHoldingUp = false;
        gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    }
    if ((input & INPUT_MASK_NAV_DN).any())
    {
        isHoldingDown = false;
        gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    }
}

void SceneSelect::inputGameAxisSelect(InputAxisPlus& input, const Time& t)
{
    double navUp = 0;
    double navDn = 0;
    for (int i = Input::Pad::S1L; i < Input::Pad::ESC; ++i)
    {
        auto k = static_cast<Input::Pad>(i);
        if (input[k].first > 0)
        {
            if (INPUT_MASK_NAV_UP[i]) navUp += input[k].first * input[k].second;
            if (INPUT_MASK_NAV_DN[i]) navDn += input[k].first * input[k].second;
        }
    }
    double navVal = -navUp + navDn;
    double navValAbs = std::abs(navVal);
    if (navValAbs >= InputMgr::getAxisMinSpeed())
    {
        if (!isHoldingUp && !isHoldingDown /* && !isScrollingByAxis*/)
        {
            gSelectContext.scrollTime = 1000 / (navValAbs * 1000);
            isScrollingByAxis = true;
        }
        if ((t - scrollTimestamp).norm() >= gSelectContext.scrollTime)
        {
            isScrollingByAxis = false;
            scrollTimestamp = t;
            if (navVal > 0)
                _navigateDownBy1(t);
            else
                _navigateUpBy1(t);
        }
    }
    else if (!isHoldingUp && !isHoldingDown)
    {
        gSelectContext.scrollTime = ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300);
    }
}

void SceneSelect::inputGamePressPanel(InputMask& input, const Time& t)
{
    using namespace Input;

    if (_skin->type() == eSkinType::LR2)
    {
        if (gSwitches.get(eSwitch::SELECT_PANEL1))
        {
            // 1: KEYS
            if (input[Pad::K12]) lr2skin::button::random_type(PLAYER_SLOT_1P, 1);
            if (input[Pad::K13]) lr2skin::button::battle(1);
            if (input[Pad::K14]) lr2skin::button::gauge_type(PLAYER_SLOT_1P, 1);
            if (input[Pad::K15]) lr2skin::button::hs(PLAYER_SLOT_1P, -1);
            if (input[Pad::K16]) lr2skin::button::autoscr(PLAYER_SLOT_1P, 1);
            if (input[Pad::K17]) lr2skin::button::hs(PLAYER_SLOT_1P, 1);

            if (gOptions.get(eOption::PLAY_BATTLE_TYPE) == 1)
            {
                // 1: KEYS
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_2P, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_2P, 1);
                if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_2P, -1);
                if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_2P, 1);
                if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_2P, 1);
            }
            else
            {
                // 1: KEYS
                if (input[Pad::K22]) lr2skin::button::random_type(PLAYER_SLOT_1P, 1);
                if (input[Pad::K23]) lr2skin::button::battle(1);
                if (input[Pad::K24]) lr2skin::button::gauge_type(PLAYER_SLOT_1P, 1);
                if (input[Pad::K25]) lr2skin::button::hs(PLAYER_SLOT_1P, -1);
                if (input[Pad::K26]) lr2skin::button::autoscr(PLAYER_SLOT_1P, 1);
                if (input[Pad::K27]) lr2skin::button::hs(PLAYER_SLOT_1P, 1);
            }
        }
    }
}

void SceneSelect::inputGameHoldPanel(InputMask& input, const Time& t)
{
}

void SceneSelect::inputGameReleasePanel(InputMask& input, const Time& t)
{
    if (gSwitches.get(eSwitch::SELECT_PANEL1) && (input[Input::Pad::K1START] || input[Input::Pad::K2START]))
    {
        // close panel 1
        gSwitches.set(eSwitch::SELECT_PANEL1, false);
        gTimers.set(eTimer::PANEL1_START, TIMER_NEVER);
        gTimers.set(eTimer::PANEL1_END, t.norm());
        SoundMgr::playSample(eSoundSample::SOUND_O_CLOSE);
        return;
    }

}

void SceneSelect::_decide()
{
    std::shared_lock<std::shared_mutex> u(gSelectContext._mutex);

    auto entry = gSelectContext.entries[gSelectContext.idx].first;
    //auto& chart = entry.charts[entry.chart_idx];
    auto& c = gChartContext;
    auto& p = gPlayContext;

    clearContextPlay();

    if (gSwitches.get(eSwitch::SYSTEM_AUTOPLAY))
    {
        gPlayContext.canRetry = false;
        gPlayContext.isAuto = true;
    }

    if (entry->type() == eEntryType::COURSE)
    {
        gPlayContext.canRetry = false;
        gPlayContext.isCourse = true;
        gPlayContext.isCourseFirstStage = true;
    }

    // gauge
    auto convertGaugeType = [](int nType) -> eModGauge
    {
        switch (nType)
        {
        case 1: return eModGauge::HARD;
        case 2: return eModGauge::DEATH;
        case 3: return eModGauge::EASY;
        case 4: return eModGauge::PATTACK;
        case 5: return eModGauge::GATTACK;
        case 6: return eModGauge::ASSISTEASY;
        case 7: return eModGauge::EXHARD;
        case 0:
        default: return eModGauge::NORMAL;
        };
    };
    gPlayContext.mods[PLAYER_SLOT_1P].gauge = convertGaugeType(gOptions.get(eOption::PLAY_GAUGE_TYPE_1P));
    gPlayContext.mods[PLAYER_SLOT_2P].gauge = convertGaugeType(gOptions.get(eOption::PLAY_GAUGE_TYPE_2P));

    // random
    auto convertRandomType = [](int nType) -> eModChart
    {
        switch (nType)
        {
        case 1: return eModChart::MIRROR;
        case 2: return eModChart::RANDOM;
        case 3: return eModChart::SRAN;
        case 4: return eModChart::HRAN;
        case 5: return eModChart::ALLSCR;
        case 0:
        default: return eModChart::NONE;
        };
    };
    gPlayContext.mods[PLAYER_SLOT_1P].chart = convertRandomType(gOptions.get(eOption::PLAY_RANDOM_TYPE_1P));
    gPlayContext.mods[PLAYER_SLOT_2P].chart = convertRandomType(gOptions.get(eOption::PLAY_RANDOM_TYPE_2P));

    // assist
    gPlayContext.mods[PLAYER_SLOT_1P].assist_mask |= gSwitches.get(eSwitch::PLAY_OPTION_AUTOSCR_1P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;
    gPlayContext.mods[PLAYER_SLOT_2P].assist_mask |= gSwitches.get(eSwitch::PLAY_OPTION_AUTOSCR_2P) ? PLAY_MOD_ASSIST_AUTOSCR : 0;

    // HS fix
    auto convertHSType = [](int nType) -> eModHs
    {
        switch (nType)
        {
        case 1: return eModHs::MAXBPM;
        case 2: return eModHs::MINBPM;
        case 3: return eModHs::AVERAGE;
        case 4: return eModHs::CONSTANT;
        case 0:
        default: return eModHs::NONE;
        };
    };
    gPlayContext.mods[PLAYER_SLOT_1P].hs = convertHSType(gOptions.get(eOption::PLAY_HSFIX_TYPE_1P));
    gPlayContext.mods[PLAYER_SLOT_2P].hs = convertHSType(gOptions.get(eOption::PLAY_HSFIX_TYPE_2P));

    // chart
    c.started = false;
    switch (entry->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
    case eEntryType::CHART:
    case eEntryType::RIVAL_CHART:
    {
        // set metadata
        if (entry->type() == eEntryType::SONG || entry->type() == eEntryType::RIVAL_SONG)
        {
            auto pFile = std::reinterpret_pointer_cast<FolderSong>(entry)->getCurrentChart();
            c.chartObj = pFile;
        }
        else
        {
            auto pFile = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            c.chartObj = pFile;
        }

        auto& chart = *c.chartObj;
        //c.path = chart._filePath;
        c.path = chart.absolutePath;

        if (c.hash != chart.fileHash)
        {
            c.isBgaLoaded = false;
            c.isSampleLoaded = false;
        }
        c.hash = chart.fileHash;

        //c.chartObj = std::make_shared<vChartFormat>(chart);
        c.title = chart.title;
        c.title2 = chart.title2;
        c.artist = chart.artist;
        c.artist2 = chart.artist2;
        c.genre = chart.genre;
        c.version = chart.version;
        c.level = chart.levelEstimated;
        c.minBPM = chart.minBPM;
        c.maxBPM = chart.maxBPM;
        c.startBPM = chart.startBPM;

        // set gamemode
        // FIXME 2P battle
        // FIXME DP battle
        if (c.chartObj->type() == eChartFormat::BMS)
        {
            auto pBMS = std::reinterpret_pointer_cast<BMS_prop>(c.chartObj);
            switch (pBMS->gamemode)
            {
            case 5: gPlayContext.mode = eMode::PLAY5; break;
            case 7: gPlayContext.mode = eMode::PLAY7; break;
            case 9: gPlayContext.mode = eMode::PLAY9; break;
            case 10: gPlayContext.mode = eMode::PLAY10; break;
            case 14: gPlayContext.mode = eMode::PLAY14; break;
            default: gPlayContext.mode = eMode::PLAY7; break;
            }
        }

        break;
    }
    case eEntryType::COURSE:
    {
        // reset mods
        static const std::set<eModGauge> courseGaugeModsAllowed = { eModGauge::NORMAL , eModGauge::HARD };
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_1P].gauge) == courseGaugeModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_GAUGE_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_1P].gauge = eModGauge::NORMAL;
        }
        if (courseGaugeModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_2P].gauge) == courseGaugeModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_GAUGE_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_2P].gauge = eModGauge::NORMAL;
        }
        static const std::set<eModChart> courseChartModsAllowed = { eModChart::NONE , eModChart::MIRROR };
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_1P].chart) == courseChartModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_RANDOM_TYPE_1P, 0);
            gPlayContext.mods[PLAYER_SLOT_1P].chart = eModChart::NONE;
        }
        if (courseChartModsAllowed.find(gPlayContext.mods[PLAYER_SLOT_2P].chart) == courseChartModsAllowed.end())
        {
            gOptions.set(eOption::PLAY_RANDOM_TYPE_2P, 0);
            gPlayContext.mods[PLAYER_SLOT_2P].chart = eModChart::NONE;
        }
        gSwitches.set(eSwitch::PLAY_OPTION_AUTOSCR_1P, false);
        gPlayContext.mods[PLAYER_SLOT_1P].assist_mask = 0; 
        gSwitches.set(eSwitch::PLAY_OPTION_AUTOSCR_2P, false);
        gPlayContext.mods[PLAYER_SLOT_2P].assist_mask = 0; 

        // set metadata

        break;
    }
    default:
        break;
    }

    loopEnd();
    _input.loopEnd();
    gNextScene = eScene::DECIDE;
}

void SceneSelect::loadSongList()
{
    // TODO load song list
    gSelectContext.entries.clear();
    for (auto& [e, s] : gSelectContext.backtrace.top().list)
    {
        // TODO replace name/name2 by tag.db

        // apply filter
        bool skip = false;
        switch (e->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
        {
            auto f = std::reinterpret_pointer_cast<FolderSong>(e);
            if (ConfigMgr::get('P', cfg::P_NO_COMBINE_CHARTS, false))
            {
                int nChartIdx = -1;
                for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
                {
                    if (f->getChart(idx)->type() == eChartFormat::BMS)
                    {
                        auto p = std::reinterpret_pointer_cast<BMS_prop>(f->getChart(idx));

                        // gamemode filter
                        if (gSelectContext.gamemode != 0 &&
                            p->gamemode != gSelectContext.gamemode)
                        {
                            continue;
                        }
                        // difficulty filter
                        if (gSelectContext.difficulty != 0 &&
                            p->difficulty != gSelectContext.difficulty)
                        {
                            nChartIdx = idx;
                            continue;
                        }
                        // filters are matched
                        nChartIdx = idx;
                        break;
                    }
                }
                // add entry into list
                gSelectContext.entries.push_back({ f, nullptr });
            }
            else
            {
                for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
                {
                    switch (f->getChart(idx)->type())
                    {
                    case eChartFormat::BMS:
                    {
                        auto p = std::reinterpret_pointer_cast<BMS_prop>(f->getChart(idx));

                        // difficulty filter
                        if (gSelectContext.difficulty != 0 &&
                            p->difficulty != gSelectContext.difficulty)
                        {
                            continue;
                        }
                        // gamemode filter
                        if (gSelectContext.gamemode != 0 &&
                            p->gamemode != gSelectContext.gamemode)
                        {
                            continue;
                        }
                        // add all charts as individual entries into list
                        gSelectContext.entries.push_back({ std::make_shared<EntryChart>(p), nullptr });
                    }
                    break;

                    default:
                        break;
                    }
                }
            }
            break;
        }
        case eEntryType::CHART:
        case eEntryType::RIVAL_CHART:
        {
            auto f = std::reinterpret_pointer_cast<EntryChart>(e)->_file;
            if (f->type() == eChartFormat::BMS)
            {
                auto p = std::reinterpret_pointer_cast<BMS_prop>(f);

                // gamemode filter
                if (gSelectContext.gamemode != 0 &&
                    p->gamemode != gSelectContext.gamemode)
                {
                    break;
                }
                // filters are matched
                gSelectContext.entries.push_back({ e, nullptr });
                break;
            }
            break;
        }

        default:
            gSelectContext.entries.push_back({ e, nullptr });
            break;
        }
    }

    // load score
    for (auto& [entry, score] : gSelectContext.entries)
    {
        std::shared_ptr<vChartFormat> pf;
        switch (entry->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
            pf = std::reinterpret_pointer_cast<FolderSong>(entry)->getCurrentChart();
            break;
        case eEntryType::CHART:
        case eEntryType::RIVAL_CHART:
            pf = std::reinterpret_pointer_cast<EntryChart>(entry)->_file;
            break;
        default: break;
        }

        if (pf)
        {
            switch (pf->type())
            {
            case eChartFormat::BMS:
            {
                auto pScore = g_pScoreDB->getChartScoreBMS(pf->fileHash);
                score = pScore;
            }
            break;
            default: break;
            }
        }
    }

    // TODO sort song list
    switch (gSelectContext.sort)
    {
    case SongListSort::DEFAULT:
    {
        auto& l = gSelectContext.entries;
        auto compareEntry = [](const Entry& entry1, const Entry& entry2)
        {
            auto& lhs = entry1.first;
            auto& rhs = entry2.first;
            if (lhs->type() != rhs->type())
                return lhs->type() > rhs->type();
            else
            {
                if (lhs->type() == eEntryType::SONG || lhs->type() == eEntryType::RIVAL_SONG)
                {
                    auto l = std::reinterpret_pointer_cast<FolderSong>(lhs)->getChart(0);
                    auto r = std::reinterpret_pointer_cast<FolderSong>(rhs)->getChart(0);
                    if (l->levelEstimated != r->levelEstimated) return l->levelEstimated > r->levelEstimated;
                    if (l->title != r->title) return l->title > r->title;
                    if (l->title2 != r->title2) return l->title2 > r->title2;
                    if (l->version != r->version) return l->version > r->version;
                    return l->fileHash > r->fileHash;
                }
                else if (lhs->type() == eEntryType::CHART || lhs->type() == eEntryType::RIVAL_CHART)
                {
                    const auto& l = std::reinterpret_pointer_cast<const EntryChart>(lhs)->_file;
                    const auto& r = std::reinterpret_pointer_cast<const EntryChart>(rhs)->_file;
                    if (l->levelEstimated != r->levelEstimated) return l->levelEstimated > r->levelEstimated;
                    if (l->title != r->title) return l->title > r->title;
                    if (l->title2 != r->title2) return l->title2 > r->title2;
                    if (l->version != r->version) return l->version > r->version;
                    return l->fileHash > r->fileHash;
                }
                else
                {
                    if (lhs->_name != rhs->_name) return lhs->_name > rhs->_name;
                    if (lhs->_name2 != rhs->_name2) return lhs->_name2 > rhs->_name2;
                    return lhs->md5 > rhs->md5;
                }
            }
        };
        std::sort(l.begin(), l.end(), compareEntry);
        break;
    }
    case SongListSort::TITLE:
    {
        auto& l = gSelectContext.entries;
        std::sort(l.begin(), l.end(), [](const Entry& entry1, const Entry& entry2)
            {
                auto&& lhs = entry1.first;
                auto&& rhs = entry2.first;
                if (lhs->type() != rhs->type())
                    return lhs->type() > rhs->type();
                else
                {
                    if (lhs->type() == eEntryType::SONG || lhs->type() == eEntryType::RIVAL_SONG)
                    {
                        auto l = std::reinterpret_pointer_cast<FolderSong>(lhs)->getChart(0);
                        auto r = std::reinterpret_pointer_cast<FolderSong>(rhs)->getChart(0);
                        if (l->title != r->title) return l->title > r->title;
                        if (l->title2 != r->title2) return l->title2 > r->title2;
                        if (l->version != r->version) return l->version > r->version;
                        return l->fileHash > r->fileHash;
                    }
                    else if (lhs->type() == eEntryType::CHART || lhs->type() == eEntryType::RIVAL_CHART)
                    {
                        const auto& l = std::reinterpret_pointer_cast<const EntryChart>(lhs)->_file;
                        const auto& r = std::reinterpret_pointer_cast<const EntryChart>(rhs)->_file;
                        if (l->title != r->title) return l->title > r->title;
                        if (l->title2 != r->title2) return l->title2 > r->title2;
                        if (l->version != r->version) return l->version > r->version;
                        return l->fileHash > r->fileHash;
                    }
                    else
                    {
                        if (lhs->_name != rhs->_name) return lhs->_name > rhs->_name;
                        if (lhs->_name2 != rhs->_name2) return lhs->_name2 > rhs->_name2;
                        return lhs->md5 > rhs->md5;
                    }
                }
            });
        break;
    }
    // TODO sort by difficulty
    // TODO sort by level
    // TODO sort by none (?)
    default:
        break;
    }
}

void SceneSelect::_navigateUpBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        gSelectContext.idx = (gSelectContext.entries.size() + gSelectContext.idx - 1) % gSelectContext.entries.size();
        _skin->start_bar_animation(-1);

        setBarInfo();
        setEntryInfo();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        SoundMgr::playSample(eSoundSample::SOUND_SCRATCH);
    }
    setDynamicTextures();
}

void SceneSelect::_navigateDownBy1(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        gSelectContext.idx = (gSelectContext.idx + 1) % gSelectContext.entries.size();
        _skin->start_bar_animation(+1);

        setBarInfo();
        setEntryInfo();

        gTimers.set(eTimer::LIST_MOVE, t.norm());
        SoundMgr::playSample(eSoundSample::SOUND_SCRATCH);
    }
    setDynamicTextures();
}

void SceneSelect::_navigateEnter(const Time& t)
{
    if (!gSelectContext.entries.empty())
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        const auto& [e, s] = gSelectContext.entries[gSelectContext.idx];
        switch (e->type())
        {
        case eEntryType::FOLDER:
        case eEntryType::CUSTOM_FOLDER:
        {
            SongListProperties prop{
                gSelectContext.backtrace.top().folder,
                e->md5,
                e->_name,
                {},
                gSelectContext.idx
            };
            auto top = g_pSongDB->browse(e->md5, false);
            for (size_t i = 0; i < top.getContentsCount(); ++i)
                prop.list.push_back({ top.getEntry(i), nullptr });

            gSelectContext.backtrace.push(prop);
            gSelectContext.entries.clear();
            gSelectContext.idx = 0;
            loadSongList();

            setBarInfo();
            setEntryInfo();

            SoundMgr::playSample(eSoundSample::SOUND_F_OPEN);
            break;
        }
        default:
            break;
        }
    }
    setDynamicTextures();
}
void SceneSelect::_navigateBack(const Time& t)
{
    if (!isInVersionList)
        selectDownTimestamp = -1;

    if (!gSelectContext.entries.empty())
    {
        std::unique_lock<std::shared_mutex> u(gSelectContext._mutex);

        auto top = gSelectContext.backtrace.top();
        if (!top.parent.empty())
        {
            gSelectContext.idx = 0;
            gSelectContext.backtrace.pop();
            top = gSelectContext.backtrace.top();
            gSelectContext.entries = top.list;
            gSelectContext.idx = top.index;

            setBarInfo();
            setEntryInfo();

            SoundMgr::playSample(eSoundSample::SOUND_F_CLOSE);
        }
    }
    setDynamicTextures();
}

void SceneSelect::_navigateVersionEnter(const Time& t)
{
    isInVersionList = true;

    // TODO
    // play some sound
    // play some animation
    // push current list into buffer
    // create version list
    // show list
}

void SceneSelect::_navigateVersionBack(const Time& t)
{
    // TODO
    // play some sound
    // play some animation
    // behavior like _navigateBack

    isInVersionList = false;
}

bool SceneSelect::_closeAllPanels(const Time& t)
{
    bool hasPanelOpened = false;
    for (int i = 1; i <= 9; ++i)
    {
        eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
        if (gSwitches.get(p))
        {
            hasPanelOpened = true;
            gSwitches.set(p, false);
            gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + i), TIMER_NEVER);
            gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + i), t.norm());
        }
    }
    if (hasPanelOpened)
    {
        SoundMgr::playSample(eSoundSample::SOUND_O_CLOSE);
    }
    return hasPanelOpened;
}

void SceneSelect::_imguiSampleDialog()
{
    ImGuiNewFrame();
    if (imguiShow)
    {
        ImGui::ShowDemoWindow(NULL);
    }
    ImGui::Render();
}

#ifdef WIN32
#include <ShlObj.h>
#include <ShlObj_core.h>
#endif
void SceneSelect::_imguiSettings()
{
    if (gNextScene != eScene::SELECT) return;

    ImGuiNewFrame();
    if (imguiShow)
    {
        if (ImGui::Begin("Settings", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(500, 450), ImGuiCond_Once);
            ImGui::PushItemWidth(ImGui::GetFontSize() * -20);

            if (ImGui::CollapsingHeader("General"))
            {
                ImGui::Combo("Profile", &imgui_profile_index, imgui_profiles_display.data(), (int)imgui_profiles_display.size());

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Folder"))
            {
                ImGui::Text("*Changing folder settings requires a restart now.");

                ImGui::ListBox("Folders", &imgui_folder_index, imgui_folders_display.data(), imgui_folders_display.size());

                if (ImGui::Button("Add..."))
                {
                    _imguiAddFolder();
                }
                ImGui::SameLine();
                if (ImGui::Button("Delete Selected"))
                {
                    _imguiDelFolder();
                }
                ImGui::SameLine();
                if (ImGui::Button("Browse Selected"))
                {
                    _imguiBrowseFolder();
                }

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Video"))
            {
                ImGui::Combo("Resolution", &imgui_video_resolution_index, imgui_video_resolution_display.data(), (int)imgui_video_resolution_display.size());

                ImGui::RadioButton("Windowed", &imgui_video_mode, 0);
                ImGui::SameLine();
                ImGui::RadioButton("FullScreen", &imgui_video_mode, 1);
                ImGui::SameLine();
                ImGui::RadioButton("Borderless", &imgui_video_mode, 2);
                ImGui::SameLine();
                if (ImGui::Button("Apply"))
                {
                    _imguiApplyResolution();
                }

                ImGui::Spacing();

                ImGui::Checkbox("VSync", &imgui_video_vsync);

                ImGui::InputInt("Max FPS", &imgui_video_maxFPS, 0);

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Audio"))
            {
                ImGui::Combo("Device", &imgui_audio_device_index, imgui_audio_devices_display.data(), (int)imgui_audio_devices_display.size());

                ImGui::Checkbox("Check ASIO Drivers", &imgui_audio_checkASIODevices);
                ImGui::SameLine();
                if (ImGui::Button("Refresh"))
                {
                    _imguiRefreshAudioDevices();
                }

                ImGui::Spacing();

                ImGui::InputInt("Buffer Count", &imgui_audio_bufferCount, 0);
                ImGui::InputInt("Buffer Size", &imgui_audio_bufferSize, 0);

                ImGui::Separator();
                ImGui::Spacing();
            }

            if (ImGui::CollapsingHeader("Advanced"))
            {
                ImGui::InputInt2("Song select scroll speed (ms)", imgui_adv_scrollSpeed);
                ImGui::InputInt("Miss BGA time (ms)", &imgui_adv_missBGATime, 0);
                ImGui::InputInt("Min input interval (ms)", &imgui_adv_minInputInterval, 1, 10);
                ImGui::InputInt("New song duration (hour)", &imgui_adv_newSongDuration, 1, 10);
                ImGui::Checkbox("Mouse Analog", &imgui_adv_mouseAnalog);
                ImGui::Checkbox("Relative Axis (useful for controllers with turntable)", &imgui_adv_relativeAxis);

                ImGui::Separator();
                ImGui::Spacing();
            }

        }
        ImGui::End();
    }
    ImGui::Render();

    _imguiCheckSettings();
}

void SceneSelect::_imguiRefreshProfileList()
{
    imgui_profile_index = -1;
    imgui_profiles.clear();
    imgui_profiles_display.clear();

    static const Path profilePath = Path(GAMEDATA_PATH) / "profile";
    if (!std::filesystem::exists(profilePath))
    {
        std::filesystem::create_directories(profilePath);
    }
    for (auto& p : std::filesystem::directory_iterator(profilePath))
    {
        if (p.is_directory())
        {
            auto name = p.path().filename().string();
            imgui_profiles.push_back(name);
            imgui_profiles_display.push_back(imgui_profiles.back().c_str());
        }
    }
    if (!imgui_profiles.empty())
    {
        int idx = -1;
        std::string profile = ConfigMgr::get('E', cfg::E_PROFILE, cfg::PROFILE_DEFAULT);
        for (const auto& p: imgui_profiles)
        {
            idx++;
            if (profile == p)
            {
                imgui_profile_index = idx;
                break;
            }
        }
        if ((size_t)idx == imgui_profiles.size())
        {
            imgui_profile_index = -1;
        }
    }
    else
    {
        imgui_profile_index = -1;
    }
}


void SceneSelect::_imguiRefreshFolderList()
{
    imgui_folder_index = -1;
    imgui_folders.clear();
    imgui_folders_display.clear();

    auto folders = ConfigMgr::General()->getFoldersStr();
    imgui_folders.assign(folders.begin(), folders.end());
    for (const auto& f : imgui_folders)
        imgui_folders_display.push_back(f.c_str());
}

void SceneSelect::_imguiRefreshVideoResolutionList()
{
    imgui_video_resolution.clear();
    imgui_video_resolution_display.clear();

    imgui_video_resolution.push_back("480p SD (640x480)");
    imgui_video_resolution.push_back("720p HD (1280x720)");
    imgui_video_resolution.push_back("1080p FHD (1920x1080)");
    imgui_video_resolution.push_back("2K WQHD (2560x1440)");
    imgui_video_resolution.push_back("4K UHD (3840x2160)");
    for (const auto& r : imgui_video_resolution)
    {
        imgui_video_resolution_display.push_back(r.c_str());
    }
}

void SceneSelect::_imguiCheckSettings()
{
    if (imgui_profile_index != old_profile_index)
    {
        // TODO reload profile
    }

    bool recreateWindow = false;
    if (imgui_video_resolution_index != old_video_resolution_index)
    {
        old_video_resolution_index = imgui_video_resolution_index;
        int x, y;
        switch (imgui_video_resolution_index)
        {
        case 0: x = 640; y = 480; break;
        case 1: x = 1280; y = 720; break;
        case 2: x = 1920; y = 1080; break;
        case 3: x = 2560; y = 1440; break;
        case 4: x = 3840; y = 2160; break;
        }
        ConfigMgr::set("V", cfg::V_RES_X, x);
        ConfigMgr::set("V", cfg::V_RES_Y, y);
        recreateWindow = true;
    }
    if (imgui_video_mode != old_video_mode)
    {
        old_video_mode = imgui_video_mode;
        const char* windowMode = NULL;
        switch (imgui_video_mode)
        {
        case 0: windowMode = cfg::V_WINMODE_WINDOWED; break;
        case 1: windowMode = cfg::V_WINMODE_FULL; break;
        case 2: windowMode = cfg::V_WINMODE_BORDERLESS; break;
        }
        ConfigMgr::set("V", cfg::V_WINMODE, windowMode);
        recreateWindow = true;
    }
    if (imgui_video_vsync != ConfigMgr::get("V", cfg::V_VSYNC, false))
    {
        ConfigMgr::set("V", cfg::V_VSYNC, imgui_video_vsync);
        recreateWindow = true;
    }
    if (recreateWindow)
    {
        // TODO recreate window
    }

    if (imgui_video_maxFPS != ConfigMgr::get("V", cfg::V_MAXFPS, 240))
    {
        ConfigMgr::set("V", cfg::V_MAXFPS, imgui_video_maxFPS);
    }

    if (imgui_audio_device_index != old_audio_device_index && imgui_audio_devices_display[imgui_audio_device_index] != ConfigMgr::get('A', cfg::A_DEVNAME, ""))
    {
        // TODO recreate audio device
    }
    if (imgui_audio_bufferCount != ConfigMgr::get("A", cfg::A_BUFCOUNT, 2))
    {
        ConfigMgr::set("A", cfg::A_BUFCOUNT, imgui_audio_bufferCount);
    }
    if (imgui_audio_bufferSize != ConfigMgr::get("A", cfg::A_BUFLEN, 256))
    {
        ConfigMgr::set("A", cfg::A_BUFLEN, imgui_audio_bufferSize);
    }

	if (imgui_adv_scrollSpeed[0] != ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_INITIAL, 300))
	{
        ConfigMgr::set("P", cfg::P_LIST_SCROLL_TIME_INITIAL, imgui_adv_scrollSpeed[0]);
	}
	if (imgui_adv_scrollSpeed[1] != ConfigMgr::get("P", cfg::P_LIST_SCROLL_TIME_HOLD, 150))
	{
        ConfigMgr::set("P", cfg::P_LIST_SCROLL_TIME_HOLD, imgui_adv_scrollSpeed[1]);
	}
	if (imgui_adv_minInputInterval != ConfigMgr::get("P", cfg::P_MIN_INPUT_INTERVAL, 16))
	{
        ConfigMgr::set("P", cfg::P_MIN_INPUT_INTERVAL, imgui_adv_minInputInterval);
	}
	if (imgui_adv_newSongDuration != ConfigMgr::get("P", cfg::P_NEW_SONG_DURATION, 24))
	{
        ConfigMgr::set("P", cfg::P_NEW_SONG_DURATION, imgui_adv_newSongDuration);
	}
	if (imgui_adv_mouseAnalog != ConfigMgr::get("P", cfg::P_MOUSE_ANALOG, false))
	{
        ConfigMgr::set("P", cfg::P_MOUSE_ANALOG, imgui_adv_mouseAnalog);
	}
	if (imgui_adv_relativeAxis != ConfigMgr::get("P", cfg::P_RELATIVE_AXIS, false))
	{
        ConfigMgr::set("P", cfg::P_RELATIVE_AXIS, imgui_adv_relativeAxis);
	}

}

bool SceneSelect::_imguiAddFolder()
{
    bool added = false;

#ifdef WIN32
    // TODO replace with IFileDialog
    char szDisplayName[MAX_PATH] = { 0 };
    BROWSEINFOA lpbi = { 0 };
    getWindowHandle(&lpbi.hwndOwner);
    lpbi.pszDisplayName = szDisplayName;
    lpbi.lpszTitle = "Select Folder";
    lpbi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_EDITBOX | BIF_USENEWUI;
    PIDLIST_ABSOLUTE lpiil = SHBrowseForFolderA(&lpbi);
    if (lpiil)
    {
        char szPath[MAX_PATH];
        if (SHGetPathFromIDList(lpiil, szPath))
        {
            imgui_folders.push_back(szPath);
            imgui_folders_display.push_back(imgui_folders.back().c_str());
            imgui_folder_index = -1;
            added = true;

            ConfigMgr::General()->setFolders(std::vector<std::string>(imgui_folders.begin(), imgui_folders.end()));

            // TODO reload?
        }
        CoTaskMemFree(lpiil);
    }
#endif
    return added;
}

bool SceneSelect::_imguiDelFolder()
{
    if (imgui_folder_index < 0 || imgui_folder_index >= imgui_folders_display.size()) return false;

    int oldSize = imgui_folders.size();
    imgui_folders.erase(std::next(imgui_folders.begin(), imgui_folder_index));
    imgui_folders_display.erase(std::next(imgui_folders_display.begin(), imgui_folder_index));
    if (imgui_folder_index == oldSize - 1)
        imgui_folder_index--;

    return false;
}

bool SceneSelect::_imguiBrowseFolder()
{
    if (imgui_folder_index < 0 || imgui_folder_index >= imgui_folders_display.size()) return false;

	std::string pathstr = Path(imgui_folders_display[imgui_folder_index]).string();

#ifdef WIN32
	ShellExecute(NULL, "open", pathstr.c_str(), NULL, NULL, SW_SHOWDEFAULT);
#elif defined __linux__
	// linux has many WMs that may have to handle differently
#endif

    return true;
}

bool SceneSelect::_imguiApplyResolution()
{
    return false;
}

bool SceneSelect::_imguiRefreshAudioDevices()
{
    imgui_audio_device_index = -1;
    imgui_audio_devices.clear();
    imgui_audio_devices_display.clear();
    auto adev = ConfigMgr::get('A', cfg::A_DEVNAME, "");

    auto devList = SoundMgr::getDeviceList(imgui_audio_checkASIODevices);
    for (auto& d : devList)
    {
        imgui_audio_devices.push_back(d);
        imgui_audio_devices_display.push_back(imgui_audio_devices.back().second.c_str());
        if (adev == d.second)
        {
            imgui_audio_device_index = (int)imgui_audio_devices.size() - 1;
        }
    }
    if (imgui_audio_device_index == -1)
    {
        imgui_audio_device_index = 0;
    }

    return false;
}
