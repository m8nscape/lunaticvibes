#pragma once
#include <memory>
#include "entry.h"
#include "chartformat/chartformat.h"

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
    Song(vChartFormat& f) : _file(std::make_shared<vChartFormat>(f)) {}
};