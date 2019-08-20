#pragma once
#include <memory>
#include <string>
#include "types.h"
#include "chart/chart.h"
#include "game/scroll/scroll.h"
#include "game/ruleset/ruleset.h"

enum class eScene
{
    NOTHINGNESS,
    SELECT,
    DECIDE,
    PLAY,
    RESULT,
    EXIT
};

inline eScene __next_scene = eScene::PLAY;

////////////////////////////////////////////////////////////////////////////////

inline struct __chart_context_params
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<vChart> chartObj{ nullptr };
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

} context_chart;

////////////////////////////////////////////////////////////////////////////////

constexpr size_t MAX_PLAYERS = 8;
constexpr size_t PLAYER_SLOT_1P = 0;
constexpr size_t PLAYER_SLOT_2P = 1;
inline struct __play_context_params
{
    eMode mode = eMode::PLAY7;
    bool canRetry = false;
    size_t playerSlot = PLAYER_SLOT_1P;  // 1P starts from 0
    unsigned judgeLevel = 0;

    std::shared_ptr<vScroll> scrollObj[2]{ nullptr, nullptr };

    // gauge/score graph key points
    // managed by SLOT, which includes local battle 1P/2P and multi battle player slots
    // maximum slot is MAX_PLAYERS
    std::array<std::vector<int>, MAX_PLAYERS> graphGauge;
    std::array<std::vector<int>, MAX_PLAYERS> graphScore;
    std::array<eGaugeOp, MAX_PLAYERS> gaugeType{};        // resolve on ruleset construction
    std::array<PlayMod, MAX_PLAYERS> mods{};         // eMod: 

    eRuleset rulesetType = eRuleset::CLASSIC;
    std::array<std::shared_ptr<vRuleset>, MAX_PLAYERS> ruleset;

    timestamp remainTime;

} context_play;

void clearContextPlayForRetry();
void clearContextPlay();

////////////////////////////////////////////////////////////////////////////////

struct SelectSongInfos
{
    unsigned type = 0; // song / folder / custom_folder / new_song / rival / rival_song / course_folder / new_course / course / random_course
    std::string title = "";
    double level = 0.0;
    unsigned level_type = 0;
    unsigned lamp = 0;
    unsigned rank = 0;
    unsigned rival = 3; // win / lose / draw / noplay
    unsigned rival_lamp_self = 0;
    unsigned rival_lamp_rival = 0;
};

inline struct __select_context_params
{
    unsigned barIndex;
    std::vector<SelectSongInfos> info;
} context_select;

void updateContextTitles();