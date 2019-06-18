#pragma once
#include "fraction.h"
#include <string>
#include <chrono>
#include <variant>
#include <iostream>

typedef unsigned Measure;
typedef double BPM;
typedef fraction Beat;      // rhythm indicator, can be above 1
typedef fraction BeatNorm;  // Normal rhythm indicator, must be normalized (value: [0, 1)).

typedef std::chrono::milliseconds timeNormRes; // Regular time, expect millisecond (1/1e3 second). Used for general timing demand
typedef std::chrono::nanoseconds  timeHighRes; // High resolution time, expect nanosecond (1/1e9 second). Used for note timings

#pragma warning(push)
#pragma warning(disable:4244)
class timestamp
{
private:
	decltype(std::declval<timeNormRes>().count()) _rTime;
	decltype(std::declval<timeHighRes>().count()) _hTime;
public:
	timestamp()
	{
		auto now = std::chrono::system_clock::now().time_since_epoch();
		_rTime = std::chrono::duration_cast<timeNormRes>(now).count();
		_hTime = std::chrono::duration_cast<timeHighRes>(now).count();
	}
	timestamp(long long n, bool high_resolution = false)
	{
		if (high_resolution || n > LLONG_MAX / 100000)
		{
			_hTime = n;
			_rTime = std::chrono::duration_cast<timeNormRes>(timeHighRes(n)).count();
		}
		else
		{
			_rTime = n;
			_hTime = std::chrono::duration_cast<timeHighRes>(timeNormRes(n)).count();
		}
	}
	timestamp(const timestamp& t) :_rTime(t._rTime), _hTime(t._hTime) {}
	static timestamp fromBPM(BPM bpm)
	{
		using namespace std::chrono;
		return timestamp(6e4 * 4 / bpm * duration_cast<timeHighRes>(1ms).count(), true);
	}
	~timestamp() {}

	timestamp operator-  ()                     const { return timestamp(-_hTime, true); }
	timestamp operator+  (const timestamp& rhs) const { timestamp tmp(*this); tmp._rTime += rhs._rTime; tmp._hTime += rhs._hTime; return tmp; }
	timestamp operator-  (const timestamp& rhs) const { timestamp tmp(*this); tmp._rTime -= rhs._rTime; tmp._hTime -= rhs._hTime; return tmp; }
	timestamp operator*  (const double rhs) const { timestamp tmp(*this); tmp._rTime *= rhs;  tmp._hTime *= rhs; return tmp; }
	timestamp& operator+= (const timestamp& rhs) { _rTime += rhs._rTime; _hTime += rhs._hTime; return *this; }
	timestamp& operator-= (const timestamp& rhs) { _rTime -= rhs._rTime; _hTime -= rhs._hTime; return *this; }
	bool   operator<  (const timestamp& rhs) const { return _hTime < rhs._hTime; }
	bool   operator>  (const timestamp& rhs) const { return _hTime > rhs._hTime; }
	bool   operator<= (const timestamp& rhs) const { return _hTime <= rhs._hTime; }
	bool   operator>= (const timestamp& rhs) const { return _hTime >= rhs._hTime; }
	bool   operator== (const timestamp& rhs) const { return _hTime == rhs._hTime; }
	bool   operator!= (const timestamp& rhs) const { return _hTime != rhs._hTime; }
	friend inline std::ostream& operator<< (std::ostream& os, const timestamp& t) { return os << t._rTime << "ms / " << t._hTime << "ns"; }

	constexpr decltype(_rTime) norm() const { return _rTime; }
	constexpr decltype(_hTime) hres() const { return _hTime; }
};
#pragma warning(pop)


struct Note
{
    Measure measure;        // Which measure the note is placed
    Beat rawBeat;           // Which beat the note is placed, can be above 1
    timestamp time;             // Time point
    std::variant<long long, double> value;              // varies from note type to type, e.g. #BGM, #BPM, etc
};
