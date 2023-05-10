#pragma once

namespace lv::data
{

inline struct Struct_ArenaData
{
    std::unordered_map<std::string, long long> timers;

    HashMD5 remoteRequestedChart;       // only valid when remote is requesting a new chart; reset after list change
    std::string remoteRequestedPlayer;  // only valid when remote is requesting a new chart; reset after list change

    bool isArenaReady = false;
    bool isInArenaRequest = false;
    bool isArenaCancellingRequest = false;

} ArenaData;

}