#include "sysutil.h"
#include <queue>
#include <shared_mutex>
#include <future>

std::shared_mutex mainThreadTaskQueueMutex;
std::queue<std::function<void()>> mainThreadTaskQueue;

void pushMainThreadTask(std::function<void()> f)
{
    if (IsMainThread())
    {
        //LOG_DEBUG << "Warning: Calling pushMainThreadTask at main thread";
        f();
    }
    else
    {
        std::unique_lock l(mainThreadTaskQueueMutex);
        mainThreadTaskQueue.push(f);
    }
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
