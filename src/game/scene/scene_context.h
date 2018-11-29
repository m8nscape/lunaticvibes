#pragma once
#include "types.h"
#include "chart/chart.h"
#include <memory>
#include <string>

inline struct
{
    Path path;
    HashMD5 hash;
    std::shared_ptr<vChart> chartObj;
    bool isChartSamplesLoaded;

    // For displaying purpose
    StringContent title;
    StringContent title2;
    StringContent artist;
    StringContent artist2;
    StringContent genre;
    StringContent version;
    double level;

    BPM minBPM;
    BPM itlBPM;
    BPM maxBPM;

} context_chart;
