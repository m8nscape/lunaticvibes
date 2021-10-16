#pragma once
#include <memory>
#include "entry.h"
#include "common/chartformat/chartformat.h"

class Song : public vEntry
{
public:
    std::shared_ptr<vChartFormat> _file;

    // extend info
    bool have_more = false;

public:
    Song() = delete;
    Song(std::shared_ptr<vChartFormat> p): _file(p)
    {
        _type = eEntryType::CHART;
        md5 = _file->fileHash;
        _name = _file->title;
        _name2 = _file->title2;

        _addTime = _file->addTime;
    }
    Song(vChartFormat& f) : Song(std::make_shared<vChartFormat>(f)) {}
};