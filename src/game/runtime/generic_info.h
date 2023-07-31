#pragma once
#include "common/asynclooper.h"
#include "game/data/data_system.h"

#ifdef _MSC_VER
inline tm gLocaltimeResult;
inline char gCtimeResult[26];
inline tm* localtime_(const std::time_t* t) { errno_t err = localtime_s(&gLocaltimeResult, t); return (err ? NULL : &gLocaltimeResult); };
inline const char* ctime_(const std::time_t* t) { errno_t err = ctime_s(gCtimeResult, 26, t); return (err ? "" : gCtimeResult); };
#else
inline tm* localtime_(const std::time_t* t) { return localtime(t); };
inline const char* ctime_(const std::time_t* t) { return ctime(t); };
#endif

namespace lunaticvibes
{

inline unsigned gFrameCount[10]{ 0 };
constexpr size_t FRAMECOUNT_IDX_FPS = 0;
constexpr size_t FRAMECOUNT_IDX_SCENE = 1;
constexpr size_t FRAMECOUNT_IDX_INPUT = 2;

// Should only have one instance at once.
class GenericInfoUpdater : public AsyncLooper
{
public:
	GenericInfoUpdater(unsigned rate = 1) : AsyncLooper("GenericInfoUpdater", std::bind(&GenericInfoUpdater::_loop, this), rate) {}
private:
	void _loop()
	{
		SystemData.currentRenderFPS = gFrameCount[0] / _rate;
		SystemData.currentUpdateFPS = gFrameCount[1] / _rate;
		SystemData.currentInputFPS = gFrameCount[2] / _rate;
		gFrameCount[0] = 0;
		gFrameCount[1] = 0;
		gFrameCount[2] = 0;

		std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto d = localtime_(&t);
		if (d)
		{
			SystemData.dateYear = d->tm_year + 1900;
			SystemData.dateMonthOfYear = d->tm_mon + 1;
			SystemData.dateDayOfMonth = d->tm_mday;
			SystemData.timeHour = d->tm_hour;
			SystemData.timeMin = d->tm_min;
			SystemData.timeSec = d->tm_sec;
		}

		//createNotification(std::to_string(t));
	}
};
//InputWrapper::InputWrapper(unsigned rate) : AsyncLooper(std::bind(&InputWrapper::_loop, this), rate)

}

#ifdef _MSC_VER
#undef localtime
#undef ctime
#endif
