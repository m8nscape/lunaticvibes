#pragma once


namespace lv::data
{

inline struct Struct_ResultData
{
    std::unordered_map<std::string, long long> timers;

    bool cleared = false;
    bool flipResult = false;

} ResultData;

}