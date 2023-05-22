#pragma once


namespace lunaticvibes
{

inline struct Struct_ResultData
{
    std::unordered_map<std::string, long long> timers;

    bool cleared = false;
    bool flipResult = false;

    bool playerCleared[2] = { false, false };

} ResultData;

}