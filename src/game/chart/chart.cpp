#include "chart.h"
#include "chart_types.h"
#include "common/chartformat/chartformat.h"
#include "common/chartformat/chartformat_types.h"
#include "game/runtime/state.h"



using namespace chart;

ChartObjectBase::ChartObjectBase(int slot, size_t pn, size_t en) :
    _playerSlot(slot), _noteLists{}, _bgmNoteLists(pn), _specialNoteLists(en), _bgmNoteListIters(pn), _specialNoteListIters(en)
{
    reset();
	_barTimestamp.fill({LLONG_MAX, false});
    _barTimestamp[0] = 0;
    _bpmNoteList.clear();
    //_bpmList.push_back({ 0, {0, 1}, 0, 130 });
    //_stopList.push_back({ 0, {0, 1}, 0.0, 0, 1.0 });
}


std::shared_ptr<ChartObjectBase> ChartObjectBase::createFromChartFormat(int slot, std::shared_ptr<ChartFormatBase> p)
{
    switch (p->type())
    {
    case eChartFormat::BMS:
        try
        {
            return std::static_pointer_cast<ChartObjectBase>(std::make_shared<ChartObjectBMS>(slot, std::reinterpret_pointer_cast<ChartFormatBMS>(p)));
        }
        catch (std::exception& e)
        {
            LOG_ERROR << "[chart] Load chart exception (" << e.what() << "): " << p->filePath.u8string();
            return nullptr;
        }
    default:
        LOG_WARNING << "[chart] Chart type unknown (" << int(p->type()) << "): " << p->filePath.u8string();
        return nullptr;
    }
}

void ChartObjectBase::reset()
{
    _currentBarTemp         = 0;
    _currentMetreTemp        = 0;
    _currentBPM         = _bpmNoteList.empty()? 150 : BPM(_bpmNoteList.front().fvalue);
    _currentBeatLength    = Time::singleBeatLengthFromBPM(_currentBPM);
    _lastChangedBPMTime = 0;
    _lastChangedBPMMetre    = 0;

    // reset notes
    for (auto& ch : _noteLists)
        for (auto& n : ch)
            n.hit = false;

    // reset iterators
    resetNoteListsIterators();
}

void ChartObjectBase::resetNoteListsIterators()
{
    for (size_t i = 0; i < _noteLists.size(); ++i)         
        _noteListIterators[i]  = _noteLists[i].begin();
    for (size_t i = 0; i < _bgmNoteLists.size(); ++i)      
        _bgmNoteListIters[i] = _bgmNoteLists[i].begin();
    for (size_t i = 0; i < _specialNoteLists.size(); ++i)   
        _specialNoteListIters[i] = _specialNoteLists[i].begin();
    _bpmNoteListIter = _bpmNoteList.begin();
}

auto ChartObjectBase::firstNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteLists.front().begin())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].begin();
}

auto ChartObjectBase::incomingNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteLists.front().begin())
{
    size_t channel = channelToIdx(cat, idx);
    return _noteListIterators[channel];
}

auto ChartObjectBase::incomingNoteBgm(size_t channel) -> decltype(_bgmNoteLists.front().begin())
{
    return _bgmNoteListIters[channel];
}
auto ChartObjectBase::incomingNoteSpecial(size_t channel) -> decltype(_specialNoteLists.front().begin())
{
    return _specialNoteListIters[channel];
}
auto ChartObjectBase::incomingNoteBpm() -> decltype(_bpmNoteListIter)
{
    return _bpmNoteListIter;
}
bool ChartObjectBase::isLastNote(NoteLaneCategory cat, NoteLaneIndex idx, decltype(_noteListIterators.front()) it)
{
    size_t channel = channelToIdx(cat, idx);
    return _noteLists[channel].empty() || it == _noteLists[channel].end();
}
bool ChartObjectBase::isLastNoteBgm(size_t idx, decltype(_bgmNoteListIters.front()) it)
{
    return _bgmNoteLists[idx].empty() || it == _bgmNoteLists[idx].end();
}
bool ChartObjectBase::isLastNoteSpecial(size_t idx, decltype(_specialNoteListIters.front()) it)
{
    return _specialNoteLists[idx].empty() || it == _specialNoteLists[idx].end(); 
}
bool ChartObjectBase::isLastNoteBpm(decltype(_bpmNoteListIter) it)
{
	return _bpmNoteList.empty() || it == _bpmNoteList.end(); 
}

bool ChartObjectBase::isLastNote(NoteLaneCategory cat, NoteLaneIndex idx)
{
	return isLastNote(cat, idx, incomingNote(cat, idx));
}
bool ChartObjectBase::isLastNoteBgm(size_t channel)
{
	return isLastNoteBgm(channel, incomingNoteBgm(channel));
}
bool ChartObjectBase::isLastNoteSpecial(size_t channel)
{
	return isLastNoteSpecial(channel, incomingNoteSpecial(channel));
}
bool ChartObjectBase::isLastNoteBpm()
{
	return isLastNoteBpm(incomingNoteBpm());
}

auto ChartObjectBase::nextNote(NoteLaneCategory cat, NoteLaneIndex idx) -> decltype(_noteListIterators.front())
{
    size_t channel = channelToIdx(cat, idx);
    return ++_noteListIterators[channel];
}

auto ChartObjectBase::nextNoteBgm(size_t channel) -> decltype(_bgmNoteListIters.front())
{
    return ++_bgmNoteListIters[channel];
}
auto ChartObjectBase::nextNoteSpecial(size_t channel) -> decltype(_specialNoteListIters.front())
{
    return ++_specialNoteListIters[channel];
}
auto ChartObjectBase::nextNoteBpm() -> decltype(_bpmNoteListIter)
{
    return ++_bpmNoteListIter;
}

Time ChartObjectBase::getBarLength(size_t bar)
{
    if (bar + 1 >= _barTimestamp.size())
    {
        return -1;
    }

    auto l = _barTimestamp[bar + 1] - _barTimestamp[bar];
	return l.hres() > 0 ? l : -1;
}

Time ChartObjectBase::getCurrentBarLength()
{
    return getBarLength(_currentBarTemp);
}

Metre ChartObjectBase::getBarMetre(size_t bar)
{
    if (bar >= barMetreLength.size())
    {
		return { 1, 1 };
    }

	return barMetreLength[bar];
}

Metre ChartObjectBase::getCurrentBarMetre()
{
	return getBarMetre(_currentBarTemp);
}

Metre ChartObjectBase::getBarMetrePosition(size_t bar)
{
    if (bar >= _barMetrePos.size())
    {
		return Metre(LLONG_MAX, 1);
    }

	return _barMetrePos[bar];
}

void ChartObjectBase::update(const Time& rt)
{
    Time vt = rt + Time(State::get(IndexNumber::TIMING_ADJUST_VISUAL), false);
    Time at = rt;

    noteExpired.clear();
    noteBgmExpired.clear();
    noteSpecialExpired.clear();

    preUpdate(vt);

    // Go through expired measures
    while (_currentBarTemp + 1 < MAX_MEASURES && vt >= _barTimestamp[_currentBarTemp + 1])
    {
        ++_currentBarTemp;
        _currentMetreTemp = 0;
        _lastChangedBPMTime = 0;
        _lastChangedBPMMetre = 0;
    }

    // check inbounds BPM change
    auto b = incomingNoteBpm();
    while (!isLastNoteBpm(b) && vt >= b->time)
    {
        //_currentMetreTemp = b->pos - getCurrentMeasureBeat();
        _currentBPM = BPM(b->fvalue);
        _currentBeatLength = Time::singleBeatLengthFromBPM(_currentBPM);
        _lastChangedBPMTime = b->time - _barTimestamp[_currentBarTemp];
        _lastChangedBPMMetre = b->pos - _barMetrePos[_currentBarTemp];
        b = nextNoteBpm();
    }

    // Skip expired notes
    for (NoteLaneCategory cat = NoteLaneCategory::Note; (size_t)cat < (size_t)NoteLaneCategory::NOTECATEGORY_COUNT; ++*((size_t*)&cat))
        for (NoteLaneIndex idx = Sc1; idx < NOTELANEINDEX_COUNT; ++*((size_t*)&idx))
        {
            auto it = incomingNote(cat, idx);
            while (!isLastNote(cat, idx, it) && vt >= it->time && it->hit)
            {
                noteExpired.push_back(*it);
                it = nextNote(cat, idx);
            }
        } 

    // Skip expired barline
    {
        NoteLaneCategory cat = NoteLaneCategory::EXTRA;
        NoteLaneIndex idx = (NoteLaneIndex)EXTRA_BARLINE_1P;
        auto it = incomingNote(cat, idx);
        while (!isLastNote(cat, idx, it) && vt >= it->time)
        {
            it->hit = true;
            it = nextNote(cat, idx);
        }
        idx = (NoteLaneIndex)EXTRA_BARLINE_2P;
        it = incomingNote(cat, idx);
        while (!isLastNote(cat, idx, it) && vt >= it->time)
        {
            it->hit = true;
            it = nextNote(cat, idx);
        }
    }

    // Skip expired plain note
    for (size_t idx = 0; idx < _bgmNoteLists.size(); ++idx)
    {
        auto it = incomingNoteBgm(idx);
        while (!isLastNoteBgm(idx) && at >= it->time)
        {
            noteBgmExpired.push_back(*it);
            it = nextNoteBgm(idx);
        }
    } 
    // Skip expired extended note
    for (size_t idx = 0; idx < _specialNoteLists.size(); ++idx)
    {
        auto it = incomingNoteSpecial(idx);
        while (!isLastNoteSpecial(idx) && vt >= it->time)
        {
            noteSpecialExpired.push_back(*it);
            it = nextNoteSpecial(idx);
        }
    }

    // update beat
    Time currentMeasureTimePassed = vt - _barTimestamp[_currentBarTemp];
    Time timeFromBPMChange = currentMeasureTimePassed - _lastChangedBPMTime;
    State::set(IndexNumber::_TEST4, (int)currentMeasureTimePassed.norm());
    _currentMetreTemp = _lastChangedBPMMetre + (double)timeFromBPMChange.hres() / _currentBeatLength.hres() / 4;

    postUpdate(vt);

	State::set(IndexNumber::_TEST1, _currentBarTemp);
	State::set(IndexNumber::_TEST2, (int)std::floor(_currentMetreTemp * 1000));
	
    _currentBar = _currentBarTemp;
    _currentMetre = _currentMetreTemp;
}
