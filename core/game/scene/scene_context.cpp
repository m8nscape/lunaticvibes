#include "scene_context.h"
#include "game/data/text.h"

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

void updateContextTitles()
{
    unsigned count = 0;
    for (unsigned idx = context_select.barIndex; count < 32; ++idx, ++count)
    {
        if (idx == context_select.info.size()) idx = 0;
        gTexts.set(eText(unsigned(eText::_SELECT_BAR_TITLE_FULL_0) + idx), context_select.info[idx].title);
    }
}