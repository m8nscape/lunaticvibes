#pragma once
#include <array>
#include <vector>
#include <list>
#include <utility>
#include <chrono>
#include "beat.h"
#include "game/data/timer.h"
#include "game/input/input_mgr.h"

struct sNote: Note
{
    bool hit = false;
};

enum class NoteLaneCategory: size_t
{
    Note,
    Mine,
    Invs,
    LN,
    EXTRA,
    NOTECATEGORY_COUNT,

    _ // INVALID
};

enum NoteLaneIndex: size_t
{
    Sc1 = 0,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    K8,
    K9,
    K10,
    K11,
    K12,
    K13,
    K14,
    K15,
    K16,
    K17,
    K18,
    K19,
    K20,
    K21,
    K22,
    K23,
    K24,
    Sc2,
    NOTECHANNEL_COUNT,
    _ // INVALID
};

enum NoteLaneExtra : size_t
{
    EXTRA_BARLINE,

    NOTEEXTRA_COUNT = NOTECHANNEL_COUNT,
};
constexpr size_t CHANNEL_KEY_COUNT = size_t(NoteLaneCategory::NOTECATEGORY_COUNT) * NOTECHANNEL_COUNT;
constexpr size_t CHANNEL_BARLINE = CHANNEL_KEY_COUNT;
constexpr size_t CHANNEL_INVALID = CHANNEL_KEY_COUNT + 1;
constexpr size_t CHANNEL_COUNT = CHANNEL_INVALID + 1;
constexpr size_t channelToIdx(NoteLaneCategory cat, size_t idx)
{
    if (cat == NoteLaneCategory::EXTRA && idx == NoteLaneExtra::EXTRA_BARLINE)
        return CHANNEL_BARLINE;

    auto ch = size_t(cat) * NOTECHANNEL_COUNT + idx;
    return (ch >= CHANNEL_KEY_COUNT) ? CHANNEL_INVALID : ch;
}
constexpr std::pair<NoteLaneCategory, NoteLaneIndex> idxToChannel(size_t idx)
{
    if (idx >= size_t(NoteLaneCategory::EXTRA) * NoteLaneIndex::NOTECHANNEL_COUNT + NoteLaneExtra::NOTEEXTRA_COUNT)
        return { NoteLaneCategory::_, NoteLaneIndex::_};

    size_t ch = idx / NOTECHANNEL_COUNT;
    return { NoteLaneCategory(ch), NoteLaneIndex(idx % ch) };
}

class vChartFormat;

// Chart in-game data representation. Contains following:
//  - Converts plain-beat to real-beat (adds up Stop beat) 
//  - Converts time to beat (if necessary)
//  - Converts (or reads directly) beat to time
//  - Measure Time Indicator
//  - Segmented charting speed (power, 0 at stop)
// [Input]  Current Time(hTime)
// [Output] Current Measure(unsigned) 
//          Beat(double)
//          BPM(BPM)
//          Expired Notes List (including normal, plain, ext, speed)
class vChart
{
public:
    static const size_t MAX_MEASURES = 1000;

protected:
	unsigned _noteCount;
	unsigned _noteCount_regular;
	unsigned _noteCount_ln;
public:
	unsigned constexpr getNoteCount() const { return _noteCount; }
	unsigned constexpr getNoteRegularCount() const { return _noteCount_regular; }
	unsigned constexpr getNoteLnCount() const { return _noteCount_ln; }

protected:

     // full list of corresponding channel through all measures; only this list is handled by input looper
    std::array<std::list<sNote>, CHANNEL_COUNT> _noteLists;

    std::vector<std::list<Note>> _commonNoteLists;     // basically sNote without hit param, including BGM, BGA; handled with timer
    std::vector<std::list<Note>> _specialNoteLists;       // e.g. Stop in BMS
    std::list<Note>              _bpmNoteList;
    std::list<Note>              _stopNoteList;

    std::array<Beat,  MAX_MEASURES>   barLength;
    std::array<Beat,  MAX_MEASURES>   _barBeatstamp;
    std::array<Time, MAX_MEASURES>   _barTimestamp;

    Time _totalLength;


public:
    vChart() = delete;
    vChart(size_t plain_n, size_t ext_n);
    static std::shared_ptr<vChart> getFromChart(std::shared_ptr<vChartFormat> p);

private:
    std::array<decltype(_noteLists.front().begin()), CHANNEL_INVALID + 1> _noteListIterators;
    std::vector<decltype(_commonNoteLists.front().begin())>  _commonNoteListIters;
    std::vector<decltype(_specialNoteLists.front().begin())>    _specialNoteListIters;
    decltype(_bpmNoteList.begin())   _bpmNoteListIter;
    decltype(_stopNoteList.begin())  _stopNoteListIter;

public:
    auto firstNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteLists.front().begin());

    auto incomingNoteOfLane      (NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front());
    auto incomingNoteOfCommonLane (size_t idx) -> decltype(_commonNoteListIters.front());
    auto incomingNoteOfSpecialLane   (size_t idx) -> decltype(_specialNoteListIters.front());
    auto incomingNoteOfBpmLane          () -> decltype(_bpmNoteListIter);
    auto incomingNoteOfStopLane         () -> decltype(_stopNoteListIter);

    bool isLastNoteOfLane      (NoteLaneCategory cat, NoteLaneIndex idx);
    bool isLastNoteOfCommonLane (size_t idx);
    bool isLastNoteOfSpecialLane   (size_t idx);
    bool isLastNoteOfBpmLane();
    bool isLastNoteOfStopLane();

    bool isLastNoteOfLane      (NoteLaneCategory cat, NoteLaneIndex idx, decltype(_noteListIterators.front()) it);
    bool isLastNoteOfCommonLane (size_t idx, decltype(_commonNoteListIters.front()) it);
    bool isLastNoteOfSpecialLane   (size_t idx, decltype(_specialNoteListIters.front()) it);
    bool isLastNoteOfBpmLane          (decltype(_bpmNoteListIter) it);
    bool isLastNoteOfStopLane         (decltype(_stopNoteListIter) it);

    auto nextNoteOfLane      (NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front());
    auto nextNoteOfCommonLane (size_t idx) -> decltype(_commonNoteListIters.front());
    auto nextNoteOfSpecialLane   (size_t idx) -> decltype(_specialNoteListIters.front());
    auto nextNoteOfBpmLane          () -> decltype(_bpmNoteListIter);
    auto nextNoteOfStopLane         () -> decltype(_stopNoteListIter);

public:
    Time getBarLength(size_t measure);
    Time getCurrentBarLength();
    Beat  getBarLengthInBeats(size_t measure);
	Beat  getCurrentBarLengthInBeats();
    Beat  getBarBeatstamp(size_t measure);
	Time getBarTimestamp(size_t m) { return m < MAX_MEASURES ? _barTimestamp[m] : LLONG_MAX; }
	Time getCurrentBarTimestamp() { return getBarTimestamp(_currentBar); }

protected:
    unsigned _currentBar    = 0;
    double   _currentBeat       = 0;
    BPM      _currentBPM        = 150.0;
    Time    _lastChangedBPMTime   = 0;
    double   _lastChangedBeat   = 0;
    double   _currentStopBeat = 0;
    bool     _currentStopBeatGuard = false;

public:
    void reset();
    void setNoteListsIterators();            // set after parsing
    /*virtual*/ void update(Time t);
    constexpr auto getCurrentMeasure() -> decltype(_currentBar) { return _currentBar; }
    constexpr auto getCurrentBeat() -> decltype(_currentBeat) { return _currentBeat; }
    inline auto getCurrentTotalBeats() -> decltype(_currentBeat) { return _currentBeat + _barBeatstamp[_currentBar]; }
    constexpr auto getCurrentBPM() -> decltype(_currentBPM) { return _currentBPM; }

public:
    std::list<sNote>  noteExpired;
    std::list<Note>   notePlainExpired;
    std::list<Note>   noteExtExpired;

public:
    virtual std::pair<NoteLaneCategory, NoteLaneIndex> getLaneFromKey(Input::Ingame input) = 0;
    virtual std::vector<Input::Ingame> getInputFromLane(size_t channel) = 0;

    inline auto getTotalLength() const { return _totalLength; }
};
