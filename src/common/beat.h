#pragma once
#include "fraction.h"
#include <chrono>
#include <variant>

typedef unsigned Measure;
typedef double BPM;
typedef fraction Beat;      // rhythm indicator, can be above 1
typedef fraction BeatNorm;  // Normal rhythm indicator, must be normalized (value: [0, 1)).

typedef std::chrono::milliseconds timeRes;
typedef decltype(std::declval<timeRes>().count()) rTime;    // Regular time, expect millisecond (1/1e3 second). Used for general timing demand
inline rTime getTimePoint() { return std::chrono::duration_cast<timeRes>(std::chrono::system_clock::now().time_since_epoch()).count(); }

typedef std::chrono::nanoseconds timeHighRes;
typedef decltype(std::declval<timeHighRes>().count()) hTime;    // High resolution time, expect nanosecond (1/1e9 second). Used for note timings
inline hTime getHighresTimePoint() { return std::chrono::duration_cast<timeHighRes>(std::chrono::system_clock::now().time_since_epoch()).count(); }

constexpr rTime h2r(hTime h) { return (rTime)(std::chrono::duration_cast<timeRes>(timeHighRes(h)).count()); }
constexpr hTime r2h(rTime r) { return (hTime)(std::chrono::duration_cast<timeHighRes>(timeRes(r)).count()); }

constexpr rTime rConvertBPM(BPM b) { using namespace std::chrono; return (rTime)(6e5 / b * duration_cast<timeRes>(1ms).count()); }
constexpr hTime hConvertBPM(BPM b) { using namespace std::chrono; return (hTime)(6e5 / b * duration_cast<timeHighRes>(1ms).count()); }

struct Note
{
    Measure measure;        // Which measure the note is placed
    Beat rawBeat;           // Which beat the note is placed, ignoring scrolling speed changes
    double renderPos;      // Where should the note be rendered, with consideration of scrolling speed
    hTime time;             // Time point in hTime (currently ns)
    std::variant<long long, double> value;              // varies from note type to type, e.g. #BGM, #BPM, etc
};
