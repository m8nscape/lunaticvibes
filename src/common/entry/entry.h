// Song list entry definition

#pragma once
#include "types.h"

enum eEntryType
{
    UNKNOWN,
    FOLDER,
    CUSTOM_FOLDER,
    SONG,
    RIVAL,
    RIVAL_SONG,
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
    long long _addTime;  // from epoch time

public:
    constexpr eEntryType type() const { return _type; }
};
