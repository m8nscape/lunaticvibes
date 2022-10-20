#include "scene_context.h"
#include "game/runtime/state.h"
#include "common/chartformat/chartformat_types.h"
#include "config/config_mgr.h"
#include "common/entry/entry_types.h"
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
    gPlayContext.courseStage = 0;

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
            // get chart score
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
        else if (entry->type() == eEntryType::COURSE)
        {
            auto ps = std::reinterpret_pointer_cast<EntryCourse>(entry);
            auto pScore = g_pScoreDB->getCourseScoreBMS(ps->md5);
            score = pScore;
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

    std::map<std::string, int> param;
    std::map<std::string, double> paramf;
    std::map<std::string, std::string> text;

    // chart parameters
    if (e[idx].first->type() == eEntryType::CHART || e[idx].first->type() == eEntryType::RIVAL_CHART)
    {
        auto ps = std::reinterpret_pointer_cast<EntryChart>(e[idx].first);
        auto pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);

        param["havereadme"] = !(pf->text1.empty() && pf->text2.empty() && pf->text3.empty());
        param["havebanner"] = !pf->banner.empty();
        param["havestagefile"] = !pf->stagefile.empty();

        param["havereplay"] = false;
        auto& score = e[idx].second;
        if (score && !score->replayFileName.empty())
        {
            Path replayFilePath = ReplayChart::getReplayPath(ps->md5) / score->replayFileName;
            if (fs::is_regular_file(replayFilePath))
            {
                gPlayContext.replay = std::make_shared<ReplayChart>();
                if (gPlayContext.replay->loadFile(replayFilePath))
                {
                    param["havereplay"] = true;
                }
                else
                {
                    gPlayContext.replay.reset();
                }
            }
        }

        text["title"] = pf->title;
        text["subtitle"] = pf->title2;
        text["fulltitle"] = pf->title2.empty() ? pf->title : (pf->title + " " + pf->title2);
        text["artist"] = pf->artist;
        text["subartist"] = pf->artist2;
        text["genre"] = pf->genre;
        text["version"] = pf->version;

        param["max"] = pf->totalNotes * 2;
        param["totalnotes"] = pf->totalNotes;

        param["bpm"] = static_cast<int>(std::round(pf->startBPM));
        param["minbpm"] = static_cast<int>(std::round(pf->minBPM));
        param["maxbpm"] = static_cast<int>(std::round(pf->maxBPM));
        param["bpmchange"] = pf->minBPM != pf->maxBPM;

        param["min"] = pf->totalLength / 60;
        param["sec"] = pf->totalLength % 60;

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
            param["keys"] = op_keys;

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
            param["judgerank"] = op_judgerank;

            // notes detail
            param["totalnotes"] = bms->notes_total;
            param["totalnoteskey"] = bms->notes_key;
            param["totalnoteskeyln"] = bms->notes_key_ln;
            param["totalnotesscr"] = bms->notes_scratch;
            param["totalnotesscrln"] = bms->notes_scratch_ln;
            param["totalnotesmine"] = bms->notes_mine;

            param["total"] = bms->total;

            param["havebga"] = bms->haveBGA;
            param["bpmchange"] = bms->haveBPMChange;
            param["haveln"] = bms->haveLN;
            param["haverandom"] = bms->haveRandom;

            //State::set(IndexSwitch::CHART_HAVE_BACKBMP, ?);

            //State::set(IndexSwitch::CHART_HAVE_SPEEDCHANGE, ?);

            break;
        }

        default:
            break;
        }

        // difficulty
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
        param["difficulty"] = op_difficulty;
        double barMaxLevel = 12.0;
        switch (pf->gamemode)
        {
        case 5:
        case 10: barMaxLevel = 10.0; break;
        case 7:
        case 14: barMaxLevel = 12.0; break;
        case 9:  barMaxLevel = 50.0; break;
        }
        param["level"s + std::to_string(pf->difficulty)] = pf->playLevel;
        paramf["levelbar"s + std::to_string(pf->difficulty)] = pf->playLevel / barMaxLevel;

        auto pSong = ps->getSongEntry();
        if (pSong)
        {
            for (size_t difficulty = 1; difficulty <= 5; ++difficulty)
            {
                auto& chartList = pSong->getDifficultyList(pf->gamemode, difficulty);
                if (chartList.size() > 0)
                {
                    param["havedifficulty"s + std::to_string(difficulty)] = true;
                    if (chartList.size() > 1)
                        param["havemultipledifficulty"s + std::to_string(difficulty)] = true;

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
                        param["level"s + std::to_string(difficulty)] = (*chartList.begin())->playLevel;
                        paramf["levelbar"s + std::to_string(difficulty)] = (*chartList.begin())->playLevel / barMaxLevel;
                    }
                }
            }
        }

    }
    else if (e[idx].first->type() == eEntryType::COURSE)
    {
        auto ps = std::reinterpret_pointer_cast<EntryCourse>(e[idx].first);
        text["title"] = ps->_name;
        text["subtitle"] = ps->_name2;
        text["fulltitle"] = ps->_name2.empty() ? ps->_name : (ps->_name + " " + ps->_name2);

        switch (ps->courseType)
        {
        case EntryCourse::CourseType::GRADE:
            param["coursetype"] = Option::COURSE_GRADE; 
            text["genre"] = "CLASS";
            break;
        }
        param["coursestagecount"] = (int)ps->charts.size();
        for (size_t stage = 0; stage < ps->charts.size(); ++stage)
        {
            auto chartList = g_pSongDB->findChartByHash(ps->charts[stage]);
            if (!chartList.empty())
            {
                auto pChart = chartList[0];
                param["coursestagechartexist"s + std::to_string(stage + 1)] = 1;
                param["courselevel"s + std::to_string(stage + 1)] = pChart->playLevel;
                param["coursedifficulty"s + std::to_string(stage + 1)] = pChart->difficulty;
                text["coursetitle"s + std::to_string(stage + 1)] = pChart->title;
                text["coursesubtitle"s + std::to_string(stage + 1)] = pChart->title2;
            }
            else
            {
                param["coursenotplayable"s] = 1;
                param["coursestagechartexist"s + std::to_string(stage + 1)] = 0;
                param["courselevel"s + std::to_string(stage + 1)] = 0;
                param["coursedifficulty"s + std::to_string(stage + 1)] = 0;
                text["coursetitle"s + std::to_string(stage + 1)] = "CHART NOT FOUND";
                text["coursesubtitle"s + std::to_string(stage + 1)] = "MD5: "s + ps->charts[stage].hexdigest();
            }
        }
    }
    else
    {
        auto& p = e[idx].first;
        text["title"] = p->_name;
        text["artist"] = p->_name2;
        text["fulltitle"] = p->_name2.empty() ? p->_name : (p->_name + " " + p->_name2);

        switch (p->type())
        {
        case eEntryType::FOLDER:        text["genre"] = ""; break;
        case eEntryType::CUSTOM_FOLDER: text["genre"] = "Custom Folder"; break;
        case eEntryType::COURSE_FOLDER: text["genre"] = "Course Folder"; break;
        }
    }

    switch (e[idx].first->type())
    {
    case eEntryType::SONG:
    case eEntryType::RIVAL_SONG:
        param["entry"] = Option::ENTRY_SONG;

        break;

    case eEntryType::RIVAL_CHART:
    {
        // TODO 
        [[fallthrough]];
    }
    case eEntryType::CHART:
    {
        param["entry"] = Option::ENTRY_SONG;

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
                param["lamp"] = lamp;

                param["rank"] = Option::getRankType(pScore->rate);

                param["score"] = pScore->score;
                param["exscore"] = pScore->exscore;
                param["max"] = pScore->notes * 2;
                param["rate"] = static_cast<int>(std::floor(pScore->rate));
                param["totalnotes"] = pScore->notes;
                param["maxcombo"] = pScore->maxcombo;
                param["bp"] = pScore->bad + pScore->bpoor + pScore->miss;
                param["playcount"] = pScore->playcount;
                param["clearcount"] = pScore->clearcount;
                param["failcount"] = pScore->playcount - pScore->clearcount;

                param["pg"] = pScore->pgreat;
                param["gr"] = pScore->great;
                param["gd"] = pScore->good;
                param["bd"] = pScore->bad;
                param["pr"] = pScore->bpoor + pScore->miss;
                if (pScore->notes != 0)
                {
                    param["pgrate"] = int(100 * pScore->pgreat / pScore->notes);
                    param["grrate"] = int(100 * pScore->great / pScore->notes);
                    param["gdrate"] = int(100 * pScore->good / pScore->notes);
                    param["bdrate"] = int(100 * pScore->bad / pScore->notes);
                    param["prrate"] = int(100 * (pScore->bpoor + pScore->miss) / pScore->notes);

                    paramf["pg"] = (double)pScore->pgreat / pScore->notes;
                    paramf["gr"] = (double)pScore->great / pScore->notes;
                    paramf["gd"] = (double)pScore->good / pScore->notes;
                    paramf["bd"] = (double)pScore->bad / pScore->notes;
                    paramf["pr"] = (double)(pScore->bpoor + pScore->miss) / pScore->notes;
                    paramf["maxcombo"] = (double)pScore->maxcombo / pScore->notes;
                    paramf["score"] = (double)pScore->score / 200000;
                    paramf["exscore"] = (double)pScore->exscore / (pScore->notes * 2);
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
    {
        param["entry"] = Option::ENTRY_COURSE;

        auto ps = std::reinterpret_pointer_cast<EntryCourse>(e[idx].first);
        auto psc = std::reinterpret_pointer_cast<vScore>(e[idx].second);
        if (psc)
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
            param["lamp"] = lamp;

            param["rank"] = Option::getRankType(pScore->rate);

            param["score"] = pScore->score;
            param["exscore"] = pScore->exscore;
            param["max"] = pScore->notes * 2;
            param["rate"] = static_cast<int>(std::floor(pScore->rate));
            param["totalnotes"] = pScore->notes;
            param["maxcombo"] = pScore->maxcombo;
            param["bp"] = pScore->bad + pScore->bpoor + pScore->miss;
            param["playcount"] = pScore->playcount;
            param["clearcount"] = pScore->clearcount;
            param["failcount"] = pScore->playcount - pScore->clearcount;

            param["pg"] = pScore->pgreat;
            param["gr"] = pScore->great;
            param["gd"] = pScore->good;
            param["bd"] = pScore->bad;
            param["pr"] = pScore->bpoor + pScore->miss;
            if (pScore->notes != 0)
            {
                param["pgrate"] = int(100 * pScore->pgreat / pScore->notes);
                param["grrate"] = int(100 * pScore->great / pScore->notes);
                param["gdrate"] = int(100 * pScore->good / pScore->notes);
                param["bdrate"] = int(100 * pScore->bad / pScore->notes);
                param["prrate"] = int(100 * (pScore->bpoor + pScore->miss) / pScore->notes);

                paramf["pg"] = (double)pScore->pgreat / pScore->notes;
                paramf["gr"] = (double)pScore->great / pScore->notes;
                paramf["gd"] = (double)pScore->good / pScore->notes;
                paramf["bd"] = (double)pScore->bad / pScore->notes;
                paramf["pr"] = (double)(pScore->bpoor + pScore->miss) / pScore->notes;
                paramf["maxcombo"] = (double)pScore->maxcombo / pScore->notes;
                paramf["score"] = ps->charts.empty() ? 0.0 : (double)pScore->score / 200000 * ps->charts.size();
                paramf["exscore"] = (double)pScore->exscore / (pScore->notes * 2);
            }
        }
        break;
    }

    case eEntryType::NEW_COURSE:
        param["entry"] = Option::ENTRY_NEW_COURSE;
        break;

    case eEntryType::FOLDER:
    case eEntryType::CUSTOM_FOLDER:
    case eEntryType::COURSE_FOLDER:
    case eEntryType::RIVAL:
    default:
        param["entry"] = Option::ENTRY_FOLDER;
        break;
    }

    // save
    {
        State::set(IndexSwitch::CHART_HAVE_README, param["havereadme"]);
        State::set(IndexSwitch::CHART_HAVE_BANNER, param["havebanner"]);
        State::set(IndexSwitch::CHART_HAVE_STAGEFILE, param["havestagefile"]);
        State::set(IndexSwitch::CHART_HAVE_REPLAY, param["havereplay"]);

        State::set(IndexOption::SELECT_ENTRY_TYPE, param["entry"]);
        State::set(IndexOption::SELECT_ENTRY_LAMP, param["lamp"]);
        State::set(IndexOption::SELECT_ENTRY_RANK, param["rank"]);

        State::set(IndexText::PLAY_TITLE, text["title"]);
        State::set(IndexText::PLAY_SUBTITLE, text["subtitle"]);
        State::set(IndexText::PLAY_FULLTITLE, text["fulltitle"]);
        State::set(IndexText::PLAY_ARTIST, text["artist"]);
        State::set(IndexText::PLAY_SUBARTIST, text["subartist"]);
        State::set(IndexText::PLAY_GENRE, text["genre"]);
        State::set(IndexText::PLAY_DIFFICULTY, text["version"]);

        State::set(IndexNumber::PLAY_BPM, param["bpm"]);
        State::set(IndexNumber::INFO_BPM_MIN, param["minbpm"]);
        State::set(IndexNumber::INFO_BPM_MAX, param["maxbpm"]);

        State::set(IndexNumber::PLAY_MIN, param["min"]);
        State::set(IndexNumber::PLAY_SEC, param["sec"]);
        State::set(IndexNumber::PLAY_REMAIN_MIN, param["min"]);
        State::set(IndexNumber::PLAY_REMAIN_SEC, param["sec"]);

        State::set(IndexOption::CHART_DIFFICULTY, param["difficulty"]);
        State::set(IndexOption::CHART_PLAY_KEYS, param["keys"]);
        State::set(IndexOption::CHART_JUDGE_TYPE, param["judgerank"]);

        State::set(IndexNumber::INFO_TOTALNOTE, param["totalnotes"]);
        State::set(IndexNumber::INFO_TOTALNOTE_NORMAL, param["totalnoteskey"]);
        State::set(IndexNumber::INFO_TOTALNOTE_LN, param["totalnoteskeyln"]);
        State::set(IndexNumber::INFO_TOTALNOTE_SCRATCH, param["totalnotesscr"]);
        State::set(IndexNumber::INFO_TOTALNOTE_BSS, param["totalnotesscrln"]);
        State::set(IndexNumber::INFO_TOTALNOTE_MINE, param["totalnotesmine"]);

        State::set(IndexNumber::INFO_BMS_TOTAL, param["total"]);

        State::set(IndexSwitch::CHART_HAVE_BGA, param["havebga"]);
        State::set(IndexSwitch::CHART_HAVE_BPMCHANGE, param["bpmchange"]);
        State::set(IndexSwitch::CHART_HAVE_LN, param["haveln"]);
        State::set(IndexSwitch::CHART_HAVE_RANDOM, param["haverandom"]);

        State::set(IndexNumber::INFO_SCORE, param["score"]);
        State::set(IndexNumber::INFO_EXSCORE, param["exscore"]);
        State::set(IndexNumber::INFO_EXSCORE_MAX, param["max"]);
        State::set(IndexNumber::INFO_RATE, param["rate"]);
        State::set(IndexNumber::INFO_MAXCOMBO, param["maxcombo"]);
        State::set(IndexNumber::INFO_BP, param["bp"]);
        State::set(IndexNumber::INFO_PLAYCOUNT, param["playcount"]);
        State::set(IndexNumber::INFO_CLEARCOUNT, param["clearcount"]);
        State::set(IndexNumber::INFO_FAILCOUNT, param["failcount"]);

        State::set(IndexNumber::INFO_PERFECT_COUNT, param["pg"]);
        State::set(IndexNumber::INFO_GREAT_COUNT, param["gr"]);
        State::set(IndexNumber::INFO_GOOD_COUNT, param["gd"]);
        State::set(IndexNumber::INFO_BAD_COUNT, param["bd"]);
        State::set(IndexNumber::INFO_POOR_COUNT, param["pr"]);

        State::set(IndexNumber::INFO_PERFECT_RATE, param["pgrate"]);
        State::set(IndexNumber::INFO_GREAT_RATE, param["grrate"]);
        State::set(IndexNumber::INFO_GOOD_RATE, param["gdrate"]);
        State::set(IndexNumber::INFO_BAD_RATE, param["bdrate"]);
        State::set(IndexNumber::INFO_POOR_RATE, param["prrate"]);

        State::set(IndexBargraph::SELECT_MYBEST_PG, paramf["pg"]);
        State::set(IndexBargraph::SELECT_MYBEST_GR, paramf["gr"]);
        State::set(IndexBargraph::SELECT_MYBEST_GD, paramf["gd"]);
        State::set(IndexBargraph::SELECT_MYBEST_BD, paramf["bd"]);
        State::set(IndexBargraph::SELECT_MYBEST_PR, paramf["pr"]);
        State::set(IndexBargraph::SELECT_MYBEST_MAXCOMBO, paramf["maxcombo"]);
        State::set(IndexBargraph::SELECT_MYBEST_SCORE, paramf["score"]);
        State::set(IndexBargraph::SELECT_MYBEST_EXSCORE, paramf["exscore"]);

        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_1, param["havedifficulty1"]);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_2, param["havedifficulty2"]);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_3, param["havedifficulty3"]);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_4, param["havedifficulty4"]);
        State::set(IndexSwitch::CHART_HAVE_DIFFICULTY_5, param["havedifficulty5"]);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_1, param["havemultipledifficulty1"]);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_2, param["havemultipledifficulty2"]);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_3, param["havemultipledifficulty3"]);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_4, param["havemultipledifficulty4"]);
        State::set(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_5, param["havemultipledifficulty5"]);
        State::set(IndexNumber::MUSIC_BEGINNER_LEVEL, param["level1"]);
        State::set(IndexNumber::MUSIC_NORMAL_LEVEL, param["level2"]);
        State::set(IndexNumber::MUSIC_HYPER_LEVEL, param["level3"]);
        State::set(IndexNumber::MUSIC_ANOTHER_LEVEL, param["level4"]);
        State::set(IndexNumber::MUSIC_INSANE_LEVEL, param["level5"]);
        State::set(IndexBargraph::LEVEL_BAR_BEGINNER, paramf["levelbar1"]);
        State::set(IndexBargraph::LEVEL_BAR_NORMAL, paramf["levelbar2"]);
        State::set(IndexBargraph::LEVEL_BAR_HYPER, paramf["levelbar3"]);
        State::set(IndexBargraph::LEVEL_BAR_ANOTHER, paramf["levelbar4"]);
        State::set(IndexBargraph::LEVEL_BAR_INSANE, paramf["levelbar5"]);

        State::set(IndexOption::COURSE_TYPE, param["coursetype"]);
        State::set(IndexOption::COURSE_STAGE_COUNT, param["coursestagecount"]);
        State::set(IndexSwitch::COURSE_NOT_PLAYABLE, param["coursenotplayable"]);
        State::set(IndexSwitch::COURSE_STAGE1_CHART_EXIST, param["coursestagechartexist1"]);
        State::set(IndexSwitch::COURSE_STAGE2_CHART_EXIST, param["coursestagechartexist2"]);
        State::set(IndexSwitch::COURSE_STAGE3_CHART_EXIST, param["coursestagechartexist3"]);
        State::set(IndexSwitch::COURSE_STAGE4_CHART_EXIST, param["coursestagechartexist4"]);
        State::set(IndexSwitch::COURSE_STAGE5_CHART_EXIST, param["coursestagechartexist5"]);
        State::set(IndexNumber::COURSE_STAGE1_LEVEL, param["courselevel1"]);
        State::set(IndexNumber::COURSE_STAGE2_LEVEL, param["courselevel2"]);
        State::set(IndexNumber::COURSE_STAGE3_LEVEL, param["courselevel3"]);
        State::set(IndexNumber::COURSE_STAGE4_LEVEL, param["courselevel4"]);
        State::set(IndexNumber::COURSE_STAGE5_LEVEL, param["courselevel5"]);
        State::set(IndexOption::COURSE_STAGE1_DIFFICULTY, param["coursedifficulty1"]);
        State::set(IndexOption::COURSE_STAGE2_DIFFICULTY, param["coursedifficulty2"]);
        State::set(IndexOption::COURSE_STAGE3_DIFFICULTY, param["coursedifficulty3"]);
        State::set(IndexOption::COURSE_STAGE4_DIFFICULTY, param["coursedifficulty4"]);
        State::set(IndexOption::COURSE_STAGE5_DIFFICULTY, param["coursedifficulty5"]);
        State::set(IndexText::COURSE_STAGE1_TITLE, text["coursetitle1"]);
        State::set(IndexText::COURSE_STAGE2_TITLE, text["coursetitle2"]);
        State::set(IndexText::COURSE_STAGE3_TITLE, text["coursetitle3"]);
        State::set(IndexText::COURSE_STAGE4_TITLE, text["coursetitle4"]);
        State::set(IndexText::COURSE_STAGE5_TITLE, text["coursetitle5"]);
        State::set(IndexText::COURSE_STAGE1_SUBTITLE, text["coursesubtitle1"]);
        State::set(IndexText::COURSE_STAGE2_SUBTITLE, text["coursesubtitle2"]);
        State::set(IndexText::COURSE_STAGE3_SUBTITLE, text["coursesubtitle3"]);
        State::set(IndexText::COURSE_STAGE4_SUBTITLE, text["coursesubtitle4"]);
        State::set(IndexText::COURSE_STAGE5_SUBTITLE, text["coursesubtitle5"]);
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