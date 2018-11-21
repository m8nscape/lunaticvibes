#include "scroll.h"

vScroll::vScroll( size_t pn, size_t en) :
    _plainLists(pn), _extLists(en), _plainListIterators(pn), _extListIterators(en)
{
    reset();
    _measureTimestamp.fill(LLONG_MAX);
    _measureTimestamp[0] = 0.0;
    _bpmList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    _scrollingSpeedList.clear();
    _scrollingSpeedList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}

void vScroll::reset()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)  _noteListIterators[i]  = _noteLists[i].begin();
    for (size_t i = 0; i < _plainLists.size(); ++i) _plainListIterators[i] = _plainLists[i].begin();
    for (size_t i = 0; i < _extLists.size(); ++i)   _extListIterators[i]   = _extLists[i].begin();
    _bpmListIterator   = _bpmList.begin();
    _speedListIterator = _scrollingSpeedList.begin();
    _currentMeasure    = 0;
    _currentBeat       = 0;
    _currentRenderBeat = 0;
    _currentBPM        = 150;
    _currentSpeed      = 1.0;
    _lastChangedTime   = 0;
    _lastChangedBeat   = 0;
}

auto vScroll::lastNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx, bool succ) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    if (!succ)
        return _noteListIterators[channel];

    while (_noteListIterators[channel] != _noteLists[channel].end() && _noteListIterators[channel]->hit)
        ++_noteListIterators[channel];
    return _noteListIterators[channel];
}

auto vScroll::lastNoteOfPlainChannel(size_t channel, bool succ) -> decltype(_plainListIterators.front())
{
    if (!succ || _plainListIterators[channel] != _plainLists[channel].end())
        return _plainListIterators[channel]++;
    else
        return _plainListIterators[channel];
}
auto vScroll::lastNoteOfExtChannel(size_t channel, bool succ) -> decltype(_extListIterators.front())
{
    if (!succ || _extListIterators[channel] != _extLists[channel].end())
        return _extListIterators[channel]++;
    else
        return _extListIterators[channel];
}
auto vScroll::lastNoteOfBpm(bool succ) -> decltype(_bpmListIterator)
{
    if (!succ || _bpmListIterator != _bpmList.end())
        return _bpmListIterator++;
    else
        return _bpmListIterator;
}
auto vScroll::lastNoteOfSpeed(bool succ) -> decltype(_speedListIterator)
{
    if (!succ || _speedListIterator != _scrollingSpeedList.end())
        return _speedListIterator++;
    else
        return _speedListIterator;
}

bool vScroll::isLastNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx)
{
    size_t channel = channelToIdx(cat, idx);
    return lastNoteOfChannel(cat, idx, false) == _noteLists[channel].end();
}
bool vScroll::isLastNoteOfPlainChannel(size_t channel)
{
    return lastNoteOfPlainChannel(channel, false) == _plainLists[channel].end();
}
bool vScroll::isLastNoteOfExtChannel(size_t channel)
{
    return lastNoteOfExtChannel(channel, false) == _extLists[channel].end(); 
}

bool vScroll::isLastNoteOfBpm() { return lastNoteOfBpm(false) == _bpmList.end(); }
bool vScroll::isLastNoteOfSpeed() { return lastNoteOfSpeed(false) == _scrollingSpeedList.end(); }

hTime vScroll::getMeasureLength(size_t measure)
{
    if (measure + 1 >= _measureTimestamp.size())
    {
        return -1;
    }

    double l = _measureTimestamp[measure + 1] - _measureTimestamp[measure];
    return (l > 0) ? l : -1;
}

hTime vScroll::getCurrentMeasureLength()
{
    return getMeasureLength(_currentMeasure);
}

void vScroll::update(hTime stime)
{
    noteExpired.clear();
    notePlainExpired.clear();
    noteExtExpired.clear();

    hTime t = stime - r2h(gTimers::get(eTimer::PLAY_START));
    hTime beatLength = hConvertBPM(_currentBPM);

    // Go through expired measures
    while (_currentMeasure < MAX_MEASURES && getCurrentMeasureLength() >= 0)
    {
        ++_currentMeasure;
        _currentBeat = 0;
        _currentRenderBeat = 0;
        _lastChangedTime = 0;
        _lastChangedBeat = 0;
        _lastChangedSpeedTime = 0;
    }
    
    // check inbounds BPM change
    decltype(lastNoteOfBpm()) b = lastNoteOfBpm(false);
    while (_bpmList.end() != b && t >= b->time);
    {
        b = lastNoteOfBpm();
    }
    _currentBeat = b->rawBeat;
    _currentBPM = std::get<BPM>(b->value);
    _currentRenderBeat = b->renderPos;
    beatLength = hConvertBPM(_currentBPM);
    _lastChangedTime = b->time;
    _lastChangedBeat = b->rawBeat;
    _lastChangedSpeedTime = b->time;

    // scroll speed
    decltype(lastNoteOfSpeed()) s = lastNoteOfSpeed(false);
    while (_scrollingSpeedList.end() != s && t <= s->time);
    {
        s = lastNoteOfSpeed();
    }
    _currentSpeed = std::get<double>(s->value);
    _currentRenderBeat = s->renderPos;
    _lastChangedSpeedTime = s->time;

    // Skip expired notes
    for (NoteChannelCategory cat = NoteChannelCategory::Note; (size_t)cat < (size_t)NoteChannelCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
    for (NoteChannelIndex idx = Sc1; idx < NOTECHANNEL_COUNT; ++*((size_t*)&idx))
    {
        auto it = lastNoteOfChannel(cat, idx, false);
        while (it != _noteLists[idx].end() && it->hit)
        {
            noteExpired.push_back(*it);
            it = lastNoteOfChannel(cat, idx);
        }
    } 

    // Skip expired plain note
    for (size_t idx = 0; idx < _plainLists.size(); ++idx)
    {
        auto it = lastNoteOfPlainChannel(idx, false);
        while (it != _plainLists[idx].end() && t >= it->time)
        {
            notePlainExpired.push_back(*it);
            it = lastNoteOfPlainChannel(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _extLists.size(); ++idx)
    {
        auto it = lastNoteOfExtChannel(idx, false);
        while (it != _extLists[idx].end() && t >= it->time)
        {
            noteExtExpired.push_back(*it);
            it = lastNoteOfExtChannel(idx);
        }
    } 

    // update current info
    hTime currentMeasureTimePassed = _measureTimestamp[_currentMeasure] - t;
    hTime cmtpFromBPMChange = currentMeasureTimePassed - _lastChangedTime;
    hTime cmtpFromSpdChange = currentMeasureTimePassed - _lastChangedSpeedTime;

    using namespace std::chrono;
    _currentBeat += (double)cmtpFromBPMChange / beatLength;
    _currentRenderBeat += (double)cmtpFromSpdChange * _currentSpeed / beatLength;
}
