#include "sysutil.h"
#include <queue>
#include <shared_mutex>
#include <future>

std::shared_mutex mainThreadTaskQueueMutex;
std::queue<std::function<void()>> mainThreadTaskQueue;
bool handleMainThreadTask = true;

void pushMainThreadTask(std::function<void()> f)
{
    if (IsMainThread())
    {
        //LOG_DEBUG << "Warning: Calling pushMainThreadTask at main thread";
        f();
    }
    else
    {
        if (handleMainThreadTask)
        {
            std::unique_lock l(mainThreadTaskQueueMutex);
            mainThreadTaskQueue.push(f);
        }
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

void StopHandleMainThreadTask()
{
    handleMainThreadTask = false;
}

bool CanHandleMainThreadTask()
{
    return handleMainThreadTask;
}

long long getFileTimeNow()
{
    return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

Path getSysFontPath(std::string* faceName, int* faceIndex)
{
    Path p = Path(GAMEDATA_PATH) / "resources" / "fonts" / "NotoSansCJK-Regular.ttc";
    if (faceName) *faceName = "Noto Sans CJK JP";
    if (faceIndex) *faceIndex = 0;
    return p;
}

Path getSysMonoFontPath(std::string* faceName, int* faceIndex)
{
    Path p = Path(GAMEDATA_PATH) / "resources" / "fonts" / "NotoSansCJK-Regular.ttc";
    if (faceName) *faceName = "Noto Sans Mono CJK JP";
    if (faceIndex) *faceIndex = 6;
    return p;
}
