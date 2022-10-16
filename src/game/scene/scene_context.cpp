#include "scene_context.h"
#include "game/runtime/state.h"
#include "common/chartformat/chartformat_types.h"
#include "config/config_mgr.h"
#include <random>
#include <mutex>

bool gResetSelectCursor = true;
bool gQuitOnFinish = false;
bool gAppIsExiting = false;
ChartContextParams gChartContext;
PlayContextParams gPlayContext;
SelectContextParams gSelectContext;
KeyConfigContextParams gKeyconfigContext;
CustomizeContextParams gCustomizeContext;
UpdateContextParams gUpdateContext;
OverlayContextParams gOverlayContext;
std::shared_ptr<SongDB> g_pSongDB;
std::shared_ptr<ScoreDB> g_pScoreDB;

std::pair<bool, Option::e_lamp_type> getSaveScoreType()
{
    if (gInCustomize) return { false, Option::LAMP_NOPLAY };

    if (gSelectContext.pitchSpeed < 1.0)
        return { false, Option::LAMP_NOPLAY };

    int battleType = State::get(IndexOption::PLAY_BATTLE_TYPE);
    if (battleType == Option::BATTLE_LOCAL || battleType == Option::BATTLE_DB)
        return { false, Option::LAMP_NOPLAY };

    if (State::get(IndexOption::PLAY_HSFIX_TYPE) == Option::e_speed_type::SPEED_FIX_CONSTANT)
        return { false, Option::LAMP_NOPLAY };

    Option::e_random_type randomType = (Option::e_random_type)State::get(IndexOption::PLAY_RANDOM_TYPE_1P);

    bool isPlaymodeDP = (State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DOUBLE ||
        State::get(IndexOption::PLAY_MODE) == Option::PLAY_MODE_DP_GHOST_BATTLE);

    if (randomType == Option::e_random_type::RAN_HRAN)
        return { false, Option::LAMP_ASSIST };
    else if (randomType == Option::e_random_type::RAN_ALLSCR)
        return { false, Option::LAMP_NOPLAY };

    if (isPlaymodeDP)
    {
        Option::e_random_type randomType2P = (Option::e_random_type)State::get(IndexOption::PLAY_RANDOM_TYPE_2P);
        if (randomType2P == Option::e_random_type::RAN_HRAN)
            return { false, Option::LAMP_ASSIST };
        else if (randomType2P == Option::e_random_type::RAN_ALLSCR)
            return { false, Option::LAMP_NOPLAY };
    }

    if (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_1P))
        return { true, Option::LAMP_ASSIST };

    if (isPlaymodeDP)
    {
        if (State::get(IndexSwitch::PLAY_OPTION_AUTOSCR_2P))
            return { true, Option::LAMP_ASSIST };
    }

    Option::e_gauge_type gaugeType = (Option::e_gauge_type)State::get(IndexOption::PLAY_GAUGE_TYPE_1P);
    Option::e_lamp_type lampType = Option::e_lamp_type::LAMP_NOPLAY;
    switch (gaugeType)
    {
    case Option::GAUGE_NORMAL: lampType = Option::e_lamp_type::LAMP_NORMAL; break;
    case Option::GAUGE_HARD:   lampType = Option::e_lamp_type::LAMP_HARD; break;
    case Option::GAUGE_DEATH:  lampType = Option::e_lamp_type::LAMP_FULLCOMBO; break;
    case Option::GAUGE_EASY:   lampType = Option::e_lamp_type::LAMP_EASY; break;
    case Option::GAUGE_ASSIST: lampType = Option::e_lamp_type::LAMP_ASSIST; break;
    case Option::GAUGE_EXHARD: lampType = Option::e_lamp_type::LAMP_EXHARD; break;
    }
    return { true, lampType };
}

void clearContextPlayForRetry()
{
    if (gPlayContext.chartObj[0] != nullptr) { gPlayContext.chartObj[0]->reset(); gPlayContext.chartObj[0].reset(); }
    if (gPlayContext.chartObj[1] != nullptr) { gPlayContext.chartObj[1]->reset(); gPlayContext.chartObj[1].reset(); }
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        gPlayContext.graphGauge[i].clear();
        gPlayContext.graphScore[i].clear();
        if (gPlayContext.ruleset[i])
            gPlayContext.ruleset[i].reset();
    }

    gPlayContext.replayNew.reset();
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
    gPlayContext.randomSeed = ((uint64_t)rd() << 32) | rd();

    gPlayContext.isCourse = false;
    gPlayContext.isCourseFirstStage = false;

    // gPlayContext.replay.reset();     // load at setEntryInfo() @ scene_context.cpp
    gPlayContext.replayMybest.reset();  // load at _decide() @ scene_select.cpp
}

void pushGraphPoints()
{
    gPlayContext.graphGauge[PLAYER_SLOT_PLAYER].push_back(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().health * 100);

    gPlayContext.graphScore[PLAYER_SLOT_PLAYER].push_back(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().score2);

    if (gPlayContext.ruleset[PLAYER_SLOT_TARGET])
    {
        gPlayContext.graphGauge[PLAYER_SLOT_TARGET].push_back(gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData().health * 100);
        gPlayContext.graphScore[PLAYER_SLOT_TARGET].push_back(gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData().score2);
    }

    if (!gPlayContext.isAuto && !gPlayContext.isReplay && gPlayContext.replayMybest)
    {
        gPlayContext.graphScore[PLAYER_SLOT_MYBEST].push_back(static_cast<int>(std::floor(
            gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->getCurrentMaxScore() * (0.01 * State::get(IndexNumber::DEFAULT_TARGET_RATE)))));
    }
}


void loadSongList()
{
    HashMD5 currentEntryHash;
    std::shared_ptr<EntryFolderSong> currentEntrySong;
    int currentEntryGamemode = 0;
    int currentEntryDifficulty = 0;
    if (!gSelectContext.entries.empty())
    {
        currentEntryHash = gSelectContext.entries.at(gSelectContext.idx).first->md5;

        if (gSelectContext.entries[gSelectContext.idx].first->type() == eEntryType::CHART ||
            gSelectContext.entries[gSelectContext.idx].first->type() == eEntryType::RIVAL_CHART)
        {
            auto& en = gSelectContext.entries[gSelectContext.idx].first;
            auto ps = std::reinterpret_pointer_cast<EntryChart>(en);
            auto pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);
            currentEntrySong = ps->getSongEntry();
            currentEntryGamemode = pf->gamemode;
            currentEntryDifficulty = pf->difficulty;
        }
    }

    gSelectContext.entries.clear();
    for (auto& [e, s] : gSelectContext.backtrace.top().dbBrowseEntries)
    {
        // TODO replace name/name2 by tag.db

        // apply filter
        auto checkFilterKeys = [](int keys)
        {
            if (State::get(IndexOption::PLAY_BATTLE_TYPE) != Option::BATTLE_DB)
            {
                // not DB, filter as usual
                switch (gSelectContext.filterKeys)
                {
                case 0: return true;
                case 1: return keys == 5 || keys == 7;
                case 2: return keys == 10 || keys == 14;
                default:
                    return keys == gSelectContext.filterKeys;
                }
            }
            else
            {
                // DB, only display SP charts
                switch (gSelectContext.filterKeys)
                {
                case 0: 
                case 1: 
                case 2: return keys == 5 || keys == 7;
                case 5:
                case 7:
                case 9:
                    return keys == gSelectContext.filterKeys;
                default:
                    return keys == gSelectContext.filterKeys / 2;
                }
            }
        };
        auto checkFilterDifficulty = [](int difficulty)
        {
            if (gSelectContext.filterDifficulty == 0) return true;
            return difficulty == gSelectContext.filterDifficulty;
        };
        bool skip = false;
        switch (e->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
        {
            auto f = std::reinterpret_pointer_cast<EntryFolderSong>(e);
            for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
            {
                switch (f->getChart(idx)->type())
                {
                case eChartFormat::BMS:
                {
                    auto p = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(f->getChart(idx));

                    if (!checkFilterDifficulty(p->difficulty)) continue;
                    if (!checkFilterKeys(p->gamemode)) continue;

                    // add all charts as individual entries into list.
                    gSelectContext.entries.push_back({ std::make_shared<EntryChart>(p, f), nullptr });
                }
                break;

                default:
                    break;
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
                auto p = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(f);

                if (!checkFilterDifficulty(p->difficulty)) continue;
                if (!checkFilterKeys(p->gamemode)) continue;

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
        std::shared_ptr<ChartFormatBase> pf;
        switch (entry->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
            pf = std::reinterpret_pointer_cast<EntryFolderSong>(entry)->getCurrentChart();
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

    if (gSelectContext.idx >= gSelectContext.entries.size())
    {
        gSelectContext.idx = 0;
    }
    else
    {
        auto findChart = [&](const HashMD5& hash)
        {
            for (size_t idx = 0; idx < gSelectContext.entries.size(); ++idx)
            {
                if (hash == gSelectContext.entries.at(idx).first->md5)
                {
                    return idx;
                }
            }
            return (size_t)-1;
        };

        gSelectContext.idx = 0;

        // look for the exact same entry
        size_t i = findChart(currentEntryHash);
        if (i != (size_t)-1)
        {
            gSelectContext.idx = i;
        }
        else if (currentEntrySong)
        {
            if (gSelectContext.filterDifficulty != 0)
            {
                // search from current difficulty
                const auto& chartList = currentEntrySong->getDifficultyList(currentEntryGamemode, gSelectContext.filterDifficulty);
                if (!chartList.empty())
                {
                    i = findChart((*chartList.begin())->fileHash);
                }
            }
            if (i != (size_t)-1)
            {
                gSelectContext.idx = i;
            }
            else
            {
                // search from the same difficulty
                const auto& chartList = currentEntrySong->getDifficultyList(currentEntryGamemode, currentEntryDifficulty);
                if (!chartList.empty())
                {
                    i = findChart((*chartList.begin())->fileHash);
                }
            }
            if (i != (size_t)-1)
            {
                gSelectContext.idx = i;
            }
            else
            {
                // search from any difficulties
                for (size_t diff = 1; diff <= 5; ++diff)
                {
                    if (currentEntryDifficulty == diff) continue;
                    if (currentEntryDifficulty == gSelectContext.filterDifficulty) continue;
                    const auto& diffList = currentEntrySong->getDifficultyList(currentEntryGamemode, diff);
                    if (!diffList.empty())
                    {
                        i = findChart((*diffList.begin())->fileHash);
                    }
                }
            }
            if (i != (size_t)-1)
            {
                gSelectContext.idx = i;
            }
            else
            {
                // search the very first version
                if (currentEntrySong->getContentsCount() > 0)
                {
                    i = findChart(currentEntrySong->getChart(0)->fileHash);
                }
            }
            if (i != (size_t)-1)
            {
                gSelectContext.idx = i;
            }
        }
    }
    State::set(IndexSlider::SELECT_LIST, gSelectContext.entries.empty() ? 0.0 : ((double)gSelectContext.idx / gSelectContext.entries.size()));
}

void sortSongList()
{
    HashMD5 currentEntryHash;
    if (!gSelectContext.entries.empty())
        currentEntryHash = gSelectContext.entries.at(gSelectContext.idx).first->md5;

    auto& entries = gSelectContext.entries;
    std::sort(entries.begin(), entries.end(), [](const Entry& entry1, const Entry& entry2)
        {
            auto& lhs = entry1.first;
            auto& rhs = entry2.first;
            if (lhs->type() != rhs->type())
            {
                return lhs->type() > rhs->type();
            }
            else if (lhs->type() == eEntryType::CUSTOM_FOLDER)
            {
                return entry1.first->md5 < entry2.first->md5;
            }
            else
            {
                pChartFormat l, r;
                if (lhs->type() == eEntryType::SONG || lhs->type() == eEntryType::RIVAL_SONG)
                {
                    l = std::reinterpret_pointer_cast<EntryFolderSong>(lhs)->getChart(0);
                    r = std::reinterpret_pointer_cast<EntryFolderSong>(rhs)->getChart(0);
                }
                else if (lhs->type() == eEntryType::CHART || lhs->type() == eEntryType::RIVAL_CHART)
                {
                    l = std::reinterpret_pointer_cast<const EntryChart>(lhs)->_file;
                    r = std::reinterpret_pointer_cast<const EntryChart>(rhs)->_file;
                }
                if (l && r)
                {
                    switch (gSelectContext.sort)
                    {
                    case SongListSort::DEFAULT:
                        if (l->folderHash != r->folderHash) return l->folderHash < r->folderHash;
                        if (l->levelEstimated != r->levelEstimated) return l->levelEstimated < r->levelEstimated;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    case SongListSort::TITLE:
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    case SongListSort::LEVEL:
                        if (l->levelEstimated != r->levelEstimated) return l->levelEstimated < r->levelEstimated;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    case SongListSort::CLEAR:
                    {
                        auto l_lamp = std::dynamic_pointer_cast<ScoreBMS>(entry1.second) ? std::reinterpret_pointer_cast<ScoreBMS>(entry1.second)->lamp : ScoreBMS::Lamp::NOPLAY;
                        auto r_lamp = std::dynamic_pointer_cast<ScoreBMS>(entry2.second) ? std::reinterpret_pointer_cast<ScoreBMS>(entry2.second)->lamp : ScoreBMS::Lamp::NOPLAY;
                        if (l_lamp != r_lamp) return l_lamp < r_lamp;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    }
                    case SongListSort::RATE:
                    {
                        auto l_rate = std::dynamic_pointer_cast<ScoreBMS>(entry1.second) ? std::reinterpret_pointer_cast<ScoreBMS>(entry1.second)->rate : 0.;
                        auto r_rate = std::dynamic_pointer_cast<ScoreBMS>(entry2.second) ? std::reinterpret_pointer_cast<ScoreBMS>(entry2.second)->rate : 0.;
                        if (l_rate != r_rate) return l_rate < r_rate;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    }
                    }
                }
                else
                {
                    if (lhs->_name != rhs->_name) return lhs->_name < rhs->_name;
                    if (lhs->_name2 != rhs->_name2) return lhs->_name2 < rhs->_name2;
                    if (lhs->md5 != rhs->md5) return lhs->md5 < rhs->md5;
                }
                return false;
            }
        });

    if (gSelectContext.idx >= gSelectContext.entries.size())
    {
        gSelectContext.idx = 0;
    }
    else
    {
        for (size_t idx = 0; idx < gSelectContext.entries.size(); ++idx)
        {
            if (currentEntryHash == gSelectContext.entries.at(idx).first->md5)
            {
                gSelectContext.idx = idx;
                break;
            }
        }
    }
    State::set(IndexSlider::SELECT_LIST, gSelectContext.entries.empty() ? 0.0 : ((double)gSelectContext.idx / gSelectContext.entries.size()));
}

void setBarInfo()
{
    const EntryList& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;
    const size_t count = size_t(IndexText::_SELECT_BAR_TITLE_FULL_MAX) - size_t(IndexText::_SELECT_BAR_TITLE_FULL_0) + 1;

    auto setSingleBarInfo = [&](size_t list_idx, size_t bar_index)
    {
        auto entry = e[list_idx].first;
        std::shared_ptr<ChartFormatBase> pf = nullptr;
        switch (entry->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
        {
            auto ps = std::reinterpret_pointer_cast<EntryFolderSong>(entry);
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
                const auto bms = std::reinterpret_pointer_cast<const ChartFormatBMSMeta>(pf);
                std::string name = entry->_name;
                if (!name.empty()) name += " ";
                if (!entry->_name2.empty()) name += entry->_name2;
                State::set(IndexText(int(IndexText::_SELECT_BAR_TITLE_FULL_0) + bar_index), name);
                State::set(IndexNumber(int(IndexNumber::_SELECT_BAR_LEVEL_0) + bar_index), bms->playLevel);

                // TODO set bar lamp

                break;
            }

            default:
                State::set(IndexText(int(IndexText::_SELECT_BAR_TITLE_FULL_0) + bar_index), entry->_name);
                State::set(IndexNumber(int(IndexNumber::_SELECT_BAR_LEVEL_0) + bar_index), 0);
                break;
            }
        }
        else
        {
            // other types. eg. folder, course, etc
            State::set(IndexText(int(IndexText::_SELECT_BAR_TITLE_FULL_0) + bar_index), entry->_name);
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
        auto pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);

        State::set(IndexSwitch::CHART_HAVE_README,
            !(pf->text1.empty() && pf->text2.empty() && pf->text3.empty()));
        State::set(IndexSwitch::CHART_HAVE_BANNER, !pf->banner.empty());
        State::set(IndexSwitch::CHART_HAVE_STAGEFILE, !pf->stagefile.empty());

        State::set(IndexSwitch::CHART_HAVE_REPLAY, false);
        auto& score = e[idx].second;
        if (score && !score->replayFileName.empty())
        {
            Path replayFilePath = ReplayChart::getReplayPath(ps->md5) / score->replayFileName;
            if (fs::is_regular_file(replayFilePath))
            {
                gPlayContext.replay = std::make_shared<ReplayChart>();
                if (gPlayContext.replay->loadFile(replayFilePath))
                {
                    State::set(IndexSwitch::CHART_HAVE_REPLAY, true);
                }
                else
                {
                    gPlayContext.replay.reset();
                }
            }
        }

        State::set(IndexText::PLAY_TITLE, pf->title);
        State::set(IndexText::PLAY_SUBTITLE, pf->title2);
        if (pf->title2.empty())
            State::set(IndexText::PLAY_FULLTITLE, pf->title);
        else
            State::set(IndexText::PLAY_FULLTITLE, pf->title + " " + pf->title2);
        State::set(IndexText::PLAY_ARTIST, pf->artist);
        State::set(IndexText::PLAY_SUBARTIST, pf->artist2);
        State::set(IndexText::PLAY_GENRE, pf->genre);
        State::set(IndexText::PLAY_DIFFICULTY, pf->version);
        // _level

        // _totalLength_sec
        State::set(IndexNumber::INFO_EXSCORE_MAX, pf->totalNotes * 2);
        State::set(IndexNumber::INFO_TOTALNOTE, pf->totalNotes);

        State::set(IndexNumber::PLAY_BPM, static_cast<int>(std::round(pf->startBPM)));
        State::set(IndexNumber::INFO_BPM_MIN, static_cast<int>(std::round(pf->minBPM)));
        State::set(IndexNumber::INFO_BPM_MAX, static_cast<int>(std::round(pf->maxBPM)));
        if (pf->minBPM != pf->maxBPM)
        {
            State::set(IndexSwitch::CHART_HAVE_BPMCHANGE, true);
        }

        switch (ps->_file->type())
        {
        case eChartFormat::BMS:
        {
            const auto bms = std::reinterpret_pointer_cast<const ChartFormatBMSMeta>(pf);

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
            State::set(IndexOption::CHART_PLAY_KEYS, op_keys);

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
            State::set(IndexOption::CHART_JUDGE_TYPE, op_judgerank);

            // notes detail
            State::set(IndexNumber::INFO_TOTALNOTE, bms->notes_total);
            State::set(IndexNumber::INFO_TOTALNOTE_NORMAL, bms->notes_key);
            State::set(IndexNumber::INFO_TOTALNOTE_LN, bms->notes_key_ln);
            State::set(IndexNumber::INFO_TOTALNOTE_SCRATCH, bms->notes_scratch);
            State::set(IndexNumber::INFO_TOTALNOTE_BSS, bms->notes_scratch_ln);
            State::set(IndexNumber::INFO_TOTALNOTE_MINE, bms->notes_mine);

            State::set(IndexNumber::INFO_BMS_TOTAL, bms->total);

            State::set(IndexSwitch::CHART_HAVE_BGA, bms->haveBGA);
            State::set(IndexSwitch::CHART_HAVE_BPMCHANGE, bms->haveBPMChange);
            State::set(IndexSwitch::CHART_HAVE_LN, bms->haveLN);
            State::set(IndexSwitch::CHART_HAVE_RANDOM, bms->haveRandom);

            //State::set(IndexSwitch::CHART_HAVE_BACKBMP, ?);

            //State::set(IndexSwitch::CHART_HAVE_SPEEDCHANGE, ?);

            break;
        }

        default:
            break;
        }

        // difficulty
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_1, false);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_2, false);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_3, false);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_4, false);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_5, false);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_1, false);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_2, false);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_3, false);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_4, false);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_5, false);
        State::set(IndexNumber::MUSIC_BEGINNER_LEVEL, 0);
        State::set(IndexNumber::MUSIC_NORMAL_LEVEL, 0);
        State::set(IndexNumber::MUSIC_HYPER_LEVEL, 0);
        State::set(IndexNumber::MUSIC_ANOTHER_LEVEL, 0);
        State::set(IndexNumber::MUSIC_INSANE_LEVEL, 0);
        State::set(IndexBargraph::LEVEL_BAR_BEGINNER, 0.0);
        State::set(IndexBargraph::LEVEL_BAR_NORMAL, 0.0);
        State::set(IndexBargraph::LEVEL_BAR_HYPER, 0.0);
        State::set(IndexBargraph::LEVEL_BAR_ANOTHER, 0.0);
        State::set(IndexBargraph::LEVEL_BAR_INSANE, 0.0);

        unsigned op_difficulty = Option::DIFF_0;
        switch (pf->difficulty)
        {
        case 0:
            op_difficulty = Option::DIFF_0;
            break;
        case 1:
            op_difficulty = Option::DIFF_1;
            break;
        case 2:
            op_difficulty = Option::DIFF_2;
            break;
        case 3:
            op_difficulty = Option::DIFF_3;
            break;
        case 4:
            op_difficulty = Option::DIFF_4;
            break;
        case 5:
            op_difficulty = Option::DIFF_5;
            break;
        }
        State::set(IndexOption::CHART_DIFFICULTY, op_difficulty);
        double barMaxLevel = 12.0;
        switch (pf->gamemode)
        {
        case 5:
        case 10: barMaxLevel = 10.0; break;
        case 7:
        case 14: barMaxLevel = 12.0; break;
        case 9:  barMaxLevel = 50.0; break;
        }
        State::set(IndexNumber((int)IndexNumber::MUSIC_BEGINNER_LEVEL + pf->difficulty - 1), pf->playLevel);
        State::set(IndexBargraph((int)IndexBargraph::LEVEL_BAR_BEGINNER + pf->difficulty - 1), pf->playLevel / barMaxLevel);

        auto pSong = ps->getSongEntry();
        if (pSong)
        {
            for (size_t difficulty = 1; difficulty <= 5; ++difficulty)
            {
                auto& chartList = pSong->getDifficultyList(pf->gamemode, difficulty);
                if (chartList.size() > 0)
                {
                    State::set(IndexSwitch((int)IndexSwitch::CHART_HAVE_DIFFICULTY_1 + difficulty - 1), true);
                    if (chartList.size() > 1)
                        State::set(IndexSwitch((int)IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_1 + difficulty - 1), true);

                    if (difficulty != pf->difficulty)
                    {
                        double barMaxLevel = 12.0;
                        switch (pf->gamemode)
                        {
                        case 5:
                        case 10: barMaxLevel = 10.0; break;
                        case 7:
                        case 14: barMaxLevel = 12.0; break;
                        case 9:  barMaxLevel = 50.0; break;
                        }
                        State::set(IndexNumber((int)IndexNumber::MUSIC_BEGINNER_LEVEL + difficulty - 1), (*chartList.begin())->playLevel);
                        State::set(IndexBargraph((int)IndexBargraph::LEVEL_BAR_BEGINNER + difficulty - 1), (*chartList.begin())->playLevel / barMaxLevel);
                    }
                }
            }
        }

    }
    else
    {
        State::set(IndexText::PLAY_TITLE, e[idx].first->_name);
        State::set(IndexText::PLAY_SUBTITLE, e[idx].first->_name2);
        if (e[idx].first->_name2.empty())
            State::set(IndexText::PLAY_FULLTITLE, e[idx].first->_name);
        else
            State::set(IndexText::PLAY_FULLTITLE, e[idx].first->_name + " " + e[idx].first->_name2);
        State::set(IndexText::PLAY_ARTIST, "");
        State::set(IndexText::PLAY_SUBARTIST, "");
        State::set(IndexText::PLAY_GENRE, "");
        State::set(IndexText::PLAY_DIFFICULTY, "");
    }

    State::set(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_FOLDER);
    State::set(IndexOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
    State::set(IndexOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

    State::set(IndexNumber::INFO_SCORE, 0);
    State::set(IndexNumber::INFO_EXSCORE, 0);
    State::set(IndexNumber::INFO_EXSCORE_MAX, 0);
    State::set(IndexNumber::INFO_RATE, 0);
    State::set(IndexNumber::INFO_TOTALNOTE, 0);
    State::set(IndexNumber::INFO_MAXCOMBO, 0);
    State::set(IndexNumber::INFO_BP, 0);
    State::set(IndexNumber::INFO_PLAYCOUNT, 0);
    State::set(IndexNumber::INFO_CLEARCOUNT, 0);
    State::set(IndexNumber::INFO_FAILCOUNT, 0);

    State::set(IndexNumber::INFO_PERFECT_COUNT, 0);
    State::set(IndexNumber::INFO_GREAT_COUNT, 0);
    State::set(IndexNumber::INFO_GOOD_COUNT, 0);
    State::set(IndexNumber::INFO_BAD_COUNT, 0);
    State::set(IndexNumber::INFO_POOR_COUNT, 0);
    State::set(IndexNumber::INFO_PERFECT_RATE, 0);
    State::set(IndexNumber::INFO_GREAT_RATE, 0);
    State::set(IndexNumber::INFO_GOOD_RATE, 0);
    State::set(IndexNumber::INFO_BAD_RATE, 0);
    State::set(IndexNumber::INFO_POOR_RATE, 0);

    State::set(IndexBargraph::SELECT_MYBEST_PG, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_GR, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_GD, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_BD, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_PR, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_MAXCOMBO, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_SCORE, 0.);
    State::set(IndexBargraph::SELECT_MYBEST_EXSCORE, 0.);

    switch (e[idx].first->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
        State::set(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_SONG);
        State::set(IndexOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        State::set(IndexOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

        break;

    case eEntryType::RIVAL_CHART:
    {
        // TODO 
        [[fallthrough]];
    }
    case eEntryType::CHART:
    {
        State::set(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_SONG);
        State::set(IndexOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        State::set(IndexOption::SELECT_ENTRY_RANK, Option::RANK_NONE);

        auto ps = std::reinterpret_pointer_cast<EntryChart>(e[idx].first);
        auto psc = std::reinterpret_pointer_cast<vScore>(e[idx].second);
        auto pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);
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
                State::set(IndexOption::SELECT_ENTRY_LAMP, lamp);

                State::set(IndexOption::SELECT_ENTRY_RANK, Option::getRankType(pScore->rate));

                State::set(IndexNumber::INFO_SCORE, pScore->score);
                State::set(IndexNumber::INFO_EXSCORE, pScore->exscore);
                State::set(IndexNumber::INFO_EXSCORE_MAX, pScore->notes * 2);
                State::set(IndexNumber::INFO_RATE, static_cast<int>(std::floor(pScore->rate)));
                State::set(IndexNumber::INFO_TOTALNOTE, pScore->notes);
                State::set(IndexNumber::INFO_MAXCOMBO, pScore->maxcombo);
                State::set(IndexNumber::INFO_BP, pScore->bad + pScore->bpoor + pScore->miss);
                State::set(IndexNumber::INFO_PLAYCOUNT, pScore->playcount);
                State::set(IndexNumber::INFO_CLEARCOUNT, pScore->clearcount);
                State::set(IndexNumber::INFO_FAILCOUNT, pScore->playcount - pScore->clearcount);

                State::set(IndexNumber::INFO_PERFECT_COUNT, pScore->pgreat);
                State::set(IndexNumber::INFO_GREAT_COUNT, pScore->great);
                State::set(IndexNumber::INFO_GOOD_COUNT, pScore->good);
                State::set(IndexNumber::INFO_BAD_COUNT, pScore->bad);
                State::set(IndexNumber::INFO_POOR_COUNT, pScore->bpoor + pScore->miss);
                if (pScore->notes != 0)
                {
                    State::set(IndexNumber::INFO_PERFECT_RATE, int(100 * pScore->pgreat / pScore->notes));
                    State::set(IndexNumber::INFO_GREAT_RATE, int(100 * pScore->great / pScore->notes));
                    State::set(IndexNumber::INFO_GOOD_RATE, int(100 * pScore->good / pScore->notes));
                    State::set(IndexNumber::INFO_BAD_RATE, int(100 * pScore->bad / pScore->notes));
                    State::set(IndexNumber::INFO_POOR_RATE, int(100 * (pScore->bpoor + pScore->miss) / pScore->notes));

                    State::set(IndexBargraph::SELECT_MYBEST_PG, (double)pScore->pgreat / pScore->notes);
                    State::set(IndexBargraph::SELECT_MYBEST_GR, (double)pScore->great / pScore->notes);
                    State::set(IndexBargraph::SELECT_MYBEST_GD, (double)pScore->good / pScore->notes);
                    State::set(IndexBargraph::SELECT_MYBEST_BD, (double)pScore->bad / pScore->notes);
                    State::set(IndexBargraph::SELECT_MYBEST_PR, (double)(pScore->bpoor + pScore->miss) / pScore->notes);
                    State::set(IndexBargraph::SELECT_MYBEST_MAXCOMBO, (double)pScore->maxcombo / pScore->notes);
                    State::set(IndexBargraph::SELECT_MYBEST_SCORE, (double)pScore->score / 200000);
                    State::set(IndexBargraph::SELECT_MYBEST_EXSCORE, (double)pScore->exscore / (pScore->notes * 2));
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
        State::set(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_COURSE);
        State::set(IndexOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        State::set(IndexOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        // TODO course score
        break;

    case eEntryType::NEW_COURSE:
        State::set(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_NEW_COURSE);
        State::set(IndexOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        State::set(IndexOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        State::set(IndexOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;

    case eEntryType::FOLDER:
    case eEntryType::CUSTOM_FOLDER:
    case eEntryType::RIVAL:
    default:
        State::set(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_FOLDER);
        State::set(IndexOption::SELECT_ENTRY_LAMP, Option::LAMP_NOPLAY);
        State::set(IndexOption::SELECT_ENTRY_RANK, Option::RANK_NONE);
        State::set(IndexOption::CHART_PLAY_KEYS, Option::KEYS_NOT_PLAYABLE);
        break;
    }

    setPlayModeInfo();
}

void setPlayModeInfo()
{
    bool isModeDP = false;
    if (State::get(IndexOption::CHART_PLAY_KEYS) != Option::KEYS_NOT_PLAYABLE)
    {
        switch (State::get(IndexOption::CHART_PLAY_KEYS))
        {
        case Option::KEYS_10:
        case Option::KEYS_14:
        case Option::KEYS_48:
            isModeDP = true;
            break;
        }
    }
    else
    {
        switch (gSelectContext.filterKeys)
        {
        case 2:
        case 10:
        case 14:
        case 48:
            isModeDP = true;
            break;
        }
    }
    if (!isModeDP)
    {
        switch (State::get(IndexOption::PLAY_BATTLE_TYPE))
        {
        case Option::BATTLE_OFF:   State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_SINGLE); break;
        case Option::BATTLE_LOCAL: State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_BATTLE); break;
        case Option::BATTLE_DB:    State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_DOUBLE_BATTLE); break;
        case Option::BATTLE_GHOST: State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_SP_GHOST_BATTLE); break;
        default: assert(false); break;
        }
    }
    else
    {
        switch (State::get(IndexOption::PLAY_BATTLE_TYPE))
        {
        case Option::BATTLE_OFF:   State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_DOUBLE); break;
        case Option::BATTLE_DB:    State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_DOUBLE_BATTLE); break;
        case Option::BATTLE_GHOST: State::set(IndexOption::PLAY_MODE, Option::PLAY_MODE_DP_GHOST_BATTLE); break;
        default: assert(false); break;
        }
    }
}

void switchDifficulty(int difficulty)
{
    const EntryList& e = gSelectContext.entries;
    if (e.empty()) return;

    const size_t idx = gSelectContext.idx;
    const size_t cursor = gSelectContext.cursor;

    // chart parameters
    if (e[idx].first->type() == eEntryType::CHART || e[idx].first->type() == eEntryType::RIVAL_CHART)
    {
        auto ps = std::reinterpret_pointer_cast<EntryChart>(e[idx].first);
        auto pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);
        auto pSong = ps->getSongEntry();
        if (pSong)
        {
            // choose next chart from song entry. They are likely be terribly scrambled
            /*
            pChartFormat pNextChart = nullptr;
            auto& chartList = pSong->getDifficultyList(pf->gamemode, difficulty);
            if (chartList.size() > 0)
            {
                if (difficulty == pf->difficulty)
                {
                    // find next entry
                    size_t chartIdx = 0;
                    for (; chartIdx < chartList.size(); ++chartIdx)
                    {
                        if (chartList[chartIdx] == pf) break;
                    }
                    if (chartIdx == chartList.size() - 1)
                    {
                        pNextChart = chartList[0];
                    }
                    else
                    {
                        pNextChart = chartList[chartIdx + 1];
                    }
                }
                else
                {
                    pNextChart = chartList[0];
                }
            }
            if (pNextChart)
            {
                for (size_t nextIdx = 0; nextIdx < e.size(); ++nextIdx)
                {
                    if (e[nextIdx].first->type() == eEntryType::CHART || e[nextIdx].first->type() == eEntryType::RIVAL_CHART)
                    {
                        auto pns = std::reinterpret_pointer_cast<EntryChart>(e[nextIdx].first);
                        if (pns->_file == pNextChart)
                        {
                            gSelectContext.idx = nextIdx;
                            break;
                        }
                    }
                }
                State::set(IndexSlider::SELECT_LIST, gSelectContext.entries.empty() ? 0.0 : ((double)gSelectContext.idx / gSelectContext.entries.size()));
            }
            */

            // choose directly from entry list
            pChartFormat pFirstChart = nullptr;
            size_t firstIdx = 0;
            bool currentFound = false;
            for (size_t nextIdx = 0; nextIdx < e.size(); ++nextIdx)
            {
                if (e[nextIdx].first->type() == eEntryType::CHART || e[nextIdx].first->type() == eEntryType::RIVAL_CHART)
                {
                    auto pns = std::reinterpret_pointer_cast<EntryChart>(e[nextIdx].first);
                    if (pns->getSongEntry() == ps->getSongEntry())
                    {
                        if (pFirstChart == nullptr)
                        {
                            pFirstChart = pns->_file;
                            firstIdx = nextIdx;
                        }

                        if (!currentFound && pns->_file == pf)
                        {
                            currentFound = true;
                        }
                        else if (currentFound && pns->_file->gamemode == pf->gamemode && pns->_file->difficulty == pf->difficulty)
                        {
                            gSelectContext.idx = nextIdx;
                            State::set(IndexSlider::SELECT_LIST, gSelectContext.entries.empty() ? 0.0 : ((double)gSelectContext.idx / gSelectContext.entries.size()));
                            return;
                        }
                    }
                }
            }
            // next chart not found, fallback to first
            if (pFirstChart)
            {
                gSelectContext.idx = firstIdx;
                State::set(IndexSlider::SELECT_LIST, gSelectContext.entries.empty() ? 0.0 : ((double)gSelectContext.idx / gSelectContext.entries.size()));
            }
        }
    }
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
        std::shared_ptr<ChartFormatBase> pf;
        if (entry->type() == eEntryType::SONG || entry->type() == eEntryType::RIVAL_SONG)
        {
            auto ps = std::reinterpret_pointer_cast<EntryFolderSong>(entry);
            pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->getCurrentChart());
        }
        else
        {
            auto ps = std::reinterpret_pointer_cast<EntryChart>(entry);
            pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);
        }

        // _BG
        if (!pf->stagefile.empty())
            gChartContext.texStagefile.setPath(pf->getDirectory() / PathFromUTF8(pf->stagefile));
        else
            gChartContext.texStagefile.setPath("");

        // backbmp
        if (!pf->backbmp.empty())
            gChartContext.texBackbmp.setPath(pf->getDirectory() / PathFromUTF8(pf->backbmp));
        else
            gChartContext.texBackbmp.setPath("");

        // _banner
        if (!pf->banner.empty())
            gChartContext.texBanner.setPath(pf->getDirectory() / PathFromUTF8(pf->banner));
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