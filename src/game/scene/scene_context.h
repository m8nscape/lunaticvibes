#pragma once
#include "types.h"
#include "chart/chart.h"
#include <memory>
#include <string>

inline struct __chart_context_params
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<vChart> chartObj{nullptr};
    //bool isChartSamplesLoaded;
    bool isSampleLoaded = false;
    bool isBgaLoaded = false;

    // For displaying purpose, typically fetch from song db directly
    StringContent title{};
    StringContent title2{};
    StringContent artist{};
    StringContent artist2{};
    StringContent genre{};
    StringContent version{};
    double level = 0.0;

    BPM minBPM = 150;
    BPM itlBPM = 150;
    BPM maxBPM = 150;

} context_chart;
