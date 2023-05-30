#pragma once
#include "common/types.h"

namespace lunaticvibes
{

class TimerStorage
{
public:
    using KeyT = size_t;
    using ValueT = long long;
private:
    std::shared_mutex sm;
    std::unordered_map<KeyT, ValueT> timers;
    std::unordered_map<uintptr_t, KeyT> keysMap;
public:
    ValueT& operator[](const std::string&);
    ValueT& operator[](std::string_view);
    ValueT& operator[](const char*);
    ValueT& operator[](KeyT);
    void clear();
};

}