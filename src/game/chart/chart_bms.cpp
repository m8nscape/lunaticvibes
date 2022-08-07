#include "chart_bms.h"
#include "game/scene/scene_context.h"
#include <random>
#include "game/data/switch.h"
#include "game/data/option.h"
#include "game/data/number.h"

using namespace chart;

const size_t NOPE = -1;

// channel misorder (#xxx08, #xxx09) is already handled in chartformat object, do not convert here
// 0 is Scratch, 1-9 are keys, matching by order
const std::vector<size_t> BMSToLaneMap = 
{
    // Normal notes
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

    // LN Head 
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

    // LN Tail
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

chartBMS::chartBMS(int slot) : vChart(slot, BGM_LANE_COUNT, (size_t)eNoteExt::EXT_COUNT),
    _currentStopNote(_specialNoteLists.front().begin())
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
	_noteCount = objBms.notes_total;
	_noteCount_regular = objBms.notes_scratch + objBms.notes_key;
	_noteCount_ln = objBms.notes_scratch_ln + objBms.notes_key_ln;
	Time basetime{ 0 };
	Metre basemetre{ 0, 1 };

    BPM bpm = objBms.startBPM * gSelectContext.pitchSpeed;
    _currentBPM = bpm;
    _bpmNoteList.push_back({ 0, {0, 1}, 0, 0, 0, bpm });
	barMetreLength.fill({ 1, 1 });
    bool bpmfucked = false; // set to true when BPM is changed to zero or negative value
    std::bitset<NOTELANEINDEX_COUNT> isLnTail{ 0 };

    int laneCountRandom = 7;
    switch (objBms.gamemode)
    {
    case 5:
    case 10: laneCountRandom = 5; break;
    case 7:
    case 14: laneCountRandom = 7; break;
    case 9:
    case 18: laneCountRandom = 9; break;
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
		barMetreLength[m] = objBms.metres[m];
		_barMetrePos[m] = basemetre;
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

        // notes [] {metre, {lane, sample/val}}
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
                            case 5:
                            case 10:
                                index += 5 * area;
                                break;

                            case 7:
                            case 14:
                                index += 7 * area;
                                break;

                            case 9:
                            case 18:
                                index += 9 * area;

                            default:
                                assert(false);  // other gamemodes does not support notes on 2P side
                                break;
                            }
                        }
                    }
                    for (const auto& n : ch.notes)
                    {
                        //              { metre,                               { { lane,              val     } }
                        notes.push_back({ Segment(n.segment, ch.resolution), { { priority, index }, n.value } });
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
                            case 5:
                            case 10:
                                index += 5 * area;
                                break;

                            case 7:
                            case 14:
                                index += 7 * area;
                                break;

                            case 9:
                            case 18:
                                index += 9 * area;

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
            if (objBms.player != 1) push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE2, 1);

            // LN
            push_notes_ln(notes, LaneCode::NOTELN1, 0);
            if (objBms.player != 1) push_notes_ln(notes, LaneCode::NOTELN2, 1);

            // invisible
            push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV1, 0);
            if (objBms.player != 1) push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV2, 1);

            // mine, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover
            push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE1, 0);
            if (objBms.player != 1) push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE2, 1);

            // BGM
            for (unsigned i = 0; i < objBms.bgmLayersCount[m]; i++)
            {
                auto ch = objBms.getLane(LaneCode::BGM, i, m);
                for (const auto& n : ch.notes)
                    //              { metre,                               { { lane,                       val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGM, i }, n.value } });
            }

            // BGA
            if (gSwitches.get(eSwitch::SYSTEM_BGA))
            {
                {
                    auto ch = objBms.getLane(LaneCode::BGABASE, 0, m);
                    for (const auto& n : ch.notes)
                        //              { metre,                               { { lane,                        val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGABASE, 0 }, n.value } });
                }
                {
                    auto ch = objBms.getLane(LaneCode::BGALAYER, 0, m);
                    for (const auto& n : ch.notes)
                        //              { metre,                               { { lane,                        val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGALAYER, 0 }, n.value } });
                }
                {
                    auto ch = objBms.getLane(LaneCode::BGAPOOR, 0, m);
                    for (const auto& n : ch.notes)
                        //              { metre,                               { { lane,                        val     } }
                        notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGAPOOR, 0 }, n.value } });
                }
            }

            // BPM Change
            {
                auto ch = objBms.getLane(LaneCode::BPM, 0, m);
                for (const auto& n : ch.notes)
                    //              { metre,                               { { lane,                        val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BPM, 0 }, n.value } });
            }

            // EX BPM
            {
                auto ch = objBms.getLane(LaneCode::EXBPM, 0, m);
                for (const auto& n : ch.notes)
                    //              { metre,                               { { lane,                        val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::EXBPM, 0 }, n.value } });
            }

            // Stop
            {
                auto ch = objBms.getLane(LaneCode::STOP, 0, m);
                for (const auto& n : ch.notes)
                    //              { metre,                               { { lane,                        val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::STOP, 0 }, n.value } });
            }
        }

        // Sort by time / lane value
        std::stable_sort(notes.begin(), notes.end(), [](const std::pair<Segment, std::pair<Lane, unsigned>>& lhs, const std::pair<Segment, std::pair<Lane, unsigned>>& rhs)
            {
                // only compare Segment; Lane and sample must keep original order
                return lhs.first < rhs.first;
            });

        ///////////////////////////////////////////////////////////////////////

        // Calculate note times and push to note list
        Segment lastBPMChangedSegment(0, 1);
        double stopMetre = 0;
        Metre barMetre = objBms.metres[m];      // visual metre
		Time beatLength = Time::singleBeatLengthFromBPM(bpm);

        for (const auto& note : notes)
        {
            auto[noteSegment, noteinfo] = note;
            auto[lane, val] = noteinfo;
            double metreFromBPMChange = (noteSegment - lastBPMChangedSegment) * barMetre;
            Metre notemetre = basemetre + noteSegment * barMetre;
			Time notetime = bpmfucked ? LLONG_MAX : basetime + beatLength * (metreFromBPMChange * 4);

            if (lane.type >= eLanePriority::NOTE && lane.type <= eLanePriority::MINE)
            {
                if (lane.type == eLanePriority::LNTAIL)
                {
                    NoteLane chartLane = idxToChannel(chart::channelToIdx(NoteLaneCategory::LN, lane.index));
                    size_t gameLaneIdx = gameLaneMap[chartLane.second];
                    size_t gameLaneIdxLN = gameLaneLNIndex[gameLaneIdx];
                    assert(gameLaneIdxLN != _); // duplicate LN tail found. Check chartformat parsing
                    assert(laneOccupiedByLN[gameLaneIdxLN] == true);
                    if (gameLaneIdxLN == _ || !laneOccupiedByLN[gameLaneIdxLN])
                    {
                        LOG_ERROR << "[BMS] Parsing LN error! File: " << objBms.absolutePath.u8string();
                        return;
                    }
                    laneOccupiedByLN[gameLaneIdxLN] = false;
                    gameLaneLNIndex[gameLaneIdx] = _;
                    _noteLists[channelToIdx(chartLane.first, gameLaneIdxLN)].push_back({ m, notemetre, notetime, Note::LN_TAIL, (long long)val, 0., false });
                }
                else // normal, invisible, mine, LN head
                {
                    NoteLane chartLane;
                    size_t flags = 0;
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
                        flags = Note::Flags::INVS;
                        break;
                    case eLanePriority::MINE:
                        chartLane = idxToChannel(chart::channelToIdx(NoteLaneCategory::Mine, lane.index));
                        flags = Note::Flags::MINE;
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
                                    auto lastNote = --_noteLists[i].end();
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
                        size_t laneScratch;
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
                            auto lastScratch = --_noteLists[channelToIdx(NoteLaneCategory::Note, laneScratch)].end();
                            if (notetime - lastScratch->time >= threshold_scr_ms && !laneOccupiedByLN[laneScratch])
                                gameLaneIdxMod = laneScratch;
                            else
                            {
                                bool availableLaneFound = false;
                                for (NoteLaneIndex i = (NoteLaneIndex)laneMin; i != (NoteLaneIndex)laneMax + laneStep; *(size_t*)&i += laneStep)
                                {
                                    if (_noteLists[channelToIdx(NoteLaneCategory::Note, i)].empty())
                                    {
                                        gameLaneIdxMod = i;
                                        availableLaneFound = true;
                                        continue;
                                    }
                                    else
                                    {
                                        auto lastNote = --_noteLists[channelToIdx(NoteLaneCategory::Note, i)].end();
                                        if (notetime - lastNote->time >= threshold_ms && !laneOccupiedByLN[i])
                                        {
                                            gameLaneIdxMod = i;
                                            availableLaneFound = true;
                                            break;
                                        }
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

                    _noteLists[channelToIdx(chartLane.first, gameLaneIdxMod)].push_back({ m, notemetre, notetime, flags, (long long)val, 0. });

                    if (lane.type == eLanePriority::LNHEAD)
                    {
                        gameLaneLNIndex[gameLaneIdx] = (NoteLaneIndex)gameLaneIdxMod;
                        laneOccupiedByLN[gameLaneIdxMod] = true;
                    }
                }
            }
            else if (lane.type == eLanePriority::BGM)
            {
                if (_bgmNoteLists.size() <= lane.index)
                {
                    _bgmNoteLists.resize(lane.index + 1);
                    _bgmNoteListIters.resize(lane.index + 1);
                }
                _bgmNoteLists[lane.index].push_back({ m, notemetre, notetime, 0, (long long)val, 0.});
            }
            else if (!bpmfucked) switch (lane.type)
            {
            case eLanePriority::BGABASE:
				_specialNoteLists[(size_t)eNoteExt::BGABASE].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
				break;
            case eLanePriority::BGALAYER:
                _specialNoteLists[(size_t)eNoteExt::BGALAYER].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
				break;
            case eLanePriority::BGAPOOR:
                _specialNoteLists[(size_t)eNoteExt::BGAPOOR].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
				break;

            case eLanePriority::BPM:
                if (bpm == static_cast<BPM>(val)) break;
                basetime = notetime;
                lastBPMChangedSegment = noteSegment;
                bpm = static_cast<BPM>(val) * gSelectContext.pitchSpeed;
				beatLength = Time::singleBeatLengthFromBPM(bpm);
                _bpmNoteList.push_back({ m, notemetre, notetime, 0, 0, bpm});
                if (bpm <= 0) bpmfucked = true;
                break;

            case eLanePriority::EXBPM:
                if (bpm == objBms.exBPM[val]) break;
                basetime = notetime;
                lastBPMChangedSegment = noteSegment;
                bpm = objBms.exBPM[val] * gSelectContext.pitchSpeed;
                beatLength = Time::singleBeatLengthFromBPM(bpm);
                _bpmNoteList.push_back({ m, notemetre, notetime, 0, 0, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case eLanePriority::STOP:
                double noteStopMetre = objBms.stop[val] / 192.0;
                if (noteStopMetre <= 0) break;
				Time noteStopTime{ (long long)std::floor(beatLength.hres() * noteStopMetre * 4), true };
				_specialNoteLists[(size_t)eNoteExt::STOP].push_back({ m, notemetre, notetime, 0, noteStopTime.hres(), noteStopMetre });
                //_chartingSpeedList.push_back({ m, notemetre, noteht, 0.0 });
                //_chartingSpeedList.push_back({ m, notemetre + d2fr(noteStopBeat), noteht + noteStopTime, currentSpd });
                stopMetre += noteStopMetre;
                basetime += noteStopTime;
                break;
            }
        }
		basetime += beatLength * (1.0 - lastBPMChangedSegment) * barMetre.toDouble() * 4;
		basemetre += barMetre;

        // add barline for next measure
        _noteLists[chart::channelToIdx(NoteLaneCategory::EXTRA, EXTRA_BARLINE)].push_back(
			{ m + 1, basemetre, basetime, long long(0), false });

    }

    _totalLength = basetime + Time(std::max(500'000'000ll, Time::singleBeatLengthFromBPM(bpm).hres() * 4), true);    // last measure + 1

    resetNoteListsIterators();
    _currentStopNote = incomingNoteSpecial(size_t(eNoteExt::STOP));
}


chart::NoteLaneIndex chartBMS::getLaneFromKey(chart::NoteLaneCategory cat, Input::Pad input)
{
    if (input >= Input::S1L && input < Input::ESC && KeyToLaneMap[input] != _)
    {
        NoteLaneIndex idx = KeyToLaneMap[input];
        if (!isLastNote(cat, idx))
            return idx;
    }
    return _;
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
    // check stop
    size_t idx = (size_t)eNoteExt::STOP;
    _inStopNote = false;
    while (!isLastNoteSpecial(idx, _currentStopNote))
    {
        if (t.hres() > _currentStopNote->time.hres() + _currentStopNote->dvalue)
        {
            _stopMetre += _currentStopNote->fvalue;
            _stopBar = _currentStopNote->measure;
            ++_currentStopNote;
        }
        else if (t >= _currentStopNote->time)
        {
            _inStopNote = true;
            break;
        }
        else // t < _currentStopNote->time
        {
            break;
        }
    }
}

void chartBMS::postUpdate(const Time& t)
{
    if (_inStopNote)
    {
        _currentMetreTemp = _currentStopNote->pos - _barMetrePos[_currentStopNote->measure];
    }
    else if (_stopBar == _currentBarTemp)
    {
        _currentMetreTemp -= _stopMetre;
    }
    else
    {
        _stopMetre = 0.0;
    }
}