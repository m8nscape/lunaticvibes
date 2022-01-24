#pragma once
#include <memory>
#include "entry_folder.h"
#include "common/chartformat/chartformat.h"

class FolderSong : public vFolder
{
public:
    FolderSong() = delete;
    FolderSong(HashMD5 md5, const Path& path, const StringContent& name = "", const StringContent& name2 = "") :
        vFolder(eFolderType::SONG, md5, path)
    {
        _name = name;
        _name2 = name2;
    }

protected:
    std::vector<std::shared_ptr<vChartFormat>> charts;
    size_t idx = 0;

public:
    std::shared_ptr<vChartFormat> getChart(size_t idx);
    std::shared_ptr<vChartFormat> getCurrentChart();
    size_t incCurrentChart();
    void pushChart(std::shared_ptr<vChartFormat> c);
    virtual size_t getContentsCount() { return charts.size(); }
    virtual bool empty() { return charts.empty(); }
};

class EntryChart : public vEntry
{
public:
    std::shared_ptr<vChartFormat> _file;

    // extend info
    bool have_more = false;

public:
    EntryChart() = delete;
    EntryChart(std::shared_ptr<vChartFormat> p): _file(p)
    {
        _type = eEntryType::CHART;
        md5 = _file->fileHash;
        _name = _file->title;
        _name2 = _file->title2;

        _addTime = _file->addTime;
    }
    EntryChart(vChartFormat& f) : EntryChart(std::make_shared<vChartFormat>(f)) {}
};