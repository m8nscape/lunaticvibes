#ifdef __linux__

#include "sysutil.h"

#include <cstdio>
#include <thread>

static std::thread::id s_main_thread {};

void SetThreadAsMainThread()
{
    s_main_thread = std::this_thread::get_id();
}

int64_t GetCurrentThreadID()
{
    // TODO.
    return 0;
}

bool IsMainThread()
{
    return s_main_thread == std::this_thread::get_id();
}

void SetThreadName(const char* name) {}

void panic(const char* title, const char* msg)
{
    fprintf(stderr, "PANIC! [%s] %s\n", title, msg); 
    abort(); 
}

#include <filesystem>
void GetExecutablePath(char* output, size_t bufsize, size_t& len)
{
    char fullpath[256];
    memset(fullpath, 0, sizeof(fullpath));

    char szTmp[32];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = std::min(readlink(szTmp, fullpath, sizeof(fullpath)), static_cast<ssize_t>(sizeof(fullpath) - 1));
    if (bytes >= 0)
        fullpath[bytes] = '\0';

    using namespace std::filesystem;
    auto parent = path(fullpath).parent_path();
    strcpy(output, bufsize, (const char*)parent.u8string().c_str());
    len = strlen(output);
}

void addWMEventHandler(void* f)
{

}

void callWMEventHandler(void* arg1, void* arg2, void* arg3, void* arg4)
{

}

long long getFileLastWriteTime(const Path& p)
{
    return std::chrono::duration_cast<std::chrono::seconds>(fs::last_write_time(p).time_since_epoch()).count();
}

#endif // __linux__
