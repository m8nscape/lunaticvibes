#pragma once
#include <chrono>
#include "state.h"
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
		State::set(IndexNumber::FPS, gFrameCount[0] / _rate);
		gFrameCount[0] = 0;

		for (unsigned i = 1; i < 10; ++i)
		{
			State::set((IndexNumber)((int)IndexNumber::_PPS1 + i - 1), gFrameCount[i] / _rate);
			gFrameCount[i] = 0;
		}
		State::set(IndexNumber::SCENE_UPDATE_FPS, State::get(IndexNumber::_PPS1));
		State::set(IndexNumber::INPUT_DETECT_FPS, State::get(IndexNumber::_PPS2));

		std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto d = localtime_(&t);
        if (d)
        {
            State::set(IndexNumber::DATE_YEAR, d->tm_year + 1900);
            State::set(IndexNumber::DATE_MON, d->tm_mon + 1);
            State::set(IndexNumber::DATE_DAY, d->tm_mday);
            State::set(IndexNumber::DATE_HOUR, d->tm_hour);
            State::set(IndexNumber::DATE_MIN, d->tm_min);
            State::set(IndexNumber::DATE_SEC, d->tm_sec);

            State::set(IndexText::_TEST1, ctime_(&t));
        }

		//createNotification(std::to_string(t));
	}
};
//InputWrapper::InputWrapper(unsigned rate) : AsyncLooper(std::bind(&InputWrapper::_loop, this), rate)

#ifdef _MSC_VER
#undef localtime
#undef ctime
#endif
