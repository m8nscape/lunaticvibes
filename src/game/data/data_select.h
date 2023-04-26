#pragma once
#include "common/entry/entry.h"
#include "common/entry/entry_types.h"
#include "common/difficultytable/difficultytable.h"

namespace lv
{

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
    _14
};

inline struct Struct_SelectData
{
    std::unordered_map<std::string, long long> timer;

    std::list<SongListProperties> backtrace;
    EntryList entries;
    size_t selectedEntryIndex = 0;     // current selected entry index
    size_t highlightBarIndex = 0;  // highlighted bar index
    bool draggingListSlider = 0;    // is dragging slider

    size_t cursorClick = 0;  // click bar
    int cursorClickScroll = 0;  // -1: scroll up / 1: scroll down / 2: decide
    bool cursorEnterPending = false;

    SongListSortType sortType = SongListSortType::DEFAULT;
    FilterDifficultyType filterDifficulty = FilterDifficultyType::All;
    FilterKeysType filterKeys = FilterKeysType::All;
    bool optionChangePending = false;

    std::vector<DifficultyTableBMS> tables;

    double pitchSpeed = 1.0;

} SelectData;

}