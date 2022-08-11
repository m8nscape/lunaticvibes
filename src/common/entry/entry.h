// Song list entry definition

#pragma once
#include "common/types.h"

enum eEntryType
{
    UNKNOWN,
    FOLDER,
    CUSTOM_FOLDER,
    SONG,   // all charts in folder
    CHART,  // one chart in folder
    RIVAL,
    RIVAL_SONG,
    RIVAL_CHART,
    NEW_COURSE,
    COURSE,
    RANDOM_COURSE,
};

class vEntry
{
protected:
    eEntryType _type = eEntryType::UNKNOWN;

public:
    vEntry() = default;
    virtual ~vEntry() = default;

public:
    HashMD5 md5;
    StringContent _name;
    StringContent _name2;
    unsigned long long _addTime = 0;  // from epoch time

public:
    constexpr eEntryType type() const { return _type; }
};
