#pragma once
#include <functional>
#include <shared_mutex>
#include <map>
#include "types.h"

#if WIN32
#include <Windows.h>
typedef HANDLE LooperHandler;
VOID CALLBACK WaitOrTimerCallback(_In_ PVOID lpParameter, _In_ BOOLEAN TimerOrWaitFired);

#elif LINUX
typedef void* LooperHandler;

#else // FALLBACK
#include <thread>
typedef std::thread LooperHandler;
#endif // WIN32


// Poll update function in background thread.
// Should be OS-specific to provide reasonable performance.
class AsyncLooper
{
private:
    // for statistics
    bool _statReady = false;
    long long _prevLoopTime = 0;
    const size_t LOOP_TIME_BUFFER_SIZE = 10;
    std::list<long long> _loopTimeBuffer;
    decltype(_loopTimeBuffer.begin()) _bufferIt;
    void _recordLoopTime();

protected:
    StringContent _tag;
    unsigned _rate;
    unsigned _rateTime;
    bool _running = false;
    bool _inLoopBody = false;
    unsigned _iterateCount = 0;
    unsigned _iterateEndCount = 0;
    LooperHandler handler;

public:
    AsyncLooper() = default;
    AsyncLooper(StringContentView tag, std::function<void()>, unsigned rate_per_sec, bool single_inst = false);
    virtual ~AsyncLooper();
    void loopStart();
    void loopEnd();
    bool isRunning() { return _running; }
    unsigned getRate();
    unsigned getRateRealtime();

private:
    std::function<void()> _loopFunc;
    std::function<void()> _loopFuncBody;
    void run();
#if WIN32
    friend VOID CALLBACK WaitOrTimerCallback(_In_ PVOID lpParameter, _In_ BOOLEAN TimerOrWaitFired);
#elif LINUX
#else // FALLBACK
    void _loopWithSleep();
#endif

protected:
    static std::map<uintptr_t, AsyncLooper> loopers;
public:
    static AsyncLooper* addLooper(uintptr_t key, StringContentView tag, std::function<void()>, unsigned rate_per_sec, bool single_inst = false);
    static void removeLooper(uintptr_t key);
};
