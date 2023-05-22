#include "common/pch.h"
#include "data_play.h"
#include "game/chart/chart_types.h"
#include "game/ruleset/ruleset.h"

namespace lunaticvibes
{

void Struct_PlayData::clearContextPlayForRetry()
{
    playStarted = false;

    if (player[0].chartObj != nullptr) { player[0].chartObj->reset(); player[0].chartObj.reset(); }
    if (player[1].chartObj != nullptr) { player[1].chartObj->reset(); player[1].chartObj.reset(); }
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        player[i].graphGauge.clear();
        player[i].graphRate.clear();
        if (player[i].ruleset)
            player[i].ruleset.reset();
    }

    replayNew.reset();

}

void Struct_PlayData::clearContextPlay()
{
    clearContextPlayForRetry();
    player[0].chartObj = nullptr;
    player[1].chartObj = nullptr;
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        player[i].mods.clear();
    }

    static std::random_device rd;
    randomSeed = ((uint64_t)rd() << 32) | rd();

    courseStage = 0;
    player[PLAYER_SLOT_PLAYER].courseRunningCombo = 0;
    player[PLAYER_SLOT_PLAYER].courseMaxCombo = 0;
    player[PLAYER_SLOT_TARGET].courseRunningCombo = 0;
    player[PLAYER_SLOT_TARGET].courseMaxCombo = 0;

    // replay.reset();     // load at setEntryInfo() @ scene_context.cpp
    replayMybest.reset();  // load at decide() @ scene_select.cpp
}

void Struct_PlayData::pushGraphPoints()
{
    player[PLAYER_SLOT_PLAYER].graphGauge.push_back(player[PLAYER_SLOT_PLAYER].ruleset->getData().health * 100);

    player[PLAYER_SLOT_PLAYER].graphRate.push_back(player[PLAYER_SLOT_PLAYER].ruleset->getData().total_acc);

    if (player[PLAYER_SLOT_TARGET].ruleset)
    {
        player[PLAYER_SLOT_TARGET].graphGauge.push_back(player[PLAYER_SLOT_TARGET].ruleset->getData().health * 100);
        player[PLAYER_SLOT_TARGET].graphRate.push_back(player[PLAYER_SLOT_TARGET].ruleset->getData().total_acc);
    }

    if (!isAuto && !isReplay && replayMybest)
    {
        player[PLAYER_SLOT_MYBEST].graphRate.push_back(player[PLAYER_SLOT_MYBEST].ruleset->getData().total_acc);
    }
}


}