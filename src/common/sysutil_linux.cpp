#ifdef __linux__

#include "pch.h"
#include "sysutil.h"

#include <cstdio>
#include <cstring>
#include <filesystem>
#include <string>
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

std::string GetExecutablePath()
{
    char fullpath[256] = { 0 };

    char process_path[] = "/proc/self/exe";
    const auto bytes = std::min(readlink(process_path, fullpath, sizeof(fullpath)), static_cast<ssize_t>(sizeof(fullpath) - 1));
    if (bytes >= 0)
        fullpath[bytes] = '\0';

    return fs::path(fullpath).parent_path().u8string();
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

namespace portable_strerror_r_detail {

// Use the output of XSI-compliant (from POSIX.1-2001) strerror_r().
static char* impl(int strerror_r_ret, char* buffer, const size_t buffer_length)
{
    if (strerror_r_ret != 0) {
        // The message format follows what GNU's strerror_r() would
        // return in such case.
        snprintf(buffer, buffer_length, "Unknown error %d", strerror_r_ret);
    }
    return buffer;
}

// Use the output of GNU-specific strerror_r().
static char* impl(char* strerror_r_ret, char* /*buffer*/, const size_t /*buffer_length*/)
{
    return strerror_r_ret;
}

} // namespace portable_strerror_r_detail

// Interface to support both GNU's and XSI's (from POSIX.1-2001) strerror_r().
static const char* portable_strerror_r(const int errnum, char* buffer, const size_t buffer_length)
{
    return portable_strerror_r_detail::impl(strerror_r(errnum, buffer, buffer_length), buffer, buffer_length);
}

const char* safe_strerror(const int errnum, char* buffer, const size_t buffer_length)
{
    return portable_strerror_r(errnum, buffer, buffer_length);
}

#endif // __linux__
