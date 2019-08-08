#include "scene_context.h"

void clearContextPlay()
{
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        context_play.graphGauge[i].clear();
        context_play.graphScore[i].clear();
        context_play.gaugeType[i] = 0;
        context_play.ruleset[i] = nullptr;
        context_play.mods[i].clear();
    }
}