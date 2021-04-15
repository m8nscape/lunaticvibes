#include "chart.h"
#include "common/chartformat/chartformat.h"
#include "game/data/number.h"

#include "chart_types.h"
#include "chartformat/chart_types.h"

vChart::vChart( size_t pn, size_t en) :
    _noteLists{}, _commonNoteLists(pn), _specialNoteLists(en), _commonNoteListIters(pn), _specialNoteListIters(en)
{
    reset();
	_barTimestamp.fill({LLONG_MAX, false});
    _barTimestamp[0] = 0;
    _bpmNoteList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    _stopNoteList.clear();
    //_stopList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}


std::shared_ptr<vChart> vChart::getFromChart(std::shared_ptr<vChartFormat> p)
{
    switch (p->type())
    {
    case eChartFormat::BMS:
        return std::make_shared<chartBMS>(std::reinterpret_pointer_cast<BMS>(p));
    default:
        LOG_WARNING << "[chart] Chart type unknown (" << int(p->type()) << "): " << p->filePath.string();
        return nullptr;
    }
}

void vChart::reset()
{
    _currentBar    = 0;
    _currentBeat       = 0;
    _currentBPM        = _bpmNoteList.empty()? 150 : std::get<BPM>(_bpmNoteList.front().value);
    _lastChangedBPMTime = 0;
    _lastChangedBeat   = 0;

    // reset notes
    for (auto& ch : _noteLists)
        for (auto& n : ch)
            n.hit = false;

    // reset iterators
    setNoteListsIterators();
}

void vChart::setNoteListsIterators()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)  _noteListIterators[i]  = _noteLists[i].begin();
    for (size_t i = 0; i < _commonNoteLists.size(); ++i) _commonNoteListIters[i] = _commonNoteLists[i].begin();
    for (size_t i = 0; i < _specialNoteLists.size(); ++i)   _specialNoteListIters[i]   = _specialNoteLists[i].begin();
    _bpmNoteListIter   = _bpmNoteList.begin();
    _stopNoteListIter = _stopNoteList.begin();
}

auto vChart::incomingNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto vChart::incomingNoteOfCommonLane(size_t channel) -> decltype(_commonNoteListIters.front())
{
    return _commonNoteListIters[channel];
}
auto vChart::incomingNoteOfSpecialLane(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return _specialNoteListIters[channel];
}
auto vChart::incomingNoteOfBpmLane() -> decltype(_bpmNoteListIter)
{
    return _bpmNoteListIter;
}
auto vChart::incomingNoteOfStopLane() -> decltype(_stopNoteListIter)
{
    return _stopNoteListIter;
}
bool vChart::isLastNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx, decltype(_noteListIterators.front()) it)
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || it == _noteLists[channel].end();
}
bool vChart::isLastNoteOfCommonLane(size_t idx, decltype(_commonNoteListIters.front()) it)
{
    return _commonNoteLists[idx].empty()    || it == _commonNoteLists[idx].end();
}
bool vChart::isLastNoteOfSpecialLane(size_t idx, decltype(_specialNoteListIters.front()) it)
{
    return _specialNoteLists[idx].empty()      ||  it == _specialNoteLists[idx].end(); 
}
bool vChart::isLastNoteOfBpmLane(decltype(_bpmNoteListIter) it)
{
	return _bpmNoteList.empty()            || it == _bpmNoteList.end(); 
}
bool vChart::isLastNoteOfStopLane(decltype(_stopNoteListIter) it)
{
	return _stopNoteList.empty() || it == _stopNoteList.end(); 
}

bool vChart::isLastNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx)
{
	return isLastNoteOfLane(cat, idx, incomingNoteOfLane(cat, idx));
}
bool vChart::isLastNoteOfCommonLane(size_t channel)
{
	return isLastNoteOfCommonLane(channel, incomingNoteOfCommonLane(channel));
}
bool vChart::isLastNoteOfSpecialLane(size_t channel)
{
	return isLastNoteOfSpecialLane(channel, incomingNoteOfSpecialLane(channel));
}
bool vChart::isLastNoteOfBpmLane()
{
	return isLastNoteOfBpmLane(incomingNoteOfBpmLane());
}
bool vChart::isLastNoteOfStopLane()
{
	return isLastNoteOfStopLane(incomingNoteOfStopLane());
}

auto vChart::nextNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return ++_noteListIterators[channel];
}

auto vChart::nextNoteOfCommonLane(size_t channel) -> decltype(_commonNoteListIters.front())
{
    return ++_commonNoteListIters[channel];
}
auto vChart::nextNoteOfSpecialLane(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return ++_specialNoteListIters[channel];
}
auto vChart::nextNoteOfBpmLane() -> decltype(_bpmNoteListIter)
{
    return ++_bpmNoteListIter;
}
auto vChart::nextNoteOfStopLane() -> decltype(_stopNoteListIter)
{
    return ++_stopNoteListIter;
}

Time vChart::getBarLength(size_t measure)
{
    if (measure + 1 >= _barTimestamp.size())
    {
        return -1;
    }

    auto l = _barTimestamp[measure + 1] - _barTimestamp[measure];
	return l.hres() > 0 ? l : -1;
}

Time vChart::getCurrentBarLength()
{
    return getBarLength(_currentBar);
}

Beat vChart::getBarLengthInBeats(size_t measure)
{
    if (measure >= barLength.size())
    {
		return { 1, 1 };
    }

	return barLength[measure];
}

Beat vChart::getCurrentBarLengthInBeats()
{
	return getBarLengthInBeats(_currentBar);
}

Beat vChart::getBarBeatstamp(size_t measure)
{
    if (measure >= _barBeatstamp.size())
    {
		return Beat(LLONG_MAX, 1);
    }

	return _barBeatstamp[measure];
}

void vChart::update(Time t)
{
    noteExpired.clear();
    notePlainExpired.clear();
    noteExtExpired.clear();

	Time beatLength = Time::singleBeatLengthFromBPM(_currentBPM);

    // Go through expired measures
    while (_currentBar + 1 < MAX_MEASURES && t >= _barTimestamp[_currentBar + 1])
    {
        ++_currentBar;
        _currentBeat = 0;
        _lastChangedBPMTime = 0;
        _lastChangedBeat = 0;
        _currentStopBeat = 0;
        _currentStopBeatGuard = false;

        auto st = incomingNoteOfStopLane();
        while (!isLastNoteOfStopLane(st) && st->measure < _currentBar)
        {
            st = nextNoteOfStopLane();
        }
    }

    // check inbounds BPM change
    auto b = incomingNoteOfBpmLane();
    while (!isLastNoteOfBpmLane(b) && t >= b->time)
    {
        //_currentBeat = b->totalbeat - getCurrentMeasureBeat();
        _currentBPM = std::get<BPM>(b->value);
		beatLength = Time::singleBeatLengthFromBPM(_currentBPM);
        _lastChangedBPMTime = b->time - _barTimestamp[_currentBar];
        _lastChangedBeat = b->totalbeat - _barBeatstamp[_currentBar];
        b = nextNoteOfBpmLane();
    }

    // check stop
    bool inStop = false;
    Beat inStopBeat;
    auto st = incomingNoteOfStopLane();
    while (!isLastNoteOfStopLane(st) && t >= st->time && 
        t.hres() < st->time.hres() + std::get<double>(st->value) * beatLength.hres())
    {
        //_currentBeat = b->totalbeat - getCurrentMeasureBeat();
        if (!_currentStopBeatGuard)
        {
            _currentStopBeat += std::get<double>(st->value);
        }
        inStop = true;
        inStopBeat = st->totalbeat;
        ++st;
    }

    // Skip expired notes
    for (NoteLaneCategory cat = NoteLaneCategory::Note; (size_t)cat < (size_t)NoteLaneCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
    for (NoteLaneIndex idx = Sc1; idx < NOTECHANNEL_COUNT; ++*((size_t*)&idx))
    {
        auto it = incomingNoteOfLane(cat, idx);
        while (!isLastNoteOfLane(cat, idx, it) && t >= it->time && it->hit)
        {
            noteExpired.push_back(*it);
            it = nextNoteOfLane(cat, idx);
        }
    } 

    // Skip expired barline
    {
        NoteLaneCategory cat = NoteLaneCategory::EXTRA;
        NoteLaneIndex idx = (NoteLaneIndex)EXTRA_BARLINE;
        auto it = incomingNoteOfLane(cat, idx);
        while (!isLastNoteOfLane(cat, idx, it) && t >= it->time)
        {
            it->hit = true;
            it = nextNoteOfLane(cat, idx);
        }
    }

    // Skip expired plain note
    for (size_t idx = 0; idx < _commonNoteLists.size(); ++idx)
    {
        auto it = incomingNoteOfCommonLane(idx);
        while (!isLastNoteOfCommonLane(idx) && t >= it->time)
        {
            notePlainExpired.push_back(*it);
            it = nextNoteOfCommonLane(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _specialNoteLists.size(); ++idx)
    {
        auto it = incomingNoteOfSpecialLane(idx);
        while (!isLastNoteOfSpecialLane(idx) && t >= it->time)
        {
            noteExtExpired.push_back(*it);
            it = nextNoteOfSpecialLane(idx);
        }
    }

    // update current info
    if (!inStop)
    {
        Time currentMeasureTimePassed = t - _barTimestamp[_currentBar];
        Time timeFromBPMChange = currentMeasureTimePassed - _lastChangedBPMTime;
        gNumbers.set(eNumber::_TEST4, (int)currentMeasureTimePassed.norm());
        _currentBeat = _lastChangedBeat + (double)timeFromBPMChange.hres() / beatLength.hres() - _currentStopBeat;
        _currentStopBeatGuard = false;
    }
    else
    {
        _currentBeat = inStopBeat - _barBeatstamp[_currentBar];
        _currentStopBeatGuard = true;
    }

	gNumbers.set(eNumber::_TEST1, _currentBar);
	gNumbers.set(eNumber::_TEST2, (int)std::floor(_currentBeat * 1000));
	
}
