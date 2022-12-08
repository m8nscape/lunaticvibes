// Song list entry definition

#pragma once
#include "common/types.h"

enum eEntryType
{
    UNKNOWN,
    NEW_SONG_FOLDER,
    FOLDER,
    SONG,   // all charts in folder
    CHART,  // one chart in folder
    CUSTOM_FOLDER,
    COURSE_FOLDER,
    RIVAL,
    RIVAL_SONG,
    RIVAL_CHART,
    NEW_COURSE,
    COURSE,
    RANDOM_COURSE,

    ARENA_FOLDER,
    ARENA_COMMAND,
    ARENA_LOBBY,

    CHART_LINK,
    REPLAY,
};

class EntryBase
{
protected:
    eEntryType _type = eEntryType::UNKNOWN;

public:
    EntryBase() = default;
    virtual ~EntryBase() = default;

public:
    HashMD5 md5;
    StringContent _name;
    StringContent _name2;
    unsigned long long _addTime = 0;  // from epoch time

public:
    virtual Path getPath() { return Path(); }
    constexpr eEntryType type() const { return _type; }
};
