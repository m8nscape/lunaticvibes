#pragma once
#include "types.h"
#include "chart/chart.h"
#include "game/scroll/scroll.h"
#include <memory>
#include <string>

inline struct __chart_context_params
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<vChart> chartObj{ nullptr };
    std::shared_ptr<vScroll> scrollObj{ nullptr };
    //bool isChartSamplesLoaded;
    bool isSampleLoaded = false;
    bool isBgaLoaded = false;
	bool started = false;

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

	std::vector<int> graphGauge1P;
	std::vector<int> graphGauge2P;
	std::vector<int> graphScore1P;
	std::vector<int> graphScore2P;
	std::vector<int> graphScoreBest;

} context_chart;
