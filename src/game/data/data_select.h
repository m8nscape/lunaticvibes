#pragma once
#include "data_internal.h"
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

struct Struct_SelectData;
class SongListManager
{
public:
    struct Entry
    {
        std::shared_ptr<EntryBase> entry;
        std::shared_ptr<ScoreBase> score;
    };
    struct List
    {
        HashMD5 parent;
        HashMD5 folder;
        std::string name;       // folder path, search query+result, etc.
        std::vector<std::shared_ptr<Entry>> dbBrowseEntries;
        std::vector<std::shared_ptr<Entry>> displayEntries;
        size_t index;
        bool ignoreFilters = false;
    };

protected:
    // typical std::shared_mutex will cause write lock starvation in our use case
    class Lock
    {
    private:
        std::atomic<bool> lockedForWrite = false;
        std::atomic<int> lockedForRead = 0;
        std::thread::id mid;
        std::shared_mutex lockMutex;
    public:
        Lock() = default;
        ~Lock();
        void lockForWrite();
        void lockForRead();
        void unlock();
        bool isLocked() const;
    } mutable lock;
    class LockReadGuard
    {
    private:
        Lock* l;
    public:
        LockReadGuard(Lock* l) : l(l) { l->lockForRead(); }
        ~LockReadGuard() { l->unlock(); }
    };
    class LockWriteGuard
    {
    private:
        Lock* l;
    public:
        LockWriteGuard(Lock* l) : l(l) { l->lockForWrite(); }
        ~LockWriteGuard() { l->unlock(); }
    };

private:
    std::deque<std::shared_ptr<List>> backtrace;

public:
    // used for scrolling animation interp
    double selectedEntryIndexRolling = 0.0;

    // hghlighted bar index (0~31).
    // We only have 32 bar instances internally
    size_t highlightBarIndex = 0;

public:
    void initialize();
    void append(std::shared_ptr<List>);
    void pop();

    bool isModifying() const { return lock.isLocked(); }
    std::shared_ptr<List> getList(size_t layer);
    std::shared_ptr<List> getCurrentList();
    size_t getCurrentIndex();
    std::shared_ptr<Entry> getCurrentEntry();
    size_t getBacktraceSize() const;

    void updateScore(const HashMD5& hash, std::shared_ptr<ScoreBase> score);

private:
    friend struct Struct_SelectData;
    void loadSongList();
    void sortSongList();
};

inline struct Struct_SelectData
{
    TimerStorage timers;

    SongListManager songList;
    std::vector<DifficultyTableBMS> tables;

    bool draggingListSlider = 0;    // is dragging slider

    bool coursePlayable = false;

    size_t cursorClick = 0;  // click bar
    int cursorClickScroll = 0;  // -1: scroll up / 1: scroll down / 2: decide
    bool cursorEnterPending = false;

    SongListSortType sortType = SongListSortType::DEFAULT;
    FilterDifficultyType filterDifficulty = FilterDifficultyType::All;
    FilterKeysType filterKeys = FilterKeysType::All;
    bool optionChangePending = false;

    bool panel[9] = { 0 };  // LR2 skin panel

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

    void updateEntryScore(size_t idx);
    void updateSongList(bool full);
    void switchVersion(int difficulty);

private:
    void setBarInfo();
    void setEntryInfo();
    void setPlayModeInfo();
    void setDynamicTextures();
    void resetJukeboxText();

} SelectData;


}