#pragma once
#include <array>
#include <memory>
#include <string>
#include <stack>
#include <shared_mutex>
#include "scene.h"
#include "common/types.h"
#include "common/chartformat/chartformat.h"
#include "game/chart/chart.h"
#include "game/ruleset/ruleset.h"
#include "game/graphics/texture_extra.h"
#include "common/entry/entry_folder.h"
#include "db/db_song.h"
#include "db/db_score.h"
#include "common/difficultytable/table_bms.h"

inline eScene gNextScene = eScene::SELECT;

////////////////////////////////////////////////////////////////////////////////

struct ChartContextParams
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<ChartFormatBase> chartObj;
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
    double HSFixBPMFactor1P = 1.0;
    double HSFixBPMFactor2P = 1.0;

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

    std::shared_ptr<ChartObjectBase> chartObj[2]{ nullptr, nullptr };
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

    eRuleset rulesetType = eRuleset::BMS;
    std::array<std::shared_ptr<vRuleset>, MAX_PLAYERS> ruleset;

    Time remainTime;

    unsigned int randomSeedChart;
    unsigned int randomSeedMod;

    bool isAuto = false;

    double Hispeed = 2.0;

    // BATTLE 2P side settings
    bool isBattle = false;  // Note: DB is NOT Battle
    double battle2PHispeed = 2.0;
    bool battle2PLanecover = false;
    int battle2PLanecoverTop = 0;
    int battle2PLanecoverBottom = 0;
    bool battle2PLockSpeed = false;
    int battle2PGreenNumber = 1200;
};

std::pair<bool, Option::e_lamp_type> getSaveScoreType();
void clearContextPlayForRetry();
void clearContextPlay();

void pushGraphPoints();

////////////////////////////////////////////////////////////////////////////////

typedef std::pair<std::shared_ptr<EntryBase>, std::shared_ptr<vScore>> Entry;
typedef std::vector<Entry> EntryList;

struct SongListProperties
{
    HashMD5 parent;
    HashMD5 folder;
    std::string name;       // folder path, search query+result, etc.
    EntryList dbBrowseEntries;
    EntryList displayEntries;
    size_t index;
};

enum class SongListSort
{
    DEFAULT,    // LEVEL
    TITLE,
    LEVEL,
    CLEAR,
    RATE,

    TYPE_COUNT,
};

struct SelectContextParams
{
    std::shared_mutex _mutex;
    std::stack<SongListProperties> backtrace;
    EntryList entries;
    size_t idx = 0;     // current selected entry index
    size_t cursor = 0;  // highlighted bar index

    SongListSort sort = SongListSort::DEFAULT;
    unsigned filterDifficulty = 0; // all / B / N / H / A / I (type 0 is not included)
    unsigned filterKeys = 0; // all / 5, 7, 9, 10, 14, etc

    std::vector<DifficultyTableBMS> tables;

    double pitchSpeed = 1.0;

    unsigned scrollTimeLength = 300; // 
    int scrollDirection = 0;    // -1: up / 1: down

    int panel = 0;

    bool isGoingToSkinSelect = false;
    bool isGoingToKeyConfig = false;
    bool isGoingToAutoPlay = false;
    bool isGoingToReplay = false;
};

void loadSongList();
void sortSongList();
void setBarInfo();
void setEntryInfo();

void setDynamicTextures();

////////////////////////////////////////////////////////////////////////////////

struct KeyConfigContextParams
{
    GameModeKeys keys;
    std::pair<Input::Pad, int> selecting = { Input::Pad::K11, 0 };
};

////////////////////////////////////////////////////////////////////////////////

struct CustomizeContextParams
{
    eMode mode;

    int skinDir;

    bool optionUpdate;
    size_t optionIdx;
    int optionDir;
};

////////////////////////////////////////////////////////////////////////////////

struct UpdateContextParams
{
    // shared
    Time updateTime;

    // vSkin / Sprite
    double metre;
    unsigned bar;
};

////////////////////////////////////////////////////////////////////////////////

struct OverlayContextParams
{
    std::shared_mutex _mutex;
    std::list<std::pair<Time, StringContent>> notifications;

    std::vector<StringContent> popupList;
    size_t popupListSel = 0;
    bool popupListShow = false;
};

void createNotification(StringContentView text);

////////////////////////////////////////////////////////////////////////////////

extern bool gResetSelectCursor;
extern bool gQuitOnFinish;
extern bool gAppIsExiting;
extern ChartContextParams gChartContext;
extern PlayContextParams gPlayContext;
extern SelectContextParams gSelectContext;
extern KeyConfigContextParams gKeyconfigContext;
extern CustomizeContextParams gCustomizeContext;
extern UpdateContextParams gUpdateContext;
extern OverlayContextParams gOverlayContext;
extern std::shared_ptr<SongDB> g_pSongDB;
extern std::shared_ptr<ScoreDB> g_pScoreDB;

////////////////////////////////////////////////////////////////////////////////