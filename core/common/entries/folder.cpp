#include "folder.h"
#include "chart/chart.h"


std::shared_ptr<vFolder> FolderRegular::getEntry(int idx)
{
    idx %= getContentsCount();
    return entries[idx];
}

void FolderRegular::pushEntry(std::shared_ptr<vFolder> f)
{
    entries.push_back(f);
}


std::shared_ptr<vChart> FolderSong::getChart(int idx)
{
    idx %= getContentsCount();
    return charts[idx];
}

void FolderSong::pushChart(std::shared_ptr<vChart> c)
{
    charts.push_back(c);
}