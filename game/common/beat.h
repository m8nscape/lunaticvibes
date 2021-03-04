#pragma once
#include "types.h"
#include "fraction.h"
#include <string>
#include <chrono>
#include <variant>
#include <iostream>

typedef unsigned Measure;
typedef double BPM;

typedef fraction Beat;      // rhythm indicator, can be above 1
typedef fraction Segment;  // Normal rhythm indicator, must be normalized (value: [0, 1)).

typedef std::chrono::milliseconds timeNormRes; // Regular time, expect millisecond (1/1e3 second). Used for general timing demand
typedef std::chrono::nanoseconds  timeHighRes; // High resolution time, expect nanosecond (1/1e9 second). Used for note timings

#pragma warning(push)
#pragma warning(disable:4244)
class Time
{
private:
	decltype(std::declval<timeNormRes>().count()) _regular;
	decltype(std::declval<timeHighRes>().count()) _highres;
public:
	Time()
	{
		auto now = std::chrono::system_clock::now().time_since_epoch();
		_regular = std::chrono::duration_cast<timeNormRes>(now).count();
		_highres = std::chrono::duration_cast<timeHighRes>(now).count();
	}
	Time(long long n, bool init_with_high_resolution_timestamp = false)
	{
		if (init_with_high_resolution_timestamp || n > LLONG_MAX / 100000)
		{
			_highres = n;
			_regular = std::chrono::duration_cast<timeNormRes>(timeHighRes(n)).count();
		}
		else
		{
			_regular = n;
			_highres = std::chrono::duration_cast<timeHighRes>(timeNormRes(n)).count();
		}
	}
	Time(const Time& t) :_regular(t._regular), _highres(t._highres) {}
	~Time() {}

	static Time singleBeatLengthFromBPM(BPM bpm)
	{
		using namespace std::chrono;
		return Time(6e4 * 4 / bpm * duration_cast<timeHighRes>(1ms).count(), true);
	}

	Time operator-  () const { return Time(-_highres, true); }
	Time operator+  (const Time& rhs) const { Time tmp(*this); tmp._regular += rhs._regular; tmp._highres += rhs._highres; return tmp; }
	Time operator-  (const Time& rhs) const { Time tmp(*this); tmp._regular -= rhs._regular; tmp._highres -= rhs._highres; return tmp; }
	Time operator*  (const double rhs) const { Time tmp(*this); tmp._regular *= rhs;  tmp._highres *= rhs; return tmp; }
	Time& operator+= (const Time& rhs) { _regular += rhs._regular; _highres += rhs._highres; return *this; }
	Time& operator-= (const Time& rhs) { _regular -= rhs._regular; _highres -= rhs._highres; return *this; }
	bool   operator<  (const Time& rhs) const { return _highres < rhs._highres; }
	bool   operator>  (const Time& rhs) const { return _highres > rhs._highres; }
	bool   operator<= (const Time& rhs) const { return _highres <= rhs._highres; }
	bool   operator>= (const Time& rhs) const { return _highres >= rhs._highres; }
	bool   operator== (const Time& rhs) const { return _highres == rhs._highres; }
	bool   operator!= (const Time& rhs) const { return _highres != rhs._highres; }
	friend inline std::ostream& operator<< (std::ostream& os, const Time& t) { return os << t._regular << "ms / " << t._highres << "ns"; }

	constexpr decltype(_regular) norm() const { return _regular; }  // ms
	constexpr decltype(_highres) hres() const { return _highres; }  // ns
};
#pragma warning(pop)

const size_t NOTE_INDEX_LN_TAIL = 0x10;
const size_t NOTE_INDEX_BGA_BASE = 0xE0;
const size_t NOTE_INDEX_BGA_LAYER = 0xE1;
const size_t NOTE_INDEX_BGA_POOR = 0xE2;

struct Note
{
    Measure measure;        // Which measure the note is placed
    Beat totalbeat;        // Which beat the note is placed in visual (ignoring SV & STOP), can be above 1
    Time time;             // Timestamp
    std::variant<long long, double> value;              // varies from note type to type, e.g. #BGM, #BPM, etc
	size_t index = INDEX_INVALID;			// used for distinguishing plain notes
};
