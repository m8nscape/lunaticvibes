#include "common/pch.h"
#include "data_select.h"
#include "data_play.h"
#include "data_shared.h"
#include "config/config_mgr.h"
#include "common/chartformat/chartformat_types.h"
#include "db/db_score.h"
#include "db/db_song.h"
#include "game/replay/replay_chart.h"
#include "game/runtime/i18n.h"

namespace lunaticvibes
{

extern std::shared_ptr<SongDB> g_pSongDB;
extern std::shared_ptr<ScoreDB> g_pScoreDB;

Entry Struct_SelectData::getCurrentEntry()
{
	if (selectedEntryIndex >= entries.size())
		return {};

	return entries[selectedEntryIndex];
}



void Struct_SelectData::loadSongList()
{
    HashMD5 currentEntryHash;
    std::shared_ptr<EntryFolderSong> currentEntrySong;
    int currentEntryGamemode = 0;
    int currentEntryDifficulty = 0;
    if (!entries.empty())
    {
        currentEntryHash = entries.at(selectedEntryIndex).first->md5;

        if (entries[selectedEntryIndex].first->type() == eEntryType::CHART ||
            entries[selectedEntryIndex].first->type() == eEntryType::RIVAL_CHART)
        {
            auto& en = entries[selectedEntryIndex].first;
            auto ps = std::reinterpret_pointer_cast<EntryChart>(en);
            auto pf = std::reinterpret_pointer_cast<ChartFormatBase>(ps->_file);
            currentEntrySong = ps->getSongEntry();
            currentEntryGamemode = pf->gamemode;
            currentEntryDifficulty = pf->difficulty;
        }
    }

    entries.clear();
    for (auto& [e, s] : backtrace.front().dbBrowseEntries)
    {
        // TODO replace name/name2 by tag.db

        // apply filter
        auto checkFilterKeys = [this](int keys)
        {
            if ((keys == 10 || keys == 14) && ConfigMgr::get("P", cfg::P_IGNORE_DP_CHARTS, false))
            {
                return false;
            }
            if (keys == 9 && ConfigMgr::get("P", cfg::P_IGNORE_9KEYS_CHARTS, false))
            {
                return false;
            }

            if (PlayData.battleType != PlayModifierBattleType::DoubleBattle)
            {
                // not DB, filter as usual
                switch (filterKeys)
                {
                case FilterKeysType::All: return true;
                case FilterKeysType::_5: return keys == 5;
                case FilterKeysType::_7: return keys == 7;
                case FilterKeysType::_9: return keys == 9;
                case FilterKeysType::_10: return keys == 10;
                case FilterKeysType::_14: return keys == 14;
                case FilterKeysType::Single: return keys == 5 || keys == 7;
                case FilterKeysType::Double: return keys == 10 || keys == 14;
                default: return false;
                }
            }
            else
            {
                // DB, only display SP charts
                switch (filterKeys)
                {
                case FilterKeysType::All:
                case FilterKeysType::_5: return keys == 5;
                case FilterKeysType::_7: return keys == 7;
                case FilterKeysType::_9: return keys == 9;
                case FilterKeysType::_10: return keys == 5;
                case FilterKeysType::_14: return keys == 10;
                case FilterKeysType::Single: 
                case FilterKeysType::Double: return keys == 5 || keys == 7;
                default: return false;
                }
            }
        };
        auto checkFilterDifficulty = [this](int difficulty)
        {
            if (filterDifficulty == FilterDifficultyType::All) return true;
            switch (filterDifficulty)
            {
                case FilterDifficultyType::All: return true;
                case FilterDifficultyType::B: return difficulty == 1;
                case FilterDifficultyType::N: return difficulty == 2;
                case FilterDifficultyType::H: return difficulty == 3;
                case FilterDifficultyType::A: return difficulty == 4;
                case FilterDifficultyType::I: return difficulty == 5;
                default: return false;
            }
        };
        bool skip = false;
        switch (e->type())
        {
        case eEntryType::SONG:
        case eEntryType::RIVAL_SONG:
        {
            auto f = std::reinterpret_pointer_cast<EntryFolderSong>(e);

            bool have7k = false;
            bool have14k = false;
            if (ConfigMgr::get("P", cfg::P_IGNORE_5KEYS_IF_7KEYS_EXIST, false))
            {
                for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
                {
                    auto pBase = f->getChart(idx);
                    if (pBase->gamemode == 7) have7k = true;
                    if (pBase->gamemode == 14) have14k = true;
                }
            }

            for (size_t idx = 0; idx < f->getContentsCount() && !skip; ++idx)
            {
                auto pBase = f->getChart(idx);

                if (ConfigMgr::get("P", cfg::P_IGNORE_5KEYS_IF_7KEYS_EXIST, false))
                {
                    if (pBase->gamemode == 5 && have7k) continue;
                    if (pBase->gamemode == 10 && have14k) continue;
                }
                if (!backtrace.front().ignoreFilters)
                {
                    if (!checkFilterDifficulty(pBase->difficulty)) continue;
                    if (!checkFilterKeys(pBase->gamemode)) continue;
                }

                switch (f->getChart(idx)->type())
                {
                case eChartFormat::BMS:
                {
                    auto p = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(f->getChart(idx));

                    // add all charts as individual entries into list.
                    entries.push_back({ std::make_shared<EntryChart>(p, f), nullptr });
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

                if (!backtrace.front().ignoreFilters)
                {
                    if (!checkFilterDifficulty(p->difficulty)) continue;
                    if (!checkFilterKeys(p->gamemode)) continue;
                }

                // filters are matched
                entries.push_back({ e, nullptr });
                break;
            }
            break;
        }

        default:
            entries.push_back({ e, nullptr });
            break;
        }
    }

    if (backtrace.front().ignoreFilters)
    {
        // change display only
        SelectData.filterDifficulty = FilterDifficultyType::All;
        SelectData.filterKeys = FilterKeysType::All;
    }
    else
    {
        // restore prev
        cfg::loadFilterDifficulty();
        cfg::loadFilterKeys();
    }

    // load score
    for (size_t idx = 0; idx < entries.size(); ++idx)
    {
        updateEntryScore(idx);
    }

    selectedEntryIndex = 0;

    // look for the exact same entry
    if (backtrace.size() > 1)
    {
        auto findChart = [&](const HashMD5& hash)
        {
            for (size_t idx = 0; idx < entries.size(); ++idx)
            {
                if (hash == entries.at(idx).first->md5)
                {
                    return idx;
                }
            }
            return (size_t)-1;
        };

        size_t i = findChart(currentEntryHash);
        if (i != (size_t)-1)
        {
            selectedEntryIndex = i;
        }
        else if (currentEntrySong)
        {
            if (filterDifficulty != FilterDifficultyType::All)
            {
                // search from current difficulty
                const auto& chartList = currentEntrySong->getDifficultyList(currentEntryGamemode, unsigned(filterDifficulty));
                if (!chartList.empty())
                {
                    i = findChart((*chartList.begin())->fileHash);
                }
            }
            if (i != (size_t)-1)
            {
                selectedEntryIndex = i;
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
                selectedEntryIndex = i;
            }
            else
            {
                // search from any difficulties
                for (size_t diff = 1; diff <= 5; ++diff)
                {
                    if (currentEntryDifficulty == diff) continue;
                    if (currentEntryDifficulty == unsigned(filterDifficulty)) continue;
                    const auto& diffList = currentEntrySong->getDifficultyList(currentEntryGamemode, diff);
                    if (!diffList.empty())
                    {
                        i = findChart((*diffList.begin())->fileHash);
                    }
                }
            }
            if (i != (size_t)-1)
            {
                selectedEntryIndex = i;
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
                selectedEntryIndex = i;
            }
        }
    }

    SelectData.selectedEntryIndexRolling = entries.empty() ? 0.0 : ((double)selectedEntryIndex / entries.size());
}

void Struct_SelectData::updateEntryScore(size_t idx)
{
    auto& [entry, score] = entries[idx];

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

void Struct_SelectData::sortSongList()
{
    HashMD5 currentEntryHash;
    if (!entries.empty())
        currentEntryHash = entries.at(selectedEntryIndex).first->md5;

    std::sort(entries.begin(), entries.end(), [this](const Entry& entry1, const Entry& entry2)
        {
            auto& lhs = entry1.first;
            auto& rhs = entry2.first;
            if (lhs->type() != rhs->type())
            {
                return lhs->type() < rhs->type();
            }
            else if (lhs->type() == eEntryType::CUSTOM_FOLDER)
            {
                return entry1.first->md5 < entry2.first->md5;
            }
            else
            {
                std::shared_ptr<ChartFormatBase> l, r;
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
                    switch (sortType)
                    {
                    case SongListSortType::DEFAULT:
                        if (l->folderHash != r->folderHash) return l->folderHash < r->folderHash;
                        if (l->levelEstimated != r->levelEstimated) return l->levelEstimated < r->levelEstimated;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    case SongListSortType::TITLE:
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    case SongListSortType::LEVEL:
                        if (l->levelEstimated != r->levelEstimated) return l->levelEstimated < r->levelEstimated;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    case SongListSortType::CLEAR:
                    {
                        auto l_lamp = std::dynamic_pointer_cast<ScoreBMS>(entry1.second) ? std::reinterpret_pointer_cast<ScoreBMS>(entry1.second)->lamp : LampType::NOPLAY;
                        auto r_lamp = std::dynamic_pointer_cast<ScoreBMS>(entry2.second) ? std::reinterpret_pointer_cast<ScoreBMS>(entry2.second)->lamp : LampType::NOPLAY;
                        if (l_lamp != r_lamp) return l_lamp < r_lamp;
                        if (l->title != r->title) return l->title < r->title;
                        if (l->title2 != r->title2) return l->title2 < r->title2;
                        if (l->version != r->version) return l->version < r->version;
                        break;
                    }
                    case SongListSortType::RATE:
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

    for (size_t idx = 0; idx < entries.size(); ++idx)
    {
        if (currentEntryHash == entries.at(idx).first->md5)
        {
            selectedEntryIndex = idx;
            break;
        }
    }
    if (selectedEntryIndex >= entries.size())
    {
        selectedEntryIndex = 0;
    }
    SelectData.selectedEntryIndexRolling = entries.empty() ? 0.0 : ((double)selectedEntryIndex / entries.size());
}

void Struct_SelectData::setBarInfo()
{
    const EntryList& e = entries;
    if (e.empty()) return;

    const size_t idx = selectedEntryIndex;
    const size_t cursor = highlightBarIndex;
    const size_t count = 32;
    const bool subtitle = !ConfigMgr::get('P', cfg::P_ONLY_DISPLAY_MAIN_TITLE_ON_BARS, false);

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
                if (subtitle)
                {
                    if (!name.empty()) name += " ";
                    if (!entry->_name2.empty()) name += entry->_name2;
                }
                barTitle[bar_index] = name;
                barLevel[bar_index] = bms->playLevel;

                break;
            }

            default:
                barTitle[bar_index] = entry->_name;
                barLevel[bar_index] = 0;
                break;
            }
        }
        else
        {
            // other types. eg. folder, course, etc
            barTitle[bar_index] = entry->_name;
            barLevel[bar_index] = 0;
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

void Struct_SelectData::setEntryInfo()
{
    const EntryList& e = entries;
    if (e.empty()) return;

    setPlayModeInfo();
}

void Struct_SelectData::setPlayModeInfo()
{
}

void Struct_SelectData::switchVersion(int difficulty)
{
    const EntryList& e = entries;
    if (e.empty()) return;

    const size_t idx = selectedEntryIndex;
    const size_t cursor = highlightBarIndex;

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
            std::shared_ptr<ChartFormatBase> pNextChart = nullptr;
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
                            selectedEntryIndex = nextIdx;
                            break;
                        }
                    }
                }
                SelectData.selectedEntryIndexRolling = entries.empty() ? 0.0 : ((double)selectedEntryIndex / entries.size());
            }
            */

            // choose directly from entry list
            std::shared_ptr<ChartFormatBase> pFirstChart = nullptr;
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
                        else if (currentFound && pns->_file->gamemode == pf->gamemode && (difficulty == 0 || pns->_file->difficulty == difficulty))
                        {
                            selectedEntryIndex = nextIdx;
                            SelectData.selectedEntryIndexRolling = entries.empty() ? 0.0 : ((double)selectedEntryIndex / entries.size());
                            return;
                        }
                    }
                }
            }
            // next chart not found, search once again
            if (pFirstChart)
            {
                for (size_t nextIdx = 0; nextIdx < e.size(); ++nextIdx)
                {
                    if (e[nextIdx].first->type() == eEntryType::CHART || e[nextIdx].first->type() == eEntryType::RIVAL_CHART)
                    {
                        auto pns = std::reinterpret_pointer_cast<EntryChart>(e[nextIdx].first);
                        if (pns->getSongEntry() == ps->getSongEntry())
                        {
                            if (pns->_file != pf && pns->_file->gamemode == pf->gamemode && (difficulty == 0 || pns->_file->difficulty == difficulty))
                            {
                                selectedEntryIndex = nextIdx;
                                SelectData.selectedEntryIndexRolling = entries.empty() ? 0.0 : ((double)selectedEntryIndex / entries.size());
                                return;
                            }
                        }
                    }
                }

                // fallback to first entry
                selectedEntryIndex = firstIdx;
                SelectData.selectedEntryIndexRolling = entries.empty() ? 0.0 : ((double)selectedEntryIndex / entries.size());
            }
        }
    }
}

void Struct_SelectData::setDynamicTextures()
{
    selectedChart.texStagefile.setPath("");
    selectedChart.texBackbmp.setPath("");
    selectedChart.texBanner.setPath("");

    const EntryList& e = entries;
    if (e.empty()) return;

    const size_t idx = selectedEntryIndex;
    const size_t cursor = highlightBarIndex;

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
            selectedChart.texStagefile.setPath(pf->getDirectory() / PathFromUTF8(pf->stagefile));

        // backbmp
        if (!pf->backbmp.empty())
            selectedChart.texBackbmp.setPath(pf->getDirectory() / PathFromUTF8(pf->backbmp));

        // _banner
        if (!pf->banner.empty())
            selectedChart.texBanner.setPath(pf->getDirectory() / PathFromUTF8(pf->banner));
    }
    break;
    }
}


}