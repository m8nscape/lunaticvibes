#include "scroll.h"

vScroll::vScroll( size_t pn, size_t en) :
    _noteLists{}, _plainLists(pn), _extLists(en), _plainListIterators(pn), _extListIterators(en)
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
    _currentMeasure    = 0;
    _currentBeat       = 0;
    _currentRenderBeat = 0;
    _currentBPM        = 150;
    _currentSpeed      = 1.0;
    _lastChangedTime   = 0;
    _lastChangedBeat   = 0;
}

void vScroll::setIterators()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)  _noteListIterators[i]  = _noteLists[i].begin();
    for (size_t i = 0; i < _plainLists.size(); ++i) _plainListIterators[i] = _plainLists[i].begin();
    for (size_t i = 0; i < _extLists.size(); ++i)   _extListIterators[i]   = _extLists[i].begin();
    _bpmListIterator   = _bpmList.begin();
    _speedListIterator = _scrollingSpeedList.begin();
}

auto vScroll::lastNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto vScroll::lastNoteOfPlainChannel(size_t channel) -> decltype(_plainListIterators.front())
{
    return _plainListIterators[channel];
}
auto vScroll::lastNoteOfExtChannel(size_t channel) -> decltype(_extListIterators.front())
{
    return _extListIterators[channel];
}
auto vScroll::lastNoteOfBpm() -> decltype(_bpmListIterator)
{
    return _bpmListIterator;
}
auto vScroll::lastNoteOfSpeed() -> decltype(_speedListIterator)
{
    return _speedListIterator;
}

bool vScroll::isLastNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx)
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || 
        lastNoteOfChannel(cat, idx) == _noteLists[channel].end();
}
bool vScroll::isLastNoteOfPlainChannel(size_t channel)
{
    return _plainLists[channel].empty() ||
        lastNoteOfPlainChannel(channel) == _plainLists[channel].end();
}
bool vScroll::isLastNoteOfExtChannel(size_t channel)
{
    return _extLists[channel].empty() || 
        lastNoteOfExtChannel(channel) == _extLists[channel].end(); 
}

bool vScroll::isLastNoteOfBpm() { return _bpmList.empty() || lastNoteOfBpm() == _bpmList.end(); }
bool vScroll::isLastNoteOfSpeed() { return _scrollingSpeedList.empty() || lastNoteOfSpeed() == _scrollingSpeedList.end(); }

auto vScroll::succNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return ++_noteListIterators[channel];
}

auto vScroll::succNoteOfPlainChannel(size_t channel) -> decltype(_plainListIterators.front())
{
    return ++_plainListIterators[channel];
}
auto vScroll::succNoteOfExtChannel(size_t channel) -> decltype(_extListIterators.front())
{
    return ++_extListIterators[channel];
}
auto vScroll::succNoteOfBpm() -> decltype(_bpmListIterator)
{
    return ++_bpmListIterator;
}
auto vScroll::succNoteOfSpeed() -> decltype(_speedListIterator)
{
    return ++_speedListIterator;
}

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

void vScroll::update(hTime t)
{
    noteExpired.clear();
    notePlainExpired.clear();
    noteExtExpired.clear();

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
    auto b = lastNoteOfBpm();
    while (!isLastNoteOfBpm() && t >= b->time)
    {
        _currentBeat = b->rawBeat;
        _currentBPM = std::get<BPM>(b->value);
        _currentRenderBeat = b->renderPos;
        beatLength = hConvertBPM(_currentBPM);
        _lastChangedTime = b->time;
        _lastChangedBeat = b->rawBeat;
        _lastChangedSpeedTime = b->time;
        b = succNoteOfBpm();
    }

    // scroll speed
    auto s = lastNoteOfSpeed();
    while (!isLastNoteOfSpeed() && t >= s->time)
    {
        _currentSpeed = std::get<double>(s->value);
        _currentRenderBeat = s->renderPos;
        _lastChangedSpeedTime = s->time;
        s = succNoteOfSpeed();
    }

    // Skip expired notes
    for (NoteChannelCategory cat = NoteChannelCategory::Note; (size_t)cat < (size_t)NoteChannelCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
    for (NoteChannelIndex idx = Sc1; idx < NOTECHANNEL_COUNT; ++*((size_t*)&idx))
    {
        auto it = lastNoteOfChannel(cat, idx);
        while (!isLastNoteOfChannel(cat, idx) && (t >= it->time + 1000 || !it->hit))
        {
            it->hit = true;
            noteExpired.push_back(*it);
            it = succNoteOfChannel(cat, idx);
        }
    } 

    // Skip expired plain note
    for (size_t idx = 0; idx < _plainLists.size(); ++idx)
    {
        auto it = lastNoteOfPlainChannel(idx);
        while (!isLastNoteOfPlainChannel(idx) && t >= it->time)
        {
            notePlainExpired.push_back(*it);
            it = succNoteOfPlainChannel(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _extLists.size(); ++idx)
    {
        auto it = lastNoteOfExtChannel(idx);
        while (!isLastNoteOfExtChannel(idx) && t >= it->time)
        {
            noteExtExpired.push_back(*it);
            it = succNoteOfExtChannel(idx);
        }
    } 

    // update current info
    hTime currentMeasureTimePassed = t - _measureTimestamp[_currentMeasure];
    hTime cmtpFromBPMChange = currentMeasureTimePassed - _lastChangedTime;
    hTime cmtpFromSpdChange = currentMeasureTimePassed - _lastChangedSpeedTime;

    using namespace std::chrono;
    _currentBeat += (double)cmtpFromBPMChange / beatLength;
    _currentRenderBeat += (double)cmtpFromSpdChange * _currentSpeed / beatLength;
}
