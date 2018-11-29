#pragma once
#include <filesystem>
#include <vector>
#include <utility>

namespace fs = std::filesystem;

// Following LR2skin path rules:
// Only filename (not including folders) could have wildcards "*"
// Searching is not recursive.
std::vector<fs::path> findFiles(fs::path path);

// For LR2 skin .csv parsing:
// op1~4 may include a '!' before the number, split it out
std::pair<unsigned, bool> stoub(const std::string&);
int stoine(const std::string& str) noexcept;

constexpr int base36(char c)
{
	return (c > '9') ? (10 + c - 'A') : (c - '0');
}

constexpr int base36(char first, char second)
{
	return 36 * base36(first) + base36(second);
}

constexpr int base36(const char* c)
{
	return base36(c[0], c[1]);
}

constexpr int base16(char c)
{
	return (c > '9') ? (10 + c - 'A') : (c - '0');
}

constexpr int base16(char first, char second)
{
	return 16 * base16(first) + base16(second);
}

constexpr int base16(const char* c)
{
	return base16(c[0], c[1]);
}

#if _MSC_VER && (_WIN32 || _WIN64)
#include <windows.h>  
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
inline void __SetThreadName(DWORD dwThreadID, const char* threadName) {  
    THREADNAME_INFO info;  
    info.dwType = 0x1000;  
    info.szName = threadName;  
    info.dwThreadID = dwThreadID;  
    info.dwFlags = 0;  
#pragma warning(push)  
#pragma warning(disable: 6320 6322)  
    __try{  
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);  
    }  
    __except (EXCEPTION_EXECUTE_HANDLER){  
    }  
#pragma warning(pop)  
}  

#define SetThreadName(name) __SetThreadName(GetCurrentThreadId(), name)
#else
#define SetThreadName(name) 0
#endif
