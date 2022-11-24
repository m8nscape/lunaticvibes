#include "asynclooper.h"
#include <numeric>
#include <chrono>
#include "log.h"

AsyncLooper::AsyncLooper(StringContentView tag, std::function<void()> func, unsigned rate_per_sec, bool single_inst) : 
    _tag(tag), _loopFunc(func)
{
    _rate = rate_per_sec;

#if WIN32
    handler = CreateWaitableTimerExA(
        NULL,
        _tag.empty() ? NULL : _tag.c_str(),
        0,
        TIMER_ALL_ACCESS
    );
    assert(handler != NULL);
#endif
}

AsyncLooper::~AsyncLooper()
{
    assert(!_running);

#if WIN32
    if (handler)
    {
        CloseHandle(handler);
        handler = NULL;
    }
#endif
}

void AsyncLooper::setRate(unsigned rate_per_sec)
{
    _rate = rate_per_sec;
    if (_running)
    {
        loopEnd();
        loopStart();
    }
}

void AsyncLooper::run()
{
#if _DEBUG
    _runThreadID = GetCurrentThreadID();
#endif
    if (_running && !_inLoopBody)
    {
        _inLoopBody = true;
        _loopFunc();
        _inLoopBody = false;
    }
}

unsigned AsyncLooper::getRate()
{
    return _rate;
}

#if WIN32

void AsyncLooper::loopStart()
{
    if (!_running)
    {
        if (handler)
        {
            _running = true;

            loopFuture = std::async(std::launch::async, [this]()
                {
                    long long us = _rate > 0 ? 1000000 / _rate : 0;
                    LARGE_INTEGER dueTime = { 0 };
                    dueTime.QuadPart = -(long long(us) * 10);

                    using namespace std::chrono;
                    using namespace std::chrono_literals;

                    tStart = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

                    while (_running)
                    {
                        if (us > 0 && dueTime.QuadPart < 0)
                        {
                            SetWaitableTimerEx(
                                handler,
                                &dueTime,
                                0,
                                NULL,
                                NULL,
                                NULL,
                                0
                            );
                            //SleepEx(100, TRUE);
                            WaitForSingleObjectEx(handler, 1000, TRUE);
                        }

                        run();

                        auto t2 = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch()).count();

                        if (us > (t2 - tStart))
                        {
                            dueTime.QuadPart = -(long long(us - (t2 - tStart)) * 10);
                            tStart += us;
                        }
                        else
                        {
                            dueTime.QuadPart = 0;
                            tStart += t2 - tStart;
                        }
                    }
                });
        }

        if (_running)
        {
            LOG_DEBUG << "[Looper] Started with rate " << _rate << "/s " << _tag;
        }
    }
}

void AsyncLooper::loopEnd()
{
    if (_running)
    {
        _running = false;
        loopFuture.wait_for(std::chrono::seconds(1));
        if (CancelWaitableTimer(handler) == 0)
        {
            DWORD dwError = GetLastError();
            if (dwError != ERROR_INVALID_HANDLE)
            {
                LOG_ERROR << "[Looper] Delete timer error: " << dwError;
                while (dwError == ERROR_IO_PENDING && DeleteTimerQueueTimer(NULL, handler, INVALID_HANDLE_VALUE) != 0)
                {
                    dwError = GetLastError();
                }
                assert(dwError == 0);
            }
        }
        LOG_DEBUG << "[Looper] Ended of rate " << _rate << "/s " << _tag;
    }
}

#else // FALLBACK
void AsyncLooper::_loopWithSleep()
{
    while (_running)
    {
        run();
        std::this_thread::sleep_for(std::chrono::milliseconds(_rateTime));
    }
}

void AsyncLooper::loopStart()
{
    handler = std::thread(&AsyncLooper::_loopWithSleep, this);
    _running = true;
}

void AsyncLooper::loopEnd()
{
    if (!_running) return;
    _running = false;
    handler.join();
}
#endif
