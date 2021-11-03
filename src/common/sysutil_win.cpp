#if _WIN32 || _WIN64
#include "sysutil.h"
#include <cstdio>
#include <windows.h>  

#ifdef _MSC_VER
#ifndef strcpy
#define strcpy strcpy_s
#endif
#endif

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack(push,8)  
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.  
    LPCSTR szName; // Pointer to name (in user addr space).  
    DWORD dwThreadID; // Thread ID (-1=caller thread).  
    DWORD dwFlags; // Reserved for future use, must be zero.  
} THREADNAME_INFO;
#pragma pack(pop)  

void SetThreadNameWin32(DWORD dwThreadID, const char* threadName) {
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = threadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)& info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
#pragma warning(pop)  
}

[[noreturn]] inline void panicWin32(const char* title, const char* msg)
{
    MessageBox(NULL, title, msg, MB_OK);
    abort();
}

static DWORD dwMainThreadId = 0;

void SetThreadAsMainThread()
{
    dwMainThreadId = GetCurrentThreadId();
}

bool IsMainThread()
{
    return GetCurrentThreadId() == dwMainThreadId;
}

void SetThreadName(const char* name) 
{
    SetThreadNameWin32(GetCurrentThreadId(), name); 
}
void panic(const char* title, const char* msg) 
{
    panicWin32(title, msg);
}

#include <filesystem>
void GetExecutablePath(char* output, size_t bufsize, size_t& len)
{
    char fullpath[256];
    memset(fullpath, 0, sizeof(fullpath));

    if (!GetModuleFileNameA(NULL, fullpath, 256))
    {
        output[0] = 0;
        len = 0;
        return;
    }

    using namespace std::filesystem;
    auto parent = path(fullpath).parent_path();
    strcpy(output, bufsize, (const char*)parent.string().c_str());
    len = strlen(output);
}

static HWND hwnd = (HWND)INVALID_HANDLE_VALUE;
void setWindowHandle(void* handle)
{
    hwnd = *(HWND*)handle;
}

bool getMouseCursorPos(int& x, int& y)
{
    RECT rect{ 0 };
    if (GetWindowRect(hwnd, &rect))
    {
        POINT point{ 0 };
        if (GetCursorPos(&point) && ScreenToClient(hwnd, &point))
        {
            x = point.x;
            y = point.y;
            return true;
        }
    }
    return false;
}

static bool foreground = true;
bool IsWindowForeground()
{
    return foreground;
}

void SetWindowForeground(bool f)
{
    foreground = f;
}

#endif