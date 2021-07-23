#pragma once
#include <chrono>
#include "game/data/number.h"
#include "game/data/text.h"
#include "common/asynclooper.h"

#ifdef _MSC_VER
inline tm gLocaltimeResult;
inline char gCtimeResult[26];
inline tm* localtime_(const std::time_t* t) { errno_t err = localtime_s(&gLocaltimeResult, t); return (err ? NULL : &gLocaltimeResult); };
inline const char* ctime_(const std::time_t* t) { errno_t err = ctime_s(gCtimeResult, 26, t); return (err ? "" : gCtimeResult); };
#else
inline tm* localtime_(const std::time_t* t) { return localtime(t); };
inline const char* ctime_(const std::time_t* t) { return ctime(t); };
#endif

inline unsigned gFrameCount[10]{ 0 };

// Should only have one instance at once.
class GenericInfoUpdater : public AsyncLooper
{
public:
	GenericInfoUpdater(unsigned rate = 1) : AsyncLooper(std::bind(&GenericInfoUpdater::_loop, this), rate) {}
private:
	void _loop()
	{
		gNumbers.set(eNumber::FPS, gFrameCount[0] / _rate);
		gFrameCount[0] = 0;

		for (unsigned i = 1; i < 10; ++i)
		{
			gNumbers.set((eNumber)((int)eNumber::_PPS1 + i - 1), gFrameCount[i] / _rate);
			gFrameCount[i] = 0;
		}

		std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto d = localtime_(&t);
        if (d)
        {
            gNumbers.set(eNumber::DATE_YEAR, d->tm_year + 1900);
            gNumbers.set(eNumber::DATE_MON, d->tm_mon + 1);
            gNumbers.set(eNumber::DATE_DAY, d->tm_mday);
            gNumbers.set(eNumber::DATE_HOUR, d->tm_hour);
            gNumbers.set(eNumber::DATE_MIN, d->tm_min);
            gNumbers.set(eNumber::DATE_SEC, d->tm_sec);

            gTexts.set(eText::_TEST1, ctime_(&t));
        }
	}
};
//InputWrapper::InputWrapper(unsigned rate) : AsyncLooper(std::bind(&InputWrapper::_loop, this), rate)

#ifdef _MSC_VER
#undef localtime
#undef ctime
#endif
