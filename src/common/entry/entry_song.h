#pragma once
#include <memory>
#include "entry.h"
#include "common/chartformat/chartformat.h"

class Song : public vEntry
{
public:
    std::shared_ptr<vChartFormat> _file;
    unsigned lamp = 0;
    unsigned rank = 0;

    // extend info
    unsigned rival = 3; // win / lose / draw / noplay
    unsigned rival_lamp_self = 0;
    unsigned rival_lamp_rival = 0;
    bool have_more = false;

public:
    Song() = delete;
    Song(std::shared_ptr<vChartFormat> p): _file(p)
    {
        _type = eEntryType::SONG;
        _name = _file->title;
        _name2 = _file->title2;
    }
    Song(vChartFormat& f) : Song(std::make_shared<vChartFormat>(f)) {}
};