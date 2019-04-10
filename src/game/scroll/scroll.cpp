#include "scroll.h"
#include "game/data/number.h"

vScroll::vScroll( size_t pn, size_t en) :
    _noteLists{}, _plainLists(pn), _extLists(en), _plainListIterators(pn), _extListIterators(en)
{
    reset();
	_measureTimestamp.fill({LLONG_MAX, false});
    _measureTimestamp[0] = 0.0;
    _bpmList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    _stopList.clear();
    //_stopList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}

void vScroll::reset()
{
    _currentMeasure    = 0;
    _currentBeat       = 0;
    _currentBPM        = 150;
    _lastChangedBPMTime = 0;
    _lastChangedBeat   = 0;
}

void vScroll::setIterators()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)  _noteListIterators[i]  = _noteLists[i].begin();
    for (size_t i = 0; i < _plainLists.size(); ++i) _plainListIterators[i] = _plainLists[i].begin();
    for (size_t i = 0; i < _extLists.size(); ++i)   _extListIterators[i]   = _extLists[i].begin();
    _bpmListIterator   = _bpmList.begin();
    _stopListIterator = _stopList.begin();
}

auto vScroll::incomingNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto vScroll::incomingNoteOfPlainChannel(size_t channel) -> decltype(_plainListIterators.front())
{
    return _plainListIterators[channel];
}
auto vScroll::incomingNoteOfExtChannel(size_t channel) -> decltype(_extListIterators.front())
{
    return _extListIterators[channel];
}
auto vScroll::incomingNoteOfBpm() -> decltype(_bpmListIterator)
{
    return _bpmListIterator;
}
auto vScroll::incomingNoteOfStop() -> decltype(_stopListIterator)
{
    return _stopListIterator;
}
bool vScroll::isLastNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx, decltype(_noteListIterators.front()) it)
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || it == _noteLists[channel].end();
}
bool vScroll::isLastNoteOfPlainChannel(size_t idx, decltype(_plainListIterators.front()) it)
{
    return _plainLists[idx].empty()    || it == _plainLists[idx].end();
}
bool vScroll::isLastNoteOfExtChannel(size_t idx, decltype(_extListIterators.front()) it)
{
    return _extLists[idx].empty()      ||  it == _extLists[idx].end(); 
}
bool vScroll::isLastNoteOfBpm(decltype(_bpmListIterator) it)
{
	return _bpmList.empty()            || it == _bpmList.end(); 
}
bool vScroll::isLastNoteOfStop(decltype(_stopListIterator) it)
{
	return _stopList.empty() || it == _stopList.end(); 
}

bool vScroll::isLastNoteOfChannel(NoteChannelCategory cat, NoteChannelIndex idx)
{
	return isLastNoteOfChannel(cat, idx, incomingNoteOfChannel(cat, idx));
}
bool vScroll::isLastNoteOfPlainChannel(size_t channel)
{
	return isLastNoteOfPlainChannel(channel, incomingNoteOfPlainChannel(channel));
}
bool vScroll::isLastNoteOfExtChannel(size_t channel)
{
	return isLastNoteOfExtChannel(channel, incomingNoteOfExtChannel(channel));
}
bool vScroll::isLastNoteOfBpm()
{
	return isLastNoteOfBpm(incomingNoteOfBpm());
}
bool vScroll::isLastNoteOfStop()
{
	return isLastNoteOfStop(incomingNoteOfStop());
}

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
auto vScroll::succNoteOfStop() -> decltype(_stopListIterator)
{
    return ++_stopListIterator;
}

timestamp vScroll::getMeasureLength(size_t measure)
{
    if (measure + 1 >= _measureTimestamp.size())
    {
        return -1;
    }

    auto l = _measureTimestamp[measure + 1] - _measureTimestamp[measure];
	return l.hres() > 0 ? l : -1;
}

timestamp vScroll::getCurrentMeasureLength()
{
    return getMeasureLength(_currentMeasure);
}

Beat vScroll::getMeasureBeat(size_t measure)
{
    if (measure >= _measureLength.size())
    {
		return { 1, 1 };
    }

	return _measureLength[measure];
}

Beat vScroll::getCurrentMeasureBeat()
{
	return getMeasureBeat(_currentMeasure);
}

void vScroll::update(timestamp t)
{
    noteExpired.clear();
    notePlainExpired.clear();
    noteExtExpired.clear();

	timestamp beatLength = timestamp::fromBPM(_currentBPM);

    // Go through expired measures
    while (_currentMeasure + 1 < MAX_MEASURES && t >= _measureTimestamp[_currentMeasure + 1])
    {
        ++_currentMeasure;
        _currentBeat = 0;
        _lastChangedBPMTime = 0;
        _lastChangedBeat = 0;
    }

    // check inbounds BPM change
    auto b = incomingNoteOfBpm();
    while (!isLastNoteOfBpm(b) && t >= b->time)
    {
        _currentBeat = b->rawBeat;
        _currentBPM = std::get<BPM>(b->value);
		beatLength = timestamp::fromBPM(_currentBPM);
        _lastChangedBPMTime = b->time;
        _lastChangedBeat = b->rawBeat;
        b = succNoteOfBpm();
    }

    // Skip expired notes
    for (NoteChannelCategory cat = NoteChannelCategory::Note; (size_t)cat < (size_t)NoteChannelCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
    for (NoteChannelIndex idx = Sc1; idx < NOTECHANNEL_COUNT; ++*((size_t*)&idx))
    {
        auto it = incomingNoteOfChannel(cat, idx);
        while (!isLastNoteOfChannel(cat, idx, it) && t >= it->time && it->hit)
        {
            noteExpired.push_back(*it);
            it = succNoteOfChannel(cat, idx);
        }
    } 

    // Skip expired plain note
    for (size_t idx = 0; idx < _plainLists.size(); ++idx)
    {
        auto it = incomingNoteOfPlainChannel(idx);
        while (!isLastNoteOfPlainChannel(idx) && t >= it->time)
        {
            notePlainExpired.push_back(*it);
            it = succNoteOfPlainChannel(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _extLists.size(); ++idx)
    {
        auto it = incomingNoteOfExtChannel(idx);
        while (!isLastNoteOfExtChannel(idx) && t >= it->time)
        {
            noteExtExpired.push_back(*it);
            it = succNoteOfExtChannel(idx);
        }
    } 

    // update current info
    timestamp currentMeasureTimePassed = t - _measureTimestamp[_currentMeasure];
	timestamp cmtpFromBPMChange = currentMeasureTimePassed - _lastChangedBPMTime;
	_currentBeat = _lastChangedBeat + (double)cmtpFromBPMChange.hres() / (beatLength.hres() * getCurrentMeasureBeat());
	gNumbers.set(eNumber::_TEST4, currentMeasureTimePassed.norm());
}
