#include "entry_song.h"

std::shared_ptr<vChartFormat> FolderSong::getChart(size_t idx)
{
    idx %= getContentsCount();
    return charts[idx];
}
std::shared_ptr<vChartFormat> FolderSong::getCurrentChart()
{
    if (charts.empty()) return nullptr;
    idx %= getContentsCount();
    return charts[idx];
}

size_t FolderSong::incCurrentChart()
{
    idx++;
    idx %= getContentsCount();
    return idx;
}

void FolderSong::pushChart(std::shared_ptr<vChartFormat> c)
{
    charts.push_back(c);
}
