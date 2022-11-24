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
#include "game/replay/replay_chart.h"
#include "common/entry/entry_folder.h"
#include "db/db_song.h"
#include "db/db_score.h"
#include "common/difficultytable/table_bms.h"

inline eScene gNextScene = eScene::SELECT;
inline bool gInCustomize = false;
inline bool gCustomizeSceneChanged = false;
inline bool gExitingCustomize = false;

////////////////////////////////////////////////////////////////////////////////

struct ChartContextParams
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<ChartFormatBase> chartObj;
    std::shared_ptr<ChartFormatBase> chartObjMybest;

    //bool isChartSamplesLoaded;
    bool isSampleLoaded = false;
    bool isBgaLoaded = false;
	bool started = false;

    // DP flags
    bool isDoubleBattle = false;

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

constexpr unsigned MAX_PLAYERS = 3;
constexpr unsigned PLAYER_SLOT_PLAYER = 0;
constexpr unsigned PLAYER_SLOT_TARGET = 1;
constexpr unsigned PLAYER_SLOT_MYBEST = 2;
struct PlayContextParams
{
    eMode mode = eMode::PLAY7;
    bool canRetry = false;

    unsigned judgeLevel = 0;

	std::shared_ptr<TextureBmsBga> bgaTexture = std::make_shared<TextureBmsBga>();

    std::array<std::shared_ptr<ChartObjectBase>, MAX_PLAYERS> chartObj{ nullptr, nullptr, nullptr };
    std::array<double, MAX_PLAYERS> initialHealth{ 1.0, 1.0, 1.0 };
    std::array<std::vector<int>, MAX_PLAYERS> graphGauge;
    std::array<std::vector<double>, MAX_PLAYERS> graphAcc;
    std::array<eGaugeOp, MAX_PLAYERS> gaugeType{};        // resolve on ruleset construction
    std::array<PlayMod, MAX_PLAYERS> mods{};         // eMod: 

    eRuleset rulesetType = eRuleset::BMS;
    std::array<std::shared_ptr<vRuleset>, MAX_PLAYERS> ruleset;

    std::shared_ptr<ReplayChart> replay;
    std::shared_ptr<ReplayChart> replayMybest;
    std::shared_ptr<ReplayChart> replayNew;

    bool isCourse = false;
    int courseStage = 0;
    HashMD5 courseHash;
    std::vector<HashMD5> courseCharts;
    std::vector<std::shared_ptr<vRuleset>> courseStageRulesetCopy[2];
    std::vector<Path> courseStageReplayPath;
    std::vector<Path> courseStageReplayPathNew;
    unsigned courseRunningCombo[2] = { 0, 0 };
    unsigned courseMaxCombo[2] = { 0, 0 };

    Time remainTime;

    uint64_t randomSeed;

    bool isAuto = false;
    bool isReplay = false;
    bool haveReplay = false;

    double Hispeed = 2.0;

    // BATTLE 2P side settings
    bool isBattle = false;  // Note: DB is NOT Battle
    double battle2PHispeed = 2.0;
    int battle2PLanecoverTop = 0;
    int battle2PLanecoverBottom = 0;
    int battle2PGreenNumber = 1200;

    Time HispeedGradientStart;
    double HispeedGradientFrom = 2.0;
    double HispeedGradientNow = 2.0;
    Time battle2PHispeedGradientStart;
    double battle2PHispeedGradientFrom = 2.0;
    double battle2PHispeedGradientNow = 2.0;
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
    bool entryDragging = 0;    // is dragging slider

    size_t cursorClick = 0;  // click bar
    int cursorClickScroll = 0;  // -1: scroll up / 1: scroll down / 2: decide
    bool cursorEnter = false;

    size_t sameDifficultyNextIdx = 0; // next entry index of same difficulty

    SongListSort sort = SongListSort::DEFAULT;
    unsigned filterDifficulty = 0; // all / B / N / H / A / I (type 0 is not included)
    unsigned filterKeys = 0; // all / 5, 7, 9, 10, 14, etc
    bool optionChanged = false;

    std::vector<DifficultyTableBMS> tables;

    double pitchSpeed = 1.0;

    unsigned scrollTimeLength = 300; // 
    int scrollDirection = 0;    // -1: up / 1: down

    int panel = 0;

    bool isGoingToSkinSelect = false;
    bool isGoingToKeyConfig = false;
    bool isGoingToAutoPlay = false;
    bool isGoingToReplay = false;
    bool isGoingToReboot = false;
};

void loadSongList();
void updateEntryScore(size_t idx);
void sortSongList();
void setBarInfo();
void setEntryInfo();
void setPlayModeInfo();
void switchVersion(int difficulty);

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
    bool modeUpdate = false;

    int skinDir = 0;

    bool optionUpdate = false;
    size_t optionIdx;
    int optionDir = 0;

    bool optionDragging = false;
};

////////////////////////////////////////////////////////////////////////////////

struct UpdateContextParams
{
    // shared
    Time updateTime;

    // vSkin / Sprite
    double metre;
    unsigned bar;

    int liftHeight1P = 0;
    int liftHeight2P = 0;
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