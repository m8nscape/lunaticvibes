#pragma once
#include <memory>
#include <string>
#include <stack>
#include <shared_mutex>
#include "common/types.h"
#include "common/chartformat/chartformat.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/graphics/texture_extra.h"
#include "common/entry/entry_folder.h"
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

inline eScene gNextScene = eScene::SELECT;

////////////////////////////////////////////////////////////////////////////////

struct ChartContextParams
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
    BPM maxBPM = 150;
    BPM startBPM = 150;

    TextureDynamic texStagefile;
    TextureDynamic texBackbmp;
    TextureDynamic texBanner;
};

////////////////////////////////////////////////////////////////////////////////

constexpr unsigned MAX_PLAYERS = 8;
constexpr unsigned PLAYER_SLOT_1P = 0;
constexpr unsigned PLAYER_SLOT_2P = 1;
struct PlayContextParams
{
    eMode mode = eMode::PLAY7;
    bool canRetry = false;
    bool isCourse = false;
    bool isCourseFirstStage = false;
    unsigned judgeLevel = 0;

    std::shared_ptr<vChart> chartObj[2]{ nullptr, nullptr };
    double initialHealth[2]{ 1.0, 1.0 };

	std::shared_ptr<TextureBmsBga> bgaTexture = std::make_shared<TextureBmsBga>();

    // gauge/score graph key points
    // managed by SLOT, which includes local battle 1P/2P and multi battle player slots
    // maximum slot is MAX_PLAYERS
    std::array<std::vector<int>, MAX_PLAYERS> graphGauge;
    std::array<std::vector<int>, MAX_PLAYERS> graphScore;
    std::vector<int> graphScoreTarget;
    std::array<eGaugeOp, MAX_PLAYERS> gaugeType{};        // resolve on ruleset construction
    std::array<PlayMod, MAX_PLAYERS> mods{};         // eMod: 

    // TODO FLIP

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
    size_t index;
};

enum class SongListSort
{
    DEFAULT,    // LEVEL
    TITLE,
};

struct SelectContextParams
{
    std::shared_mutex _mutex;
    std::stack<SongListProperties> backtrace;
    std::vector<std::shared_ptr<vEntry>> entries;
    size_t idx = 0;
    size_t cursor = 0;
    SongListSort sort = SongListSort::DEFAULT;
    unsigned difficulty = 0; // all / B / N / H / A / I (type 0 is not included)
    unsigned gamemode = 0; // all / 5, 7, 9, 10, 14, etc
    unsigned scrollTime = 300; // 
};

void updateContextSelectTitles();

extern bool gResetSelectCursor;
extern bool gQuitOnFinish;
extern ChartContextParams gChartContext;
extern PlayContextParams gPlayContext;
extern SelectContextParams gSelectContext;
extern std::shared_ptr<SongDB> g_pSongDB;

////////////////////////////////////////////////////////////////////////////////

void pushMainThreadTask(std::function<void()> f);
void doMainThreadTask();