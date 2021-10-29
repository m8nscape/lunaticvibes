#pragma once
#include <array>
#include <vector>
#include <list>
#include <utility>
#include <chrono>
#include "common/beat.h"
#include "game/data/timer.h"
#include "game/input/input_mgr.h"

struct HitableNote: Note
{
    bool hit = false;
};

namespace chart
{

enum class NoteLaneCategory : size_t
{
    Note,
    Mine,
    Invs,
    LN,
    EXTRA,
    NOTECATEGORY_COUNT,

    _ // INVALID
};

enum NoteLaneIndex : size_t
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
    NOTELANEINDEX_COUNT,
    _ // INVALID
};

typedef std::pair<NoteLaneCategory, NoteLaneIndex> NoteLane;

enum NoteLaneExtra : size_t
{
    EXTRA_BARLINE,

    NOTELANEEXTRA_COUNT = NOTELANEINDEX_COUNT,
};
constexpr size_t LANE_ALL_KEY_COUNT = size_t(NoteLaneCategory::NOTECATEGORY_COUNT) * NOTELANEINDEX_COUNT;
constexpr size_t LANE_BARLINE = LANE_ALL_KEY_COUNT;
constexpr size_t LANE_INVALID = LANE_ALL_KEY_COUNT + 1;
constexpr size_t LANE_COUNT = LANE_INVALID + 1;
constexpr size_t channelToIdx(NoteLaneCategory cat, size_t idx)
{
    if (cat == NoteLaneCategory::EXTRA && idx == NoteLaneExtra::EXTRA_BARLINE)
        return LANE_BARLINE;

    auto ch = size_t(cat) * NOTELANEINDEX_COUNT + idx;
    return (ch >= LANE_ALL_KEY_COUNT) ? LANE_INVALID : ch;
}
constexpr NoteLane idxToChannel(size_t idx)
{
    if (idx >= size_t(NoteLaneCategory::EXTRA) * NoteLaneIndex::NOTELANEINDEX_COUNT + NoteLaneExtra::NOTELANEEXTRA_COUNT)
        return { NoteLaneCategory::_, NoteLaneIndex::_ };

    size_t ch = idx / NOTELANEINDEX_COUNT;
    return { NoteLaneCategory(ch), NoteLaneIndex(idx % ch) };
}

class ::vChartFormat;

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
    static constexpr size_t MAX_MEASURES = 1000;
protected:
    int _playerSlot = -1;

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
    std::array<std::list<HitableNote>, LANE_COUNT> _noteLists;
    std::vector<std::list<Note>> _bgmNoteLists;      // BGM notes; handled with timer
    std::vector<std::list<Note>> _specialNoteLists;     // Special definitions for each format. e.g. BGA, Stop
    std::list<Note>              _bpmNoteList;          // BPM change is so common that they are not special

protected:
    std::array<Beat,  MAX_MEASURES>   barLength;
    std::array<Beat,  MAX_MEASURES>   _barBeatstamp;
    std::array<Time, MAX_MEASURES>   _barTimestamp;

    Time _totalLength;


public:
    vChart() = delete;
    vChart(int slot, size_t plain_n, size_t ext_n);
    static std::shared_ptr<vChart> createFromChartFormat(int slot, std::shared_ptr<vChartFormat> p);

private:
    std::array<decltype(_noteLists.front().begin()), LANE_COUNT>    _noteListIterators;
    std::vector<decltype(_bgmNoteLists.front().begin())>            _bgmNoteListIters;
    std::vector<decltype(_specialNoteLists.front().begin())>        _specialNoteListIters;
    decltype(_bpmNoteList.begin())                                  _bpmNoteListIter;

public:
    auto firstNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteLists.front().begin());

    auto incomingNote         (NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front());
    auto incomingNoteBgm      (size_t idx) -> decltype(_bgmNoteListIters.front());
    auto incomingNoteSpecial  (size_t idx) -> decltype(_specialNoteListIters.front());
    auto incomingNoteBpm      () -> decltype(_bpmNoteListIter);

    bool isLastNote           (NoteLaneCategory cat, NoteLaneIndex idx);
    bool isLastNoteBgm        (size_t idx);
    bool isLastNoteSpecial    (size_t idx);
    bool isLastNoteBpm        ();

    bool isLastNote           (NoteLaneCategory cat, NoteLaneIndex idx, decltype(_noteListIterators.front()) it);
    bool isLastNoteBgm        (size_t idx, decltype(_bgmNoteListIters.front()) it);
    bool isLastNoteSpecial    (size_t idx, decltype(_specialNoteListIters.front()) it);
    bool isLastNoteBpm        (decltype(_bpmNoteListIter) it);

    auto nextNote             (NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front());
    auto nextNoteBgm          (size_t idx) -> decltype(_bgmNoteListIters.front());
    auto nextNoteSpecial      (size_t idx) -> decltype(_specialNoteListIters.front());
    auto nextNoteBpm          () -> decltype(_bpmNoteListIter);

public:
    Time getBarLength(size_t measure);
    Time getCurrentBarLength();
    Beat getBarLengthInBeats(size_t measure);
	Beat getCurrentBarLengthInBeats();
    Beat getBarBeatstamp(size_t measure);
	Time getBarTimestamp(size_t m) { return m < MAX_MEASURES ? _barTimestamp[m] : LLONG_MAX; }
	Time getCurrentBarTimestamp() { return getBarTimestamp(_currentBar); }

protected:
    unsigned _currentBar           = 0;
    double   _currentBeat          = 0;
    BPM      _currentBPM           = 150.0;
    Time     _lastChangedBPMTime   = 0;
    double   _lastChangedBeat      = 0;

public:
    void reset();
    void resetNoteListsIterators();            // set after parsing
    /*virtual*/ void update(Time t);
    virtual void preUpdate(const Time& t) = 0;
    virtual void postUpdate(const Time& t) = 0;
    constexpr auto getCurrentMeasure() -> decltype(_currentBar) { return _currentBar; }
    constexpr auto getCurrentBeat() -> decltype(_currentBeat) { return _currentBeat; }
    inline auto getCurrentTotalBeats() -> decltype(_currentBeat) { return _currentBeat + _barBeatstamp[_currentBar]; }
    constexpr auto getCurrentBPM() -> decltype(_currentBPM) { return _currentBPM; }

public:
    std::list<HitableNote>  noteExpired;
    std::list<Note>   noteBgmExpired;
    std::list<Note>   noteSpecialExpired;

public:
    virtual NoteLane getLaneFromKey(Input::Pad input) = 0;
    virtual std::vector<Input::Pad> getInputFromLane(size_t channel) = 0;

    inline auto getTotalLength() const { return _totalLength; }
};

}
