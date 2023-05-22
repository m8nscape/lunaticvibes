#pragma once
#include "common/types.h"
#include "common/entry/entry.h"
#include "common/entry/entry_types.h"
#include "common/difficultytable/difficultytable.h"
#include "common/difficultytable/table_bms.h"
#include "game/graphics/texture_extra.h"


namespace lunaticvibes
{

struct ChartMetadata
{
    Path path{};
    HashMD5 hash{};
    std::shared_ptr<ChartFormatBase> chart;
    std::shared_ptr<ChartFormatBase> chartMybest;   // mybest obj is loaded with a different random seed

    //bool isChartSamplesLoaded;
    bool isSampleLoaded = false;
    HashMD5 sampleLoadedHash;
    bool isBgaLoaded = false;
    HashMD5 bgaLoadedHash;
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

typedef std::pair<std::shared_ptr<EntryBase>, std::shared_ptr<ScoreBase>> Entry;
typedef std::vector<Entry> EntryList;

struct SongListProperties
{
    HashMD5 parent;
    HashMD5 folder;
    std::string name;       // folder path, search query+result, etc.
    EntryList dbBrowseEntries;
    EntryList displayEntries;
    size_t index;
    bool ignoreFilters = false;
};

enum class SongListSortType
{
    DEFAULT,    // LEVEL
    TITLE,
    LEVEL,
    CLEAR,
    RATE,

    TYPE_COUNT,
};

enum class FilterDifficultyType
{
    All,
    B,
    N,
    H,
    A,
    I
};

enum class FilterKeysType
{
    All,
    _5,
    _7,
    _9,
    _10,
    _14,
    Single,
    Double,
};

inline struct Struct_SelectData
{
    std::unordered_map<std::string, long long> timers;

    std::shared_mutex _mutex;
    std::list<SongListProperties> backtrace;
    EntryList entries;
    size_t selectedEntryIndex = 0;     // current selected entry index
    double selectedEntryIndexRolling = 0.0;
    size_t highlightBarIndex = 0;  // highlighted bar index
    bool draggingListSlider = 0;    // is dragging slider

    bool coursePlayable = false;

    size_t cursorClick = 0;  // click bar
    int cursorClickScroll = 0;  // -1: scroll up / 1: scroll down / 2: decide
    bool cursorEnterPending = false;

    SongListSortType sortType = SongListSortType::DEFAULT;
    FilterDifficultyType filterDifficulty = FilterDifficultyType::All;
    FilterKeysType filterKeys = FilterKeysType::All;
    bool optionChangePending = false;

    bool panel[9] = { 0 };

    std::vector<DifficultyTableBMS> tables;

    double pitchSpeed = 1.0;

    int levelOfChartDifficulty[5] = { 0 };
    int countOfChartDifficulty[5] = { 0 };

    std::string barTitle[32];
    int barLevel[32] = { 0 };
    int newEntrySeconds = 600;

    PlayModifierLaneEffectType lastLaneEffectType1P = PlayModifierLaneEffectType::OFF;   // for LR2skin button SHUTTER

    unsigned scrollTimeLength = 300; // 
    int scrollDirection = 0;    // -1: up / 1: down

    std::string jukeboxName;

    bool isGoingToSkinSelect = false;
    bool isGoingToKeyConfig = false;
    bool isGoingToAutoPlay = false;
    bool isGoingToReplay = false;
    bool isGoingToReboot = false;

    ChartMetadata selectedChart;

    Entry getCurrentEntry();
    void loadSongList();
    void updateEntryScore(size_t idx);
    void sortSongList();
    void setBarInfo();
    void setEntryInfo();
    void setPlayModeInfo();
    void switchVersion(int difficulty);
    void setDynamicTextures();

} SelectData;


}