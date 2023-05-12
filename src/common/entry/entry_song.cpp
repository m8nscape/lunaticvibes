#include "entry_song.h"

namespace lunaticvibes
{

std::shared_ptr<ChartFormatBase> EntryFolderSong::getChart(size_t idx)
{
    idx %= getContentsCount();
    return charts[idx];
}
std::shared_ptr<ChartFormatBase> EntryFolderSong::getCurrentChart()
{
    if (charts.empty()) return nullptr;
    idx %= getContentsCount();
    return charts[idx];
}

size_t EntryFolderSong::incCurrentChart()
{
    idx++;
    idx %= getContentsCount();
    return idx;
}

void EntryFolderSong::pushChart(std::shared_ptr<ChartFormatBase> c)
{
    charts.push_back(c);
    chartMap[c->gamemode][c->difficulty].push_back(c);
}

const std::vector<std::shared_ptr<ChartFormatBase>>& EntryFolderSong::getDifficultyList(int gamemode, unsigned difficulty) const
{
    if (chartMap.find(gamemode) != chartMap.end())
    {
        const auto& diffMap = chartMap.at(gamemode);
        if (diffMap.find(difficulty) != diffMap.end())
            return diffMap.at(difficulty);
    }

    static const std::vector<std::shared_ptr<ChartFormatBase>> emptyList;
    return emptyList;
}

}
