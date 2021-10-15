#include "scene_context.h"
#include "game/data/data.h"

bool gResetSelectCursor = true;
bool gQuitOnFinish = false;
ChartContextParams gChartContext;
PlayContextParams gPlayContext;
SelectContextParams gSelectContext;
UpdateContextParams gUpdateContext;
std::shared_ptr<SongDB> g_pSongDB;
std::shared_ptr<ScoreDB> g_pScoreDB;


void clearContextPlayForRetry()
{
    if (gPlayContext.chartObj[0] != nullptr) { gPlayContext.chartObj[0]->reset(); gPlayContext.chartObj[0].reset(); }
    if (gPlayContext.chartObj[1] != nullptr) { gPlayContext.chartObj[1]->reset(); gPlayContext.chartObj[1].reset(); }
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        gPlayContext.graphGauge[i].clear();
        gPlayContext.graphScore[i].clear();
        gPlayContext.ruleset[i]->reset();
        gPlayContext.ruleset[i].reset();
    }
    gPlayContext.graphScoreTarget.clear();


}

void clearContextPlay()
{
    clearContextPlayForRetry();
    gPlayContext.chartObj[0] = nullptr;
    gPlayContext.chartObj[1] = nullptr;
    for (size_t i = 0; i < MAX_PLAYERS; ++i)
    {
        gPlayContext.gaugeType[i] = eGaugeOp::GROOVE;
        gPlayContext.mods[i].clear();
    }
    gPlayContext.remainTime = 0;
}

void updateContextSelectTitles()
{
    unsigned count = 0;
    /*
    for (unsigned idx = context_select.barIndex; count < 32; ++idx, ++count)
    {
        if (idx == context_select.info.size()) idx = 0;
        gTexts.set(eText(unsigned(eText::_SELECT_BAR_TITLE_FULL_0) + idx), context_select.info[idx].title);
    }
    */
}

std::shared_mutex mainThreadTaskQueueMutex;
std::queue<std::function<void()>> mainThreadTaskQueue;

void pushMainThreadTask(std::function<void()> f)
{
    std::unique_lock l(mainThreadTaskQueueMutex);
    mainThreadTaskQueue.push(f);
}

void doMainThreadTask()
{
    std::shared_lock l(mainThreadTaskQueueMutex);
    while (!mainThreadTaskQueue.empty())
    {
        mainThreadTaskQueue.front()();
        mainThreadTaskQueue.pop();
    }
}