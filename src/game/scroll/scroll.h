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

enum class NoteChannelCategory: size_t
{
    Note,
    Mine,
    Invs,
    LN,
    _, // INVALID
    NOTECATEGORY_COUNT,
};

enum NoteChannelIndex: size_t
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

constexpr size_t channelToIdx(NoteChannelCategory cat, NoteChannelIndex idx)
{
	return (size_t)cat * NOTECHANNEL_COUNT + idx;
}

// Chart in-game data representation. Contains following:
//  - Converts plain-beat to real-beat (adds up Stop beat) 
//  - Converts time to beat (if necessary)
//  - Converts (or reads directly) beat to time
//  - Measure Time Indicator
//  - Segmented Scrolling speed (power, 0 at stop)
// [Input]  Current Time(hTime)
// [Output] Current Measure(unsigned) 
//          Beat(double)
//          BPM(BPM)
//          Expired Notes List (including normal, plain, ext, speed)
class vScroll
{
public:
    static const size_t MAX_MEASURES = 1000;
    static const size_t CHANNEL_COUNT = (size_t)NoteChannelCategory::NOTECATEGORY_COUNT * NOTECHANNEL_COUNT;

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

    std::vector<std::list<Note>> _plainLists;     // basically sNote without hit param, including BGM, BGA; handled with timer
    std::vector<std::list<Note>> _extLists;       // e.g. Stop in BMS
    std::list<Note>              _bpmList;
    std::list<Note>              _stopList;

    std::array<Beat,  MAX_MEASURES>   _measureLength;
    std::array<Beat,  MAX_MEASURES>   _measureTotalBeats;
    std::array<timestamp, MAX_MEASURES>   _measureTimestamp;


public:
    vScroll() = delete;
    vScroll(size_t plain_n, size_t ext_n);

private:
    std::array<decltype(_noteLists.front().begin()), CHANNEL_COUNT> _noteListIterators;
    std::vector<decltype(_plainLists.front().begin())>  _plainListIterators;
    std::vector<decltype(_extLists.front().begin())>    _extListIterators;
    decltype(_bpmList.begin())   _bpmListIterator;
    decltype(_stopList.begin())  _stopListIterator;
public:
    auto incomingNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front());
    auto incomingNoteOfPlainChannel (size_t idx) -> decltype(_plainListIterators.front());
    auto incomingNoteOfExtChannel   (size_t idx) -> decltype(_extListIterators.front());
    auto incomingNoteOfBpm          () -> decltype(_bpmListIterator);
    auto incomingNoteOfStop         () -> decltype(_stopListIterator);
    bool isLastNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx);
    bool isLastNoteOfPlainChannel (size_t idx);
    bool isLastNoteOfExtChannel   (size_t idx);
    bool isLastNoteOfBpm();
    bool isLastNoteOfStop();
    bool isLastNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx, decltype(_noteListIterators.front()) it);
    bool isLastNoteOfPlainChannel (size_t idx, decltype(_plainListIterators.front()) it);
    bool isLastNoteOfExtChannel   (size_t idx, decltype(_extListIterators.front()) it);
    bool isLastNoteOfBpm          (decltype(_bpmListIterator) it);
    bool isLastNoteOfStop         (decltype(_stopListIterator) it);
    auto succNoteOfChannel      (NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front());
    auto succNoteOfPlainChannel (size_t idx) -> decltype(_plainListIterators.front());
    auto succNoteOfExtChannel   (size_t idx) -> decltype(_extListIterators.front());
    auto succNoteOfBpm          () -> decltype(_bpmListIterator);
    auto succNoteOfStop         () -> decltype(_stopListIterator);

public:
    timestamp getMeasureLength(size_t measure);
    timestamp getCurrentMeasureLength();
    Beat  getMeasureBeat(size_t measure);
	Beat  getCurrentMeasureBeat();
    Beat  getMeasureTotalBeats(size_t measure);
	timestamp getMeasureTime(size_t m) { return m < MAX_MEASURES ? _measureTimestamp[m] : LLONG_MAX; }
	timestamp getCurrentMeasureTime() { return getMeasureTime(_currentMeasure); }

protected:
    unsigned _currentMeasure    = 0;
    double   _currentBeat       = 0;
    BPM      _currentBPM        = 150.0;
    timestamp    _lastChangedBPMTime   = 0;
    double   _lastChangedBeat   = 0;

public:
    void reset();
    void setIterators();            // set after parsing
    /*virtual*/ void update(timestamp t);
    constexpr auto getCurrentMeasure() -> decltype(_currentMeasure) { return _currentMeasure; }
    constexpr auto getCurrentBeat() -> decltype(_currentBeat) { return _currentBeat; }
    inline auto getCurrentTotalBeats() -> decltype(_currentBeat) { return _currentBeat + _measureTotalBeats[_currentMeasure]; }
    constexpr auto getCurrentBPM() -> decltype(_currentBPM) { return _currentBPM; }

public:
    std::list<sNote>  noteExpired;
    std::list<Note>   notePlainExpired;
    std::list<Note>   noteExtExpired;

public:
    virtual std::pair<NoteChannelCategory, NoteChannelIndex> getChannelFromKey(Input::Ingame input) = 0;
    virtual std::vector<Input::Ingame> getInputFromChannel(size_t channel) = 0;
};
