#include "scene_context.h"

void clearContextPlayForRetry()
{
    if (context_play.scrollObj[0] != nullptr) { context_play.scrollObj[0].reset(); }
    if (context_play.scrollObj[1] != nullptr) { context_play.scrollObj[1].reset(); }
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        context_play.graphGauge[i].clear();
        context_play.graphScore[i].clear();
        context_play.ruleset[i] = nullptr;
    }
}

void clearContextPlay()
{
    clearContextPlayForRetry();
    context_play.scrollObj[0] = nullptr;
    context_play.scrollObj[1] = nullptr;
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        context_play.gaugeType[i] = eGaugeOp::GROOVE;
        context_play.mods[i].clear();
    }
    context_play.judgeLevel = 0;
    context_play.remainTime = 0;
}