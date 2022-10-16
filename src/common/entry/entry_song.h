#pragma once
#include <memory>
#include <boost/format.hpp>
#include "entry_folder.h"
#include "common/chartformat/chartformat.h"

// entry for individual song, e.g. jukebox/bms/L9
class EntryFolderSong : public EntryFolderBase
{
public:
    EntryFolderSong() = delete;
    EntryFolderSong(HashMD5 md5, const Path& path, const StringContent& name = "", const StringContent& name2 = "") :
        EntryFolderBase(md5, path)
    {
        _type = eEntryType::SONG;
        _name = name;
        _name2 = name2;
    }

protected:
    std::vector<std::shared_ptr<ChartFormatBase>> charts;
    size_t idx = 0;

public:
    std::shared_ptr<ChartFormatBase> getChart(size_t idx);
    std::shared_ptr<ChartFormatBase> getCurrentChart();
    size_t incCurrentChart();
    void pushChart(std::shared_ptr<ChartFormatBase> c);
    virtual size_t getContentsCount() { return charts.size(); }
    virtual bool empty() { return charts.empty(); }

protected:
    std::map<int, std::map<unsigned, std::vector<std::shared_ptr<ChartFormatBase>>>> chartMap;
public:
    const std::vector<std::shared_ptr<ChartFormatBase>>& getDifficultyList(int gamemode, unsigned difficulty) const;
};

// entry for each charts of song, e.g. jukebox/bms/L9/kuso9_7.bme
class EntryChart : public EntryBase
{
public:
    std::shared_ptr<ChartFormatBase> _file = nullptr;

    // extend info
    std::shared_ptr<EntryFolderSong> _song = nullptr;

public:
    EntryChart() = default;
    EntryChart(std::shared_ptr<ChartFormatBase> p, std::shared_ptr<EntryFolderSong> ps = nullptr) : _file(p), _song(ps)
    {
        _type = eEntryType::CHART;
        md5 = _file->fileHash;
        _name = _file->title;
        _name2 = _file->title2;

        _addTime = _file->addTime;
    }
    EntryChart(ChartFormatBase& f) : EntryChart(std::make_shared<ChartFormatBase>(f)) {}

    std::shared_ptr<EntryFolderSong> getSongEntry() const { return _song; }
};

// entry for tables
class EntryChartLink : public EntryChart
{
public:
    std::string urlBase;
    std::string urlChart;

public:
    EntryChartLink(const std::string& md5, const std::string& urlBase, const std::string urlChart)
    {
        _type = eEntryType::CHART_LINK;
        this->md5 = md5;
        this->urlBase = urlBase;
        this->urlChart = urlChart;
    }
};