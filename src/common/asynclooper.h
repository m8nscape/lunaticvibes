#pragma once

namespace lunaticvibes
{

#if WIN32
typedef HANDLE LooperHandler;
#else
typedef std::thread LooperHandler;
#endif // WIN32


// Poll update function in background thread.
// Should be OS-specific to provide reasonable performance.
class AsyncLooper
{
protected:
    StringContent _tag;
    unsigned _rate;
    bool _running = false;
    bool _inLoopBody = false;

    LooperHandler handler;

#ifdef _DEBUG
    int64_t _runThreadID = 0;
#endif

#if WIN32
protected:
    std::future<void> loopFuture;
    long long tStart = 0;
#else
    void _loopWithSleep();
#endif

public:
    AsyncLooper() = delete;
    AsyncLooper(StringContentView tag, std::function<void()>, unsigned rate_per_sec, bool single_inst = false);
    virtual ~AsyncLooper();
    void setRate(unsigned rate_per_sec);
    void loopStart();
    void loopEnd();
    bool isRunning() const { return _running; }
    unsigned getRate();

private:
    std::function<void()> _loopFunc;
    void run();
};

}
