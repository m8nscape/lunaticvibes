#include "scene_context.h"

void clearContextPlay()
{
    context_play.scrollObj[0] = nullptr;
    context_play.scrollObj[1] = nullptr;
    context_play.judgeLevel = 0;
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        context_play.graphGauge[i].clear();
        context_play.graphScore[i].clear();
        context_play.gaugeType[i] = eGaugeOp::GROOVE;
        context_play.ruleset[i] = nullptr;
        context_play.mods[i].clear();
    }
    context_play.remainTime = 0;
}