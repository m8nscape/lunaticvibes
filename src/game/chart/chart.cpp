#include "chart.h"
#include "common/chartformat/chartformat.h"
#include "game/data/number.h"

#include "common/chartformat/chartformat_types.h"

#include "chart_types.h"

vChart::vChart( size_t pn, size_t en) :
    _noteLists{}, _bgmNoteLists(pn), _specialNoteLists(en), _bgmNoteListIters(pn), _specialNoteListIters(en)
{
    reset();
	_barTimestamp.fill({LLONG_MAX, false});
    _barTimestamp[0] = 0;
    _bpmNoteList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    //_stopList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}


std::shared_ptr<vChart> vChart::createFromChartFormat(std::shared_ptr<vChartFormat> p)
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
    _currentBar         = 0;
    _currentBeat        = 0;
    _currentBPM         = _bpmNoteList.empty()? 150 : std::get<BPM>(_bpmNoteList.front().value);
    _lastChangedBPMTime = 0;
    _lastChangedBeat    = 0;

    // reset notes
    for (auto& ch : _noteLists)
        for (auto& n : ch)
            n.hit = false;

    // reset iterators
    resetNoteListsIterators();
}

void vChart::resetNoteListsIterators()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)         
        _noteListIterators[i]  = _noteLists[i].begin();
    for (size_t i = 0; i < _bgmNoteLists.size(); ++i)      
        _bgmNoteListIters[i] = _bgmNoteLists[i].begin();
    for (size_t i = 0; i < _specialNoteLists.size(); ++i)   
        _specialNoteListIters[i] = _specialNoteLists[i].begin();
    _bpmNoteListIter = _bpmNoteList.begin();
}

auto vChart::firstNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteLists.front().begin())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].begin();
}

auto vChart::incomingNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto vChart::incomingNoteOfBgmLane(size_t channel) -> decltype(_bgmNoteListIters.front())
{
    return _bgmNoteListIters[channel];
}
auto vChart::incomingNoteOfSpecialLane(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return _specialNoteListIters[channel];
}
auto vChart::incomingNoteOfBpmLane() -> decltype(_bpmNoteListIter)
{
    return _bpmNoteListIter;
}
bool vChart::isLastNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx, decltype(_noteListIterators.front()) it)
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || it == _noteLists[channel].end();
}
bool vChart::isLastNoteOfBgmLane(size_t idx, decltype(_bgmNoteListIters.front()) it)
{
    return _bgmNoteLists[idx].empty() || it == _bgmNoteLists[idx].end();
}
bool vChart::isLastNoteOfSpecialLane(size_t idx, decltype(_specialNoteListIters.front()) it)
{
    return _specialNoteLists[idx].empty() || it == _specialNoteLists[idx].end(); 
}
bool vChart::isLastNoteOfBpmLane(decltype(_bpmNoteListIter) it)
{
	return _bpmNoteList.empty() || it == _bpmNoteList.end(); 
}

bool vChart::isLastNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx)
{
	return isLastNoteOfLane(cat, idx, incomingNoteOfLane(cat, idx));
}
bool vChart::isLastNoteOfBgmLane(size_t channel)
{
	return isLastNoteOfBgmLane(channel, incomingNoteOfBgmLane(channel));
}
bool vChart::isLastNoteOfSpecialLane(size_t channel)
{
	return isLastNoteOfSpecialLane(channel, incomingNoteOfSpecialLane(channel));
}
bool vChart::isLastNoteOfBpmLane()
{
	return isLastNoteOfBpmLane(incomingNoteOfBpmLane());
}

auto vChart::nextNoteOfLane(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return ++_noteListIterators[channel];
}

auto vChart::nextNoteOfBgmLane(size_t channel) -> decltype(_bgmNoteListIters.front())
{
    return ++_bgmNoteListIters[channel];
}
auto vChart::nextNoteOfSpecialLane(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return ++_specialNoteListIters[channel];
}
auto vChart::nextNoteOfBpmLane() -> decltype(_bpmNoteListIter)
{
    return ++_bpmNoteListIter;
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
    noteBgmExpired.clear();
    noteSpecialExpired.clear();

    preUpdate(t);

	Time beatLength = Time::singleBeatLengthFromBPM(_currentBPM);

    // Go through expired measures
    while (_currentBar + 1 < MAX_MEASURES && t >= _barTimestamp[_currentBar + 1])
    {
        ++_currentBar;
        _currentBeat = 0;
        _lastChangedBPMTime = 0;
        _lastChangedBeat = 0;
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

    // Skip expired notes
    for (NoteLaneCategory cat = NoteLaneCategory::Note; (size_t)cat < (size_t)NoteLaneCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
        for (NoteLaneIndex idx = Sc1; idx < NOTELANEINDEX_COUNT; ++*((size_t*)&idx))
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
    for (size_t idx = 0; idx < _bgmNoteLists.size(); ++idx)
    {
        auto it = incomingNoteOfBgmLane(idx);
        while (!isLastNoteOfBgmLane(idx) && t >= it->time)
        {
            noteBgmExpired.push_back(*it);
            it = nextNoteOfBgmLane(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _specialNoteLists.size(); ++idx)
    {
        auto it = incomingNoteOfSpecialLane(idx);
        while (!isLastNoteOfSpecialLane(idx) && t >= it->time)
        {
            noteSpecialExpired.push_back(*it);
            it = nextNoteOfSpecialLane(idx);
        }
    }

    // update beat
    Time currentMeasureTimePassed = t - _barTimestamp[_currentBar];
    Time timeFromBPMChange = currentMeasureTimePassed - _lastChangedBPMTime;
    gNumbers.set(eNumber::_TEST4, (int)currentMeasureTimePassed.norm());
    _currentBeat = _lastChangedBeat + (double)timeFromBPMChange.hres() / beatLength.hres();

    postUpdate(t);

	gNumbers.set(eNumber::_TEST1, _currentBar);
	gNumbers.set(eNumber::_TEST2, (int)std::floor(_currentBeat * 1000));
	
}
