#include "chart.h"
#include "common/chartformat/chartformat.h"
#include "game/data/number.h"

#include "common/chartformat/chartformat_types.h"

#include "chart_types.h"

using namespace chart;

vChart::vChart(int slot, size_t pn, size_t en) :
    _playerSlot(slot), _noteLists{}, _bgmNoteLists(pn), _specialNoteLists(en), _bgmNoteListIters(pn), _specialNoteListIters(en)
{
    reset();
	_barTimestamp.fill({LLONG_MAX, false});
    _barTimestamp[0] = 0;
    _bpmNoteList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    //_stopList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}


std::shared_ptr<vChart> vChart::createFromChartFormat(int slot, std::shared_ptr<vChartFormat> p)
{
    switch (p->type())
    {
    case eChartFormat::BMS:
        return std::make_shared<chartBMS>(slot, std::reinterpret_pointer_cast<BMS>(p));
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

auto vChart::firstNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteLists.front().begin())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].begin();
}

auto vChart::incomingNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto vChart::incomingNoteBgm(size_t channel) -> decltype(_bgmNoteListIters.front())
{
    return _bgmNoteListIters[channel];
}
auto vChart::incomingNoteSpecial(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return _specialNoteListIters[channel];
}
auto vChart::incomingNoteBpm() -> decltype(_bpmNoteListIter)
{
    return _bpmNoteListIter;
}
bool vChart::isLastNote(NoteLaneCategory cat, NoteLaneIndex idx, decltype(_noteListIterators.front()) it)
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || it == _noteLists[channel].end();
}
bool vChart::isLastNoteBgm(size_t idx, decltype(_bgmNoteListIters.front()) it)
{
    return _bgmNoteLists[idx].empty() || it == _bgmNoteLists[idx].end();
}
bool vChart::isLastNoteSpecial(size_t idx, decltype(_specialNoteListIters.front()) it)
{
    return _specialNoteLists[idx].empty() || it == _specialNoteLists[idx].end(); 
}
bool vChart::isLastNoteBpm(decltype(_bpmNoteListIter) it)
{
	return _bpmNoteList.empty() || it == _bpmNoteList.end(); 
}

bool vChart::isLastNote(NoteLaneCategory cat, NoteLaneIndex idx)
{
	return isLastNote(cat, idx, incomingNote(cat, idx));
}
bool vChart::isLastNoteBgm(size_t channel)
{
	return isLastNoteBgm(channel, incomingNoteBgm(channel));
}
bool vChart::isLastNoteSpecial(size_t channel)
{
	return isLastNoteSpecial(channel, incomingNoteSpecial(channel));
}
bool vChart::isLastNoteBpm()
{
	return isLastNoteBpm(incomingNoteBpm());
}

auto vChart::nextNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return ++_noteListIterators[channel];
}

auto vChart::nextNoteBgm(size_t channel) -> decltype(_bgmNoteListIters.front())
{
    return ++_bgmNoteListIters[channel];
}
auto vChart::nextNoteSpecial(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return ++_specialNoteListIters[channel];
}
auto vChart::nextNoteBpm() -> decltype(_bpmNoteListIter)
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
    auto b = incomingNoteBpm();
    while (!isLastNoteBpm(b) && t >= b->time)
    {
        //_currentBeat = b->totalbeat - getCurrentMeasureBeat();
        _currentBPM = std::get<BPM>(b->value);
		beatLength = Time::singleBeatLengthFromBPM(_currentBPM);
        _lastChangedBPMTime = b->time - _barTimestamp[_currentBar];
        _lastChangedBeat = b->totalbeat - _barBeatstamp[_currentBar];
        b = nextNoteBpm();
    }

    // Skip expired notes
    for (NoteLaneCategory cat = NoteLaneCategory::Note; (size_t)cat < (size_t)NoteLaneCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
        for (NoteLaneIndex idx = Sc1; idx < NOTELANEINDEX_COUNT; ++*((size_t*)&idx))
        {
            auto it = incomingNote(cat, idx);
            while (!isLastNote(cat, idx, it) && t >= it->time && it->hit)
            {
                noteExpired.push_back(*it);
                it = nextNote(cat, idx);
            }
        } 

    // Skip expired barline
    {
        NoteLaneCategory cat = NoteLaneCategory::EXTRA;
        NoteLaneIndex idx = (NoteLaneIndex)EXTRA_BARLINE;
        auto it = incomingNote(cat, idx);
        while (!isLastNote(cat, idx, it) && t >= it->time)
        {
            it->hit = true;
            it = nextNote(cat, idx);
        }
    }

    // Skip expired plain note
    for (size_t idx = 0; idx < _bgmNoteLists.size(); ++idx)
    {
        auto it = incomingNoteBgm(idx);
        while (!isLastNoteBgm(idx) && t >= it->time)
        {
            noteBgmExpired.push_back(*it);
            it = nextNoteBgm(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _specialNoteLists.size(); ++idx)
    {
        auto it = incomingNoteSpecial(idx);
        while (!isLastNoteSpecial(idx) && t >= it->time)
        {
            noteSpecialExpired.push_back(*it);
            it = nextNoteSpecial(idx);
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
