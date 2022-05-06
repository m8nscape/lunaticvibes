#include "chart_bms.h"
#include "game/scene/scene_context.h"
#include <random>
#include "game/data/switch.h"

using namespace chart;

const size_t NOPE = -1;

// channel misorder (#xxx08, #xxx09) is already handled in chartformat object, do not convert here
// 0 is Scratch, 1-9 are keys, matching by order
const std::vector<size_t> BMSToLaneMap = 
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
	Metre basebeat{ 0, 1 };

    BPM bpm = objBms.startBPM;
    _currentBPM = bpm;
    _bpmNoteList.push_back({ 0, {0, 1}, 0, bpm });
	barLength.fill({ 1, 1 });
    bool bpmfucked = false; // set to true when BPM is changed to zero or negative value
    std::bitset<NOTELANEINDEX_COUNT> isLnTail{ 0 };

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

    std::array<NoteLaneIndex, NOTELANEINDEX_COUNT> gameLaneLNIndex;
    gameLaneLNIndex.fill(_);

    for (unsigned m = 0; m <= objBms.lastBarIdx; m++)
    {
		barLength[m] = objBms.metres[m];
		_barMetrePos[m] = basebeat;
        _barTimestamp[m] = basetime;


        // In case the channels from the file are shuffled, store the data into buffer and sort it out first
        // The following patterns must be arranged to keep process order by [Notes > BPM > Stop]
        enum class eLanePriority: unsigned
        {
            // Notes
            NOTE,
            LNHEAD,
            LNTAIL,
            INV,
            MINE,

            BGM,
            BGABASE,
            BGALAYER,
            BGAPOOR,

            // BPM
            BPM,
            EXBPM,

            // Stop
            STOP,
        };

        struct Lane
        {
            eLanePriority type;
            unsigned index;
            bool operator< (const Lane& rhs) const { return std::make_pair(type, index) < std::make_pair(rhs.type, rhs.index); }
        };

        // notes [] {beat, {lane, sample/val}}
        std::vector<std::pair<Segment, std::pair<Lane, unsigned>>> notes;

        // add notes
        {
            auto push_notes = [&objBms, m](decltype(notes)& notes, eLanePriority priority, LaneCode code, int area)
            {
                for (unsigned i = 0; i < 10; i++)
                {
                    auto ch = objBms.getLane(code, i, m);
                    unsigned index = i;
                    if (area != 0 && objBms.player != 1)
                    {
                        if (index == Sc1)
                        {
                            assert(area == 1);
                            index = Sc2;
                        }
                        else
                        {
                            switch (objBms.gamemode)
                            {
                            case 10:
                                index += 5 * area;
                                break;

                            case 14:
                                index += 7 * area;
                                break;

                            default:
                                assert(false);  // other gamemodes does not support notes on 2P side
                                break;
                            }
                        }
                    }
                    for (const auto& n : ch.notes)
                    {
                        //              { beat,                               { { lane,              val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { priority, index }, n.value } });
                    }
                }
            };
            auto push_notes_ln = [&objBms, m, &isLnTail](decltype(notes)& notes, LaneCode code, int area)
            {
                for (unsigned i = 0; i < 10; i++)
                {
                    auto ch = objBms.getLane(code, i, m);
                    unsigned index = i;
                    if (area != 0 && objBms.player != 1)
                    {
                        if (index == Sc1)
                        {
                            assert(area == 1);
                            index = Sc2;
                        }
                        else
                        {
                            switch (objBms.gamemode)
                            {
                            case 10:
                                index += 5 * area;
                                break;

                            case 14:
                                index += 7 * area;
                                break;

                            default:
                                assert(false);  // other gamemodes does not support notes on 2P side
                                break;
                            }
                        }
                    }
                    for (const auto& n : ch.notes)
                    {
                        eLanePriority priority = isLnTail[index] ? eLanePriority::LNTAIL : eLanePriority::LNHEAD;
                        notes.push_back({ fraction(n.segment, ch.resolution),{ { priority, index }, n.value } });
                        isLnTail[index].flip();
                    }
                }
            };

            // Lookup BMSToLaneMap[]
            // Regular Notes
            push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE1, 0);
            push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE2, 1);

            // LN
            push_notes_ln(notes, LaneCode::NOTELN1, 0);
            push_notes_ln(notes, LaneCode::NOTELN2, 1);

            // invisible
            push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV1, 0);
            push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV2, 1);

            // mine, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover
            push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE1, 0);
            push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE2, 1);

            // BGM
            for (unsigned i = 0; i < objBms.bgmLayersCount[m]; i++)
            {
                auto ch = objBms.getLane(LaneCode::BGM, i, m);
                for (const auto& n : ch.notes)
                    //              { beat,                               { { lane,                       val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGM, i }, n.value } });
            }

            // BGA
            if (gSwitches.get(eSwitch::SYSTEM_BGA))
            {
                {
                    auto ch = objBms.getLane(LaneCode::BGABASE, 0, m);
                    for (const auto& n : ch.notes)
                        //              { beat,                               { { lane,                        val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGABASE, 0 }, n.value } });
                }
                {
                    auto ch = objBms.getLane(LaneCode::BGALAYER, 0, m);
                    for (const auto& n : ch.notes)
                        //              { beat,                               { { lane,                        val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGALAYER, 0 }, n.value } });
                }
                {
                    auto ch = objBms.getLane(LaneCode::BGAPOOR, 0, m);
                    for (const auto& n : ch.notes)
                        //              { beat,                               { { lane,                        val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGAPOOR, 0 }, n.value } });
                }
            }

            // BPM Change
            {
                auto ch = objBms.getLane(LaneCode::BPM, 0, m);
                for (const auto& n : ch.notes)
                    //              { beat,                               { { lane,                        val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BPM, 0 }, n.value } });
            }

            // EX BPM
            {
                auto ch = objBms.getLane(LaneCode::EXBPM, 0, m);
                for (const auto& n : ch.notes)
                    //              { beat,                               { { lane,                        val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::EXBPM, 0 }, n.value } });
            }

            // Stop
            {
                auto ch = objBms.getLane(LaneCode::STOP, 0, m);
                for (const auto& n : ch.notes)
                    //              { beat,                               { { lane,                        val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::STOP, 0 }, n.value } });
            }
        }

        // Sort by time / lane value
        std::stable_sort(notes.begin(), notes.end());

        ///////////////////////////////////////////////////////////////////////

        // Calculate note times and push to note list
        Segment lastBPMChangedSegment(0, 1);
        double stopBeat = 0;
        double currentSpd = 1.0;
        Metre metre = objBms.metres[m];      // visual beat
		Time beatLength = Time::singleBeatLengthFromBPM(bpm);

        for (const auto& note : notes)
        {
            auto[noteSegment, noteinfo] = note;
            auto[lane, val] = noteinfo;
            double beatFromBPMChange = (noteSegment - lastBPMChangedSegment) * metre;
            Metre beat = basebeat + noteSegment * metre;
			Time notetime = bpmfucked ? LLONG_MAX : basetime + beatLength * beatFromBPMChange;

            if (lane.type >= eLanePriority::NOTE && lane.type <= eLanePriority::MINE)
            {
                if (lane.type == eLanePriority::LNTAIL)
                {
                    NoteLane chartLane = idxToChannel(chart::channelToIdx(NoteLaneCategory::LN, lane.index));
                    size_t gameLaneIdx = gameLaneMap[chartLane.second];
                    size_t gameLaneIdxLN = gameLaneLNIndex[gameLaneIdx];
                    assert(gameLaneIdxLN != _);
                    assert(laneOccupiedByLN[gameLaneIdxLN] == true);
                    laneOccupiedByLN[gameLaneIdxLN] = false;
                    gameLaneLNIndex[gameLaneIdx] = _;
                    _noteLists[channelToIdx(chartLane.first, gameLaneIdxLN)].push_back({ m, beat, notetime, (long long)val, Note::LN_TAIL, false });
                }
                else // normal, invisible, mine, LN head
                {
                    NoteLane chartLane;
                    switch (lane.type)
                    {
                    case eLanePriority::NOTE:
                        chartLane = idxToChannel(lane.index);
                        break;
                    case eLanePriority::LNHEAD:
                        chartLane = idxToChannel(chart::channelToIdx(NoteLaneCategory::LN, lane.index));
                        break;
                    case eLanePriority::INV:
                        chartLane = idxToChannel(chart::channelToIdx(NoteLaneCategory::Invs, lane.index));
                        break;
                    case eLanePriority::MINE:
                        chartLane = idxToChannel(chart::channelToIdx(NoteLaneCategory::Mine, lane.index));
                        break;
                    default:
                        assert(false);
                        break;
                    }
                    size_t gameLaneIdx = gameLaneMap[chartLane.second];
                    size_t gameLaneIdxMod = gameLaneIdx;
                    size_t laneMin, laneMax;
                    int laneArea;
                    if (gameLaneIdx >= Sc1 && gameLaneIdx <= Sc1 + laneCountRandom)
                    {
                        laneMin = K1;                              // K1
                        laneMax = laneMin + (laneCountRandom - 1); // K7
                        laneArea = 0;
                    }
                    else if (gameLaneIdx >= Sc1 + laneCountRandom + 1 && gameLaneIdx <= Sc1 + laneCountRandom + 1 + laneCountRandom - 1 || gameLaneIdx == Sc2)
                    {
                        laneMin = K1 + laneCountRandom;            // K8
                        laneMax = laneMin + (laneCountRandom - 1); // K14
                        laneArea = 1;
                    }
                    else
                    {
                        assert(false);
                        break;
                    }

                    switch (gPlayContext.mods[_playerSlot].chart)
                    {
                    case eModChart::SRAN:
                        if (gameLaneIdx != Sc1 && gameLaneIdx != Sc2)
                        {
                            constexpr int threshold_ms = 50;
                            std::vector<NoteLaneIndex> placable;
                            for (NoteLaneIndex i = NoteLaneIndex(laneMin); i != NoteLaneIndex(laneMax + 1); ++*(size_t*)&i)
                            {
                                if (laneOccupiedByLN[i])
                                {
                                    continue;
                                }
                                else if (_noteLists[channelToIdx(NoteLaneCategory::Note, i)].empty())
                                {
                                    placable.push_back(i);
                                    continue;
                                }
                                else
                                {
                                    auto lastNote = --_noteLists[channelToIdx(NoteLaneCategory::Note, i)].end();
                                    if (notetime - lastNote->time >= threshold_ms && gameLaneLNIndex[i] == _)
                                        placable.push_back(i);
                                }
                            }
                            gameLaneIdxMod = placable.empty() ? (K1 + rng() % laneCountRandom) : placable[rng() % placable.size()];
                        }
                        break;

                    case eModChart::HRAN:
                        if (gameLaneIdx != Sc1 && gameLaneIdx != Sc2)
                        {
                            constexpr int threshold_ms = 250;
                            std::vector<NoteLaneIndex> placable;
                            for (NoteLaneIndex i = NoteLaneIndex(laneMin); i != NoteLaneIndex(laneMax + 1); ++ * (size_t*)&i)
                            {
                                if (laneOccupiedByLN[i])
                                {
                                    continue;
                                }
                                else if (_noteLists[channelToIdx(NoteLaneCategory::Note, i)].empty())
                                {
                                    placable.push_back(i);
                                    continue;
                                }
                                else
                                {
                                    auto lastNote = --_noteLists[channelToIdx(NoteLaneCategory::Note, i)].end();
                                    if (notetime - lastNote->time >= threshold_ms && gameLaneLNIndex[i] == _)
                                        placable.push_back(i);
                                }
                            }
                            if (!placable.empty())
                                gameLaneIdxMod = placable[rng() % placable.size()];
                            else
                            {
                                using noteTimePair = std::pair<long long, NoteLaneIndex>;
                                std::vector<noteTimePair> placableMin;
                                for (NoteLaneIndex i = NoteLaneIndex(laneMin); i != NoteLaneIndex(laneMax + 1); ++ * (size_t*)&i)
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
                        constexpr int threshold_scr_ms = 33;    // threshold of moving notes to keyboard lanes
                        constexpr int threshold_ms = 250;       // try to not make keyboard jacks
                        size_t laneMin, laneMax, laneScratch;
                        int laneStep;
                        if (laneArea == 0)
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
                            auto& lastScratch = --_noteLists[channelToIdx(NoteLaneCategory::Note, laneScratch)].end();
                            if (notetime - lastScratch->time >= threshold_scr_ms && !laneOccupiedByLN[laneScratch])
                                gameLaneIdxMod = laneScratch;
                            else
                            {
                                bool availableLaneFound = false;
                                for (NoteLaneIndex i = (NoteLaneIndex)laneMin; i != (NoteLaneIndex)laneMax + laneStep; *(size_t*)&i += laneStep)
                                {
                                    auto lastNote = --_noteLists[channelToIdx(NoteLaneCategory::Note, laneScratch)].end();
                                    if (notetime - lastNote->time >= threshold_ms && !laneOccupiedByLN[i])
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

                    _noteLists[channelToIdx(chartLane.first, gameLaneIdxMod)].push_back({ m, beat, notetime, (long long)val });

                    if (lane.type == eLanePriority::LNHEAD)
                    {
                        gameLaneLNIndex[gameLaneIdx] = (NoteLaneIndex)gameLaneIdxMod;
                        laneOccupiedByLN[gameLaneIdxMod] = true;
                    }
                }
            }
            else if (lane.type == eLanePriority::BGM)
            {
                _bgmNoteLists[lane.index].push_back({ m, beat, notetime, (long long)val, 0 });
            }
            else if (!bpmfucked) switch (lane.type)
            {
            case eLanePriority::BGABASE:
				_specialNoteLists[(size_t)eNoteExt::BGABASE].push_back({ m, beat, notetime, (long long)val });
				break;
            case eLanePriority::BGALAYER:
                _specialNoteLists[(size_t)eNoteExt::BGALAYER].push_back({ m, beat, notetime, (long long)val });
				break;
            case eLanePriority::BGAPOOR:
                _specialNoteLists[(size_t)eNoteExt::BGAPOOR].push_back({ m, beat, notetime, (long long)val });
				break;

            case eLanePriority::BPM:
                if (bpm == static_cast<BPM>(val)) break;
                basetime = notetime;
                lastBPMChangedSegment = noteSegment;
                bpm = static_cast<BPM>(val);
				beatLength = Time::singleBeatLengthFromBPM(bpm);
                _bpmNoteList.push_back({ m, beat, notetime, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case eLanePriority::EXBPM:
                if (bpm == objBms.exBPM[val]) break;
                basetime = notetime;
                lastBPMChangedSegment = noteSegment;
                bpm = objBms.exBPM[val];
                beatLength = Time::singleBeatLengthFromBPM(bpm);
                _bpmNoteList.push_back({ m, beat, notetime, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case eLanePriority::STOP:
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
		basetime += beatLength * (1.0 - lastBPMChangedSegment) * metre.toDouble();
		basebeat += metre;

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
    Metre inStopBeat;
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
        _currentBeat = inStopBeat - _barMetrePos[_currentBar];
    }

}