#include "chart_bms.h"
#include "game/scene/scene_context.h"
#include <random>

using namespace chart;

const size_t NOPE = -1;

// Need this since current parser stores channel directly
// TODO mapping is different between file formats
const size_t BMSToLaneMap[] = 
{
    // Normal notes #xxx0x
    Sc1,
    K1,
    K2,
    K3,
    K4,
    K5,
    K6,
    K7,
    NOPE,
    NOPE,

    // #xxx1x
    Sc2,
    K8,
    K9,
    K10,
    K11,
    K12,
    K13,
    K14,
    NOPE,
    NOPE,

    // LN Head #xxx2x
    chart::channelToIdx(NoteLaneCategory::LN, Sc1),
    chart::channelToIdx(NoteLaneCategory::LN, K1),
    chart::channelToIdx(NoteLaneCategory::LN, K2),
    chart::channelToIdx(NoteLaneCategory::LN, K3),
    chart::channelToIdx(NoteLaneCategory::LN, K4),
    chart::channelToIdx(NoteLaneCategory::LN, K5),
    chart::channelToIdx(NoteLaneCategory::LN, K6),
    chart::channelToIdx(NoteLaneCategory::LN, K7),
    NOPE,
    NOPE,

    // #xxx3x
    chart::channelToIdx(NoteLaneCategory::LN, Sc2),
    chart::channelToIdx(NoteLaneCategory::LN, K8),
    chart::channelToIdx(NoteLaneCategory::LN, K9),
    chart::channelToIdx(NoteLaneCategory::LN, K10),
    chart::channelToIdx(NoteLaneCategory::LN, K11),
    chart::channelToIdx(NoteLaneCategory::LN, K12),
    chart::channelToIdx(NoteLaneCategory::LN, K13),
    chart::channelToIdx(NoteLaneCategory::LN, K14),
    NOPE,
    NOPE,

    // LN Tail #xxx4x
    chart::channelToIdx(NoteLaneCategory::LN, Sc1),
    chart::channelToIdx(NoteLaneCategory::LN, K1),
    chart::channelToIdx(NoteLaneCategory::LN, K2),
    chart::channelToIdx(NoteLaneCategory::LN, K3),
    chart::channelToIdx(NoteLaneCategory::LN, K4),
    chart::channelToIdx(NoteLaneCategory::LN, K5),
    chart::channelToIdx(NoteLaneCategory::LN, K6),
    chart::channelToIdx(NoteLaneCategory::LN, K7),
    NOPE,
    NOPE,

    // #xxx5x
    chart::channelToIdx(NoteLaneCategory::LN, Sc2),
    chart::channelToIdx(NoteLaneCategory::LN, K8),
    chart::channelToIdx(NoteLaneCategory::LN, K9),
    chart::channelToIdx(NoteLaneCategory::LN, K10),
    chart::channelToIdx(NoteLaneCategory::LN, K11),
    chart::channelToIdx(NoteLaneCategory::LN, K12),
    chart::channelToIdx(NoteLaneCategory::LN, K13),
    chart::channelToIdx(NoteLaneCategory::LN, K14),
    NOPE,
    NOPE,
};
    
// from NoteLaneIndex to Input::Pad
const std::vector<Input::Pad> LaneToKeyMap[] = 
{
    // 0: Notes
    {Input::Pad::S1L, Input::Pad::S1R}, // Sc1
    {Input::Pad::K11},
    {Input::Pad::K12},
    {Input::Pad::K13},
    {Input::Pad::K14},
    {Input::Pad::K15},
    {Input::Pad::K16},
    {Input::Pad::K17},
    {Input::Pad::K21}, //8
    {Input::Pad::K22}, //9
    {Input::Pad::K23}, //10
    {Input::Pad::K24}, //11
    {Input::Pad::K25}, //12
    {Input::Pad::K26}, //13
    {Input::Pad::K27}, //14
    {},{},{},{},{},{},{},{},{},{}, //15~24
    {Input::Pad::S2L, Input::Pad::S2R} // Sc2
};

// from Input::Pad to NoteLaneIndex
const NoteLaneIndex KeyToLaneMap[] = 
{
    Sc1, Sc1,
    K1, K2, K3, K4, K5, K6, K7,
    _, _, _, _, _, _,

    Sc2, Sc2,
    K8, K9, K10, K11, K12, K13, K14,
    _, _, _, _, _, _,
};

chartBMS::chartBMS(int slot) : vChart(slot, BGM_LANE_COUNT, (size_t)eNoteExt::EXT_COUNT)
{
}

chartBMS::chartBMS(int slot, std::shared_ptr<BMS> b) : chartBMS(slot)
{
    loadBMS(*b);
}
chartBMS::chartBMS(int slot, const BMS& b) : chartBMS(slot)
{
    loadBMS(b);
}

void chartBMS::loadBMS(const BMS& objBms)
{
	_noteCount = objBms.notes;
	_noteCount_regular = objBms.notes - objBms.notes_ln;
	_noteCount_ln = objBms.notes_ln;
	Time basetime{ 0 };
	Beat basebeat{ 0, 1 };

    BPM bpm = objBms.startBPM;
    _currentBPM = bpm;
    _bpmNoteList.push_back({ 0, {0, 1}, 0, bpm });
	barLength.fill({ 1, 1 });
    bool bpmfucked = false; // set to true when BPM is changed to zero or negative value
    std::bitset<10> isLnTail[2]{ 0 };

    int laneCountRandom = 7;
    switch (gPlayContext.mode)
    {
    case eMode::PLAY5:
    case eMode::PLAY5_2:
    case eMode::PLAY10: laneCountRandom = 5; break;
    case eMode::PLAY7:
    case eMode::PLAY7_2:
    case eMode::PLAY14: laneCountRandom = 7; break;
    case eMode::PLAY9:
    case eMode::PLAY9_2: laneCountRandom = 9; break;
    default: break;
    }

    std::array<NoteLaneIndex, NOTELANEINDEX_COUNT> gameLaneMap;
    for (size_t i = Sc1; i < NOTELANEINDEX_COUNT; ++i) 
        gameLaneMap[i] = (NoteLaneIndex)i;

    std::mt19937 rng(gPlayContext.randomSeedMod);
    switch (gPlayContext.mods[_playerSlot].chart)
    {
    case eModChart::RANDOM:
        std::shuffle(gameLaneMap.begin() + 1, gameLaneMap.begin() + 1 + laneCountRandom, rng);
        break;

    case eModChart::MIRROR:
        std::reverse(gameLaneMap.begin() + 1, gameLaneMap.begin() + 1 + laneCountRandom);
        break;

    case eModChart::SRAN:
    case eModChart::HRAN:
    case eModChart::ALLSCR:
        // handled below
        break;

    case eModChart::NONE:
    default: 
        break;
    }

    std::bitset<NOTELANEINDEX_COUNT> laneOccupiedByLN{ 0 };

    for (unsigned m = 0; m <= objBms.lastBarIdx; m++)
    {
		barLength[m] = objBms.barLength[m];
		_barBeatstamp[m] = basebeat;
        _barTimestamp[m] = basetime;

        // notes [] {beat, {lane, sample/val}}
        std::vector<std::pair<Segment, std::pair<unsigned, unsigned>>> notes;

        // In case the channels from the file are shuffled, store the data into buffer and sort it out first
        // The following patterns must be arranged to keep process order by [Notes > BPM > Stop]
        {
            // Lookup BMSToLaneMap[]
            // Visible Notes, bms: 1x/2x
            for (unsigned i = 0; i < 10; i++)
            {
                auto ch = objBms.getLane(LaneCode::NOTE1, i, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ i, n.value } });
            }
            for (unsigned i = 0; i < 10; i++)
            {
                auto ch = objBms.getLane(LaneCode::NOTE2, i, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 10 + i, n.value } });
            }

            // LN: , bms: 5x/6x
            for (unsigned i = 0; i < 10; i++)
            {
                auto ch = objBms.getLane(LaneCode::NOTELN1, i, m);
                for (const auto& n : ch.notes)
                {
                    int lane = isLnTail[0][i] ? 40 + i : 20 + i;
                    notes.push_back({ fraction(n.segment, ch.resolution),{ lane, n.value } });
                    isLnTail[0][i].flip();
                }
            }
            for (unsigned i = 0; i < 10; i++)
            {
                auto ch = objBms.getLane(LaneCode::NOTELN2, i, m);
                for (const auto& n : ch.notes)
                {
                    int lane = isLnTail[1][i] ? 50 + i : 30 + i;
                    notes.push_back({ fraction(n.segment, ch.resolution),{ lane, n.value } });
                    isLnTail[1][i].flip();
                }
            }

            // invisible: , bms: 3x/4x
            // mine: , bms: Dx/Ex, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover

            // BGM: 
            for (unsigned i = 0; i < objBms.bgmLayersCount[m]; i++)
            {
                auto ch = objBms.getLane(LaneCode::BGM, i, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 100 + i, n.value } });
            }

            // BGA: 0xF0 / 0xF1 / 0xF2
            if (/* BGA switch */ false)
            {
                {
                    auto ch = objBms.getLane(LaneCode::BGABASE, 0, m);
                    for (const auto& n : ch.notes)
                        notes.push_back({ fraction(n.segment, ch.resolution),{ 0xF0, n.value } });
                }
                {
                    auto ch = objBms.getLane(LaneCode::BGALAYER, 0, m);
                    for (const auto& n : ch.notes)
                        notes.push_back({ fraction(n.segment, ch.resolution),{ 0xF1, n.value } });
                }
                {
                    auto ch = objBms.getLane(LaneCode::BGAPOOR, 0, m);
                    for (const auto& n : ch.notes)
                        notes.push_back({ fraction(n.segment, ch.resolution),{ 0xF2, n.value } });
                }
            }

            // BPM Change: 0xFD
            {
                auto ch = objBms.getLane(LaneCode::BPM, 0, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 0xFD, n.value } });
            }

            // EX BPM: 0xFE
            {
                auto ch = objBms.getLane(LaneCode::EXBPM, 0, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 0xFE, n.value } });
            }

            // Stop: 0xFF
            {
                auto ch = objBms.getLane(LaneCode::STOP, 0, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 0xFF, n.value } });
            }
        }

        // Sort by time / lane value
        std::stable_sort(notes.begin(), notes.end());

        ///////////////////////////////////////////////////////////////////////

        // Calculate note times and push to note list
        Segment lastBPMChangedSegment(0, 1);
        double stopBeat = 0;
        double currentSpd = 1.0;
        Beat measureLength = objBms.barLength[m];      // visual beat
		Time beatLength = Time::singleBeatLengthFromBPM(bpm);

        std::array<NoteLaneIndex, NOTELANEINDEX_COUNT> gameLaneLNIndex;
        gameLaneLNIndex.fill(_);

        for (const auto& note : notes)
        {
            auto[noteSegment, noteinfo] = note;
            auto[lane, val] = noteinfo;
            double beatFromBPMChange = (noteSegment - lastBPMChangedSegment) * measureLength;
            Beat beat = basebeat + noteSegment * measureLength;
			Time notetime = bpmfucked ? LLONG_MAX : basetime + beatLength * beatFromBPMChange;

            if (lane >= 0 && lane < 100)
            {
				// TODO mapping is different between file formats (bms, bme, etc)
                if (BMSToLaneMap[lane] == NOPE) continue;
                size_t chartLaneIdx = BMSToLaneMap[lane];
                size_t gameLaneIdx = gameLaneMap[chartLaneIdx];

                if (lane >= 40 && lane < 60)
                {
                    // LN tail
                    size_t lnLaneIdx = gameLaneLNIndex[gameLaneIdx];
                    assert(lnLaneIdx != _);
                    assert(laneOccupiedByLN[lnLaneIdx] == true);
                    _noteLists[lnLaneIdx].push_back({ m, beat, notetime, (long long)val, Note::LN_TAIL, false });
                    laneOccupiedByLN[lnLaneIdx] = false;
                }
                else
                {
                    // normal, LN head
                    size_t gameLaneIdxMod = gameLaneIdx;
                    size_t laneMin, laneMax;
                    int laneSlot;
                    if (gameLaneIdx >= Sc1 && gameLaneIdx <= Sc1 + laneCountRandom)
                    {
                        laneMin = K1;                   // K1
                        laneMax = laneMin + (laneCountRandom - 1); // K7
                        laneSlot = PLAYER_SLOT_1P;
                    }
                    else if (gameLaneIdx >= Sc1 + laneCountRandom + 1 && gameLaneIdx <= Sc1 + laneCountRandom + 1 + laneCountRandom - 1)
                    {
                        laneMin = K1 + laneCountRandom;            // K8
                        laneMax = laneMin + (laneCountRandom - 1); // K14
                        laneSlot = PLAYER_SLOT_2P;
                    }
                    else
                    {
                        assert(false);
                        break;
                    }

                    switch (gPlayContext.mods[_playerSlot].chart)
                    {
                    case eModChart::SRAN:
                        if (gameLaneIdx != Sc1 && gameLaneIdx == Sc2)
                        {
                            constexpr int threshold_ms = 50;
                            std::vector<NoteLaneIndex> placable;
                            for (NoteLaneIndex i = NoteLaneIndex(laneMin); i != NoteLaneIndex(laneMax + 1); ++*(size_t*)i)
                            {
                                if (_noteLists[i].empty())
                                {
                                    placable.push_back(i);
                                    continue;
                                }
                                else
                                {
                                    auto& lastNote = --_noteLists[i].end();
                                    if (notetime - lastNote->time < threshold_ms && gameLaneLNIndex[i] == _)
                                        placable.push_back(i);
                                }
                            }
                            gameLaneIdxMod = placable.empty() ? (K1 + rng() % laneCountRandom) : placable[rng() % placable.size()];
                        }
                        break;

                    case eModChart::HRAN:
                        if (gameLaneIdx != Sc1 && gameLaneIdx == Sc2)
                        {
                            constexpr int threshold_ms = 250;
                            std::vector<NoteLaneIndex> placable;
                            for (NoteLaneIndex i = NoteLaneIndex(laneMin); i != NoteLaneIndex(laneMax + 1); ++ * (size_t*)i)
                            {
                                if (_noteLists[i].empty())
                                {
                                    placable.push_back(i);
                                    continue;
                                }
                                else
                                {
                                    auto& lastNote = --_noteLists[i].end();
                                    if (notetime - lastNote->time < threshold_ms && gameLaneLNIndex[i] == _)
                                        placable.push_back(i);
                                }
                            }
                            if (!placable.empty())
                                gameLaneIdxMod = placable[rng() % placable.size()];
                            else
                            {
                                using noteTimePair = std::pair<long long, NoteLaneIndex>;
                                std::vector<noteTimePair> placableMin;
                                for (NoteLaneIndex i = NoteLaneIndex(laneMin); i != NoteLaneIndex(laneMax + 1); ++ * (size_t*)i)
                                {
                                    // no need to check if noteLists[i] is empty
                                    auto& lastNote = --_noteLists[i].end();
                                    placableMin.push_back({lastNote->time.hres(), i});
                                }
                                assert(!placableMin.empty());
                                std::sort(placableMin.begin(), placableMin.end());
                                gameLaneIdxMod = placableMin.begin()->second;
                            }
                        }
                        break;

                    case eModChart::ALLSCR:
                    {
                        constexpr int threshold_ms = 250;
                        size_t laneMin, laneMax, laneScratch;
                        int laneStep;
                        if (laneSlot == PLAYER_SLOT_1P)
                        {
                            laneStep = 1;
                            laneScratch = Sc1;
                        }
                        else
                        {
                            laneStep = -1;
                            std::swap(laneMin, laneMax);
                            laneScratch = Sc2;
                        }

                        // FIXME 2p side SP all-scr should reverse the lanes

                        if (_noteLists[laneScratch].empty())
                            gameLaneIdxMod = laneScratch;
                        else
                        {
                            auto& lastScratch = --_noteLists[laneScratch].end();
                            if (notetime - lastScratch->time < threshold_ms && !laneOccupiedByLN[laneScratch])
                                gameLaneIdxMod = laneScratch;
                            else
                            {
                                bool availableLaneFound = false;
                                for (NoteLaneIndex i = (NoteLaneIndex)laneMin; i != (NoteLaneIndex)laneMax + laneStep; *(size_t*)&i += laneStep)
                                {
                                    auto& lastNote = --_noteLists[laneScratch].end();
                                    if (notetime - lastNote->time < threshold_ms && !laneOccupiedByLN[i])
                                    {
                                        gameLaneIdxMod = i;
                                        availableLaneFound = true;
                                        break;
                                    }
                                }
                                if (!availableLaneFound)
                                    gameLaneIdxMod = laneMin + rng() % (std::abs(int(laneMax - laneMin)) + 1);
                            }
                        }
                        break;
                    }

                    default:
                        break;
                    }

                    _noteLists[gameLaneIdxMod].push_back({ m, beat, notetime, (long long)val });

                    if (lane >= 20 && lane < 40)
                        gameLaneLNIndex[gameLaneIdx] = (NoteLaneIndex)gameLaneIdxMod;
                }
            }
            else if (lane >= 100 && lane <= 131)
            {
                _bgmNoteLists[lane - 100].push_back({ m, beat, notetime, (long long)val, 0 });
            }
            else if (!bpmfucked) switch (lane)
            {
            case 0xE0:  // BGA base
				_specialNoteLists[(size_t)eNoteExt::BGABASE].push_back({ m, beat, notetime, (long long)val });
				break;
            case 0xE1:  // BGA layer
                _specialNoteLists[(size_t)eNoteExt::BGALAYER].push_back({ m, beat, notetime, (long long)val });
				break;
            case 0xE2:  // BGA poor
                _specialNoteLists[(size_t)eNoteExt::BGAPOOR].push_back({ m, beat, notetime, (long long)val });
				break;

            case 0xFD:	// BPM Change
                if (bpm == static_cast<BPM>(val)) break;
                basetime = notetime;
                lastBPMChangedSegment = noteSegment;
                bpm = static_cast<BPM>(val);
				beatLength = Time::singleBeatLengthFromBPM(bpm);
                _bpmNoteList.push_back({ m, beat, notetime, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case 0xFE:	// ExBPM Change
                if (bpm == objBms.exBPM[val]) break;
                basetime = notetime;
                lastBPMChangedSegment = noteSegment;
                bpm = objBms.exBPM[val];
                beatLength = Time::singleBeatLengthFromBPM(bpm);
                _bpmNoteList.push_back({ m, beat, notetime, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case 0xFF:	// Stop
                double curStopBeat = objBms.stop[val] / 192.0;
				Time  curStopTime{ (long long)std::floor(Time::singleBeatLengthFromBPM(bpm).hres() * curStopBeat), true };
                if (curStopBeat <= 0) break;
				_specialNoteLists[(size_t)eNoteExt::STOP].push_back({ m, beat, notetime, curStopBeat });
                //_chartingSpeedList.push_back({ m, beat, noteht, 0.0 });
                //_chartingSpeedList.push_back({ m, beat + d2fr(curStopBeat), noteht + curStopTime, currentSpd });
                stopBeat += curStopBeat;
                basetime += curStopTime;
                break;
            }
        }
		basetime += beatLength * (1.0 - lastBPMChangedSegment) * measureLength;
		basebeat += measureLength;

        // add barline for next measure
        _noteLists[chart::channelToIdx(NoteLaneCategory::EXTRA, EXTRA_BARLINE)].push_back(
			{ m + 1, basebeat, basetime, long long(0), false });

    }

    _totalLength = basetime;    // last measure + 1

    resetNoteListsIterators();
}


NoteLane chartBMS::getLaneFromKey(Input::Pad input)
{
    if (input >= Input::S1L && input < Input::ESC && KeyToLaneMap[input] != _)
    {
        using cat = NoteLaneCategory;
        NoteLaneIndex idx = KeyToLaneMap[input];
        std::vector<std::pair<cat, HitableNote>> note;
        if (!isLastNote(cat::Note, idx))
            note.push_back({ cat::Note, *incomingNote(cat::Note, idx) });
        if (!isLastNote(cat::Invs, idx))
            note.push_back({ cat::Invs, *incomingNote(cat::Invs, idx) });
        if (!isLastNote(cat::LN, idx))
            note.push_back({ cat::LN,   *incomingNote(cat::LN, idx) });
        std::sort(note.begin(), note.end(), [](decltype(note.front())& a, decltype(note.front())& b) { return b.second.time > a.second.time; });
        for (size_t i = 0; i < note.size(); ++i)
            if (!note[i].second.hit) return { note[i].first, idx };
    }
    return { NoteLaneCategory::_, _ };
}

std::vector<Input::Pad> chartBMS::getInputFromLane(size_t channel)
{
    if (channel >= LaneToKeyMap->size())
        return {};
    else
        return LaneToKeyMap[channel];
}

void chartBMS::preUpdate(const Time& t)
{
    if (_currentBar + 1 < MAX_MEASURES && t >= _barTimestamp[_currentBar + 1])
    {
        _currentStopBeat = 0;
        _currentStopBeatGuard = false;
    }
}

void chartBMS::postUpdate(const Time& t)
{
    Time beatLength = Time::singleBeatLengthFromBPM(_currentBPM);

    // check stop
    bool inStop = false;
    Beat inStopBeat;
    size_t idx = (size_t)eNoteExt::STOP;
    auto st = incomingNoteSpecial(idx);
    while (!isLastNoteSpecial(idx, st) && t >= st->time &&
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


    // update current info
    if (!inStop)
    {
        _currentStopBeatGuard = false;
        _currentBeat -= _currentStopBeat;
    }
    else
    {
        _currentStopBeatGuard = true;
        _currentBeat = inStopBeat - _barBeatstamp[_currentBar];
    }

}