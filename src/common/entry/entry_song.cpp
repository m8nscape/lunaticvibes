#include "entry_song.h"

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
}
