#pragma once
#include <memory>
#include <string>
#include <stack>
#include <mutex>
#include "types.h"
#include "chartformat/chartformat.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/graphics/texture_extra.h"
#include "entry/entry_folder.h"
#include "db/db_song.h"

enum class eScene
{
    NOTHINGNESS,
    SELECT,
    DECIDE,
    PLAY,
    RESULT,
    EXIT
};

inline eScene __next_scene = eScene::SELECT;

////////////////////////////////////////////////////////////////////////////////

struct __chart_context_params
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<vChartFormat> chartObj;
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

};

////////////////////////////////////////////////////////////////////////////////

constexpr size_t MAX_PLAYERS = 8;
constexpr size_t PLAYER_SLOT_1P = 0;
constexpr size_t PLAYER_SLOT_2P = 1;
struct __play_context_params
{
    eMode mode = eMode::PLAY7;
    bool canRetry = false;
    bool isCourse = false;
    bool isCourseFirstStage = false;
    size_t playerSlot = PLAYER_SLOT_1P;  // 1P starts from 0
    unsigned judgeLevel = 0;

    std::shared_ptr<vChart> chartObj[2]{ nullptr, nullptr };
    double initialHealth[2]{ 1.0, 1.0 };

	std::shared_ptr<TextureBmsBga> bgaTexture = std::make_shared<TextureBmsBga>();

    // gauge/score graph key points
    // managed by SLOT, which includes local battle 1P/2P and multi battle player slots
    // maximum slot is MAX_PLAYERS
    std::array<std::vector<int>, MAX_PLAYERS> graphGauge;
    std::array<std::vector<int>, MAX_PLAYERS> graphScore;
    std::array<eGaugeOp, MAX_PLAYERS> gaugeType{};        // resolve on ruleset construction
    std::array<PlayMod, MAX_PLAYERS> mods{};         // eMod: 

    eRuleset rulesetType = eRuleset::CLASSIC;
    std::array<std::shared_ptr<vRuleset>, MAX_PLAYERS> ruleset;

    Time remainTime;

};

void clearContextPlayForRetry();
void clearContextPlay();

////////////////////////////////////////////////////////////////////////////////

struct SongListProperties
{
    HashMD5 parent;
    HashMD5 folder;
    std::string name;       // folder path, search query+result, etc.
    std::vector<std::shared_ptr<vEntry>> list;
    unsigned index;
};

enum class SongListSort
{
    DEFAULT,    // LEVEL
    TITLE,
};

struct __select_context_params
{
    std::mutex _mutex;
    std::stack<SongListProperties> backtrace;
    std::vector<std::shared_ptr<vEntry>> entries;
    size_t idx = 0;
    size_t cursor = 0;
    SongListSort sort = SongListSort::DEFAULT;
    unsigned difficulty = 0; // all / B / N / H / A / I (type 0 is not included)
    unsigned gamemode = 0; // all / 5, 7, 9, 10, 14, etc

};

void updateContextSelectTitles();

extern bool _quit_on_finish;
extern __chart_context_params context_chart;
extern __play_context_params context_play;
extern __select_context_params context_select;
extern std::shared_ptr<SongDB> pSongDB;