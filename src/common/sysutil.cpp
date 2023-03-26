#include "sysutil.h"
#include <queue>
#include <shared_mutex>
#include <future>

std::shared_mutex mainThreadTaskQueueMutex;
std::queue<std::function<void()>> mainThreadTaskQueue;
bool handleMainThreadTask = true;

static bool s_foreground = true;
bool IsWindowForeground()
{
    return s_foreground;
}

void SetWindowForeground(bool f)
{
    s_foreground = f;
}

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

Path getSysFontPath(std::string* faceName, int* faceIndex, Languages lang)
{
    Path p = Path(GAMEDATA_PATH) / "resources" / "fonts" / "NotoSansCJK-Regular.ttc";
    switch (lang)
    {
    case Languages::KR:
        if (faceName) *faceName = "Noto Sans CJK KR";
        if (faceIndex) *faceIndex = 1;
        break;
    case Languages::ZHCN:
        if (faceName) *faceName = "Noto Sans CJK SC";
        if (faceIndex) *faceIndex = 2;
        break;
    case Languages::ZHTW:
        if (faceName) *faceName = "Noto Sans CJK TC";
        if (faceIndex) *faceIndex = 3;
        break;
    case Languages::ZHHK:
        if (faceName) *faceName = "Noto Sans CJK HK";
        if (faceIndex) *faceIndex = 4;
        break;
    case Languages::EN:
    case Languages::JP:
    default:
        if (faceName) *faceName = "Noto Sans CJK JP";
        if (faceIndex) *faceIndex = 0;
        break;
    }
    return p;
}

Path getSysMonoFontPath(std::string* faceName, int* faceIndex, Languages lang)
{
    Path p = Path(GAMEDATA_PATH) / "resources" / "fonts" / "NotoSansCJK-Regular.ttc";
    switch (lang)
    {
    case Languages::KR:
        if (faceName) *faceName = "Noto Sans Mono CJK KR";
        if (faceIndex) *faceIndex = 6;
        break;
    case Languages::ZHCN:
        if (faceName) *faceName = "Noto Sans Mono CJK SC";
        if (faceIndex) *faceIndex = 7;
        break;
    case Languages::ZHTW:
        if (faceName) *faceName = "Noto Sans Mono CJK TC";
        if (faceIndex) *faceIndex = 8;
        break;
    case Languages::ZHHK:
        if (faceName) *faceName = "Noto Sans Mono CJK HK";
        if (faceIndex) *faceIndex = 9;
        break;
    case Languages::EN:
    case Languages::JP:
    default:
        if (faceName) *faceName = "Noto Sans Mono CJK JP";
        if (faceIndex) *faceIndex = 5;
        break;
    }
    return p;
}
