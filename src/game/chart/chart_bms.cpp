#include "chart_bms.h"
#include "game/scene/scene_context.h"
#include <random>
#include "game/runtime/state.h"

static const size_t NOPE = -1;

// from NoteLaneIndex to Input::Pad
const std::vector<Input::Pad>& LaneToKey(int keys, size_t idx)
{
    assert(idx < 26);
    using namespace Input;
    switch (keys)
    {
    case 5:
    case 10:
    {
        static const std::vector<Input::Pad> pad[] = {
            {Input::Pad::S1L, Input::Pad::S1R}, // Sc1
            {Input::Pad::K11},
            {Input::Pad::K12},
            {Input::Pad::K13},
            {Input::Pad::K14},
            {Input::Pad::K15},
            {Input::Pad::K21},
            {Input::Pad::K22},
            {Input::Pad::K23},
            {Input::Pad::K24},
            {Input::Pad::K25},
            {}, {}, {}, {}, {},{},{},{},{},{},{},{},{},{}, //11~24
            {Input::Pad::S2L, Input::Pad::S2R} // Sc2
        };
        return pad[idx];
    }
    case 7:
    case 14:
    {
        static const std::vector<Input::Pad> pad[] = {
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
        return pad[idx];
    }
    case 9:
    {
        static const std::vector<Input::Pad> pad[] = {
            {}, // Sc1
            {Input::Pad::K11},
            {Input::Pad::K12},
            {Input::Pad::K13},
            {Input::Pad::K14},
            {Input::Pad::K15},
            {Input::Pad::K16},
            {Input::Pad::K17},
            {Input::Pad::K18},
            {Input::Pad::K19},
            {},{},{},{},{},{},{},{},{},{},{},{},{},{},{}, //15~24
            {Input::Pad::S2L, Input::Pad::S2R} // Sc2
        };
        return pad[idx];
    }
    }
    static const std::vector<Input::Pad> empty{};
    return empty;
}

// from Input::Pad to NoteLaneIndex
chart::NoteLaneIndex KeyToLane(int keys, Input::Pad pad)
{
    assert(pad < 30);

    using namespace chart;
    switch (keys)
    {
    case 5:
    case 10:
    {
        static const NoteLaneIndex lane[] =
        {
            Sc1, Sc1, K1, K2, K3, K4, K5, _, _, _, _, _, _, _, _,
            Sc2, Sc2, K6, K7, K8, K9, K10, _, _, _, _, _, _, _, _,
        };
        return lane[pad];
    }
    case 7:
    case 14:
    {
        static const NoteLaneIndex lane[] =
        {
            Sc1, Sc1, K1, K2, K3, K4, K5, K6, K7, _, _, _, _, _, _,
            Sc2, Sc2, K8, K9, K10, K11, K12, K13, K14, _, _, _, _, _, _,
        };
        return lane[pad];
    }
    case 9:
    {
        static const NoteLaneIndex lane[] =
        {
            _, _, K1, K2, K3, K4, K5, K6, K7, K8, K9, _, _, _, _,
            _, _, _, _, _, _, _, _, _, _, _, _, _, _, _,
        };
        return lane[pad];
    }
    }
    return _;
}

using namespace chart;

ChartObjectBMS::ChartObjectBMS(int slot) : ChartObjectBase(slot, BGM_LANE_COUNT, (size_t)eNoteExt::EXT_COUNT),
    _currentStopNote(_specialNoteLists.front().begin())
{
}

ChartObjectBMS::ChartObjectBMS(int slot, std::shared_ptr<ChartFormatBMS> b) : ChartObjectBMS(slot)
{
    loadBMS(*b);
}
ChartObjectBMS::ChartObjectBMS(int slot, const ChartFormatBMS& b) : ChartObjectBMS(slot)
{
    loadBMS(b);
}

void ChartObjectBMS::loadBMS(const ChartFormatBMS& objBms)
{
	_noteCount_total = objBms.notes_total;
	_noteCount_regular = objBms.notes_scratch + objBms.notes_key;
	_noteCount_ln = objBms.notes_scratch_ln + objBms.notes_key_ln;
    _noteCount_scratch = objBms.notes_scratch;
    _noteCount_scratch_ln = objBms.notes_scratch_ln;

    barMetreLength.clear();
    _barMetrePos.clear();
    _barTimestamp.clear();

    int laneCountOneSide = 7;
    switch (objBms.gamemode)
    {
    case 5:
    case 10: laneCountOneSide = 5; break;
    case 7:
    case 14: laneCountOneSide = 7; break;
    case 9:
    case 18: laneCountOneSide = 9; break;
    default: break;
    }
    size_t laneLeftStart = K1;
    size_t laneLeftEnd = K1 + laneCountOneSide - 1;
    size_t laneRightStart = K1 + laneCountOneSide;
    size_t laneRightEnd = K1 + laneCountOneSide + laneCountOneSide - 1;

    bool isChartDP = objBms.player != 1;
    _keys = laneCountOneSide;
    if (isChartDP)
    {
        _keys *= 2;
    }
    else if (gChartContext.isDoubleBattle)
    {
        _noteCount_total *= 2;
        _noteCount_regular *= 2;
        _noteCount_ln *= 2;
    }

	Time basetime{ 0 };
	Metre basemetre{ 0, 1 };

    BPM bpm = objBms.startBPM * gSelectContext.pitchSpeed;
    _currentBPM = bpm;
    _bpmNoteList.push_back({ 0, {0, 1}, 0, 0, 0, bpm });
    bool bpmfucked = false; // set to true when BPM is changed to zero or negative value
    std::bitset<NOTELANEINDEX_COUNT> isLnTail{ 0 };

    std::array<NoteLaneIndex, NOTELANEINDEX_COUNT> gameLaneMap;
    for (size_t i = Sc1; i < NOTELANEINDEX_COUNT; ++i) 
        gameLaneMap[i] = (NoteLaneIndex)i;

    uint64_t seed = gPlayContext.randomSeed;
    if (gPlayContext.isReplay && gPlayContext.replay)
        seed = gPlayContext.replay->randomSeed;
    else if (_playerSlot == PLAYER_SLOT_MYBEST && gPlayContext.replayMybest)
        seed = gPlayContext.replayMybest->randomSeed;
    std::mt19937_64 rng(seed);

    if (gPlayContext.isBattle && _playerSlot == PLAYER_SLOT_TARGET)
    {
        // notes are loaded in 2P area, we should check randomRight instead of randomLeft
        switch (gPlayContext.mods[_playerSlot].randomRight)
        {
        case eModRandom::RANDOM:
            std::shuffle(gameLaneMap.begin() + laneRightStart, gameLaneMap.begin() + laneRightEnd + 1, rng);
            break;

        case eModRandom::MIRROR:
            std::reverse(gameLaneMap.begin() + laneRightStart, gameLaneMap.begin() + laneRightEnd + 1);
            break;
        
        case eModRandom::RRAN:
        {
            size_t count = (laneRightEnd - laneRightStart + 1);
            size_t lane = rng() % count;
            for (size_t c = laneRightStart; c <= laneRightEnd; ++c, lane = (lane + 1) % count)
                gameLaneMap[lane] = NoteLaneIndex(laneRightStart + lane);

            bool mirror = bool(rng() % 2);
            if (mirror)
                std::reverse(gameLaneMap.begin() + laneRightStart, gameLaneMap.begin() + laneRightEnd + 1);
            break;
        }
        }
    }
    else
    {
        switch (gPlayContext.mods[_playerSlot].randomLeft)
        {
        case eModRandom::RANDOM:
            std::shuffle(gameLaneMap.begin() + laneLeftStart, gameLaneMap.begin() + laneLeftEnd + 1, rng);
            break;

        case eModRandom::MIRROR:
            std::reverse(gameLaneMap.begin() + laneLeftStart, gameLaneMap.begin() + laneLeftEnd + 1);
            break;

        case eModRandom::RRAN:
        {
            size_t count = (laneLeftEnd - laneLeftStart + 1);
            size_t lane = rng() % count;
            for (size_t c = laneLeftStart; c <= laneLeftEnd; ++c, lane = (lane + 1) % count)
                gameLaneMap[lane] = NoteLaneIndex(laneLeftStart + lane);

            bool mirror = bool(rng() % 2);
            if (mirror)
                std::reverse(gameLaneMap.begin() + laneLeftStart, gameLaneMap.begin() + laneLeftEnd + 1);
            break;
        }
        }
        if (isChartDP || gChartContext.isDoubleBattle)
        {
            switch (gPlayContext.mods[_playerSlot].randomRight)
            {
            case eModRandom::RANDOM:
                std::shuffle(gameLaneMap.begin() + laneRightStart, gameLaneMap.begin() + laneRightEnd + 1, rng);
                break;

            case eModRandom::MIRROR:
                std::reverse(gameLaneMap.begin() + laneRightStart, gameLaneMap.begin() + laneRightEnd + 1);
                break;

            case eModRandom::RRAN:
            {
                size_t count = (laneRightEnd - laneRightStart + 1);
                size_t lane = rng() % count;
                for (size_t c = laneRightStart; c <= laneRightEnd; ++c, lane = (lane + 1) % count)
                    gameLaneMap[lane] = NoteLaneIndex(laneRightStart + lane);

                bool mirror = bool(rng() % 2);
                if (mirror)
                    std::reverse(gameLaneMap.begin() + laneRightStart, gameLaneMap.begin() + laneRightEnd + 1);
                break;
            }

            case eModRandom::DB_SYNCHRONIZE:
            {
                assert(gChartContext.isDoubleBattle);

                size_t count = (laneLeftEnd - laneLeftStart + 1);
                std::vector<size_t> offsets(count);
                for (size_t i = 0; i <= laneLeftEnd - laneLeftStart; ++i)
                    offsets[i] = i;
                std::shuffle(offsets.begin(), offsets.end(), rng);

                for (size_t i = laneLeftStart; i <= laneLeftEnd; ++i)
                    gameLaneMap[i] = NoteLaneIndex(laneLeftStart + offsets[i - laneLeftStart]);
                for (size_t i = laneRightStart; i <= laneRightEnd; ++i)
                    gameLaneMap[i] = NoteLaneIndex(laneRightStart + offsets[i - laneRightStart]);
                break;
            }

            case eModRandom::DB_SYMMETRY:
            {
                assert(gChartContext.isDoubleBattle);

                size_t count = (laneLeftEnd - laneLeftStart + 1);
                std::vector<size_t> offsets(count);
                for (size_t i = 0; i <= laneLeftEnd - laneLeftStart; ++i)
                    offsets[i] = i;
                std::shuffle(offsets.begin(), offsets.end(), rng);

                for (size_t i = laneLeftStart; i <= laneLeftEnd; ++i)
                    gameLaneMap[i] = NoteLaneIndex(laneLeftStart + offsets[i - laneLeftStart]);
                for (size_t i = laneRightStart; i <= laneRightEnd; ++i)
                    gameLaneMap[i] = NoteLaneIndex(laneRightStart + (count - 1) - offsets[i - laneRightStart]);
                break;
            }

            }
        }
    }

    std::bitset<NOTELANEINDEX_COUNT> laneOccupiedByLN{ 0 };

    std::array<NoteLaneIndex, NOTELANEINDEX_COUNT> gameLaneLNIndex;
    gameLaneLNIndex.fill(_);

    bool leadInTimeSet = false;

    size_t lastBarIdx = objBms.lastBarIdx;

    for (unsigned m = 0; m <= objBms.lastBarIdx; m++)
    {
		barMetreLength.push_back(objBms.metres[m]);
		_barMetrePos.push_back(basemetre);
        _barTimestamp.push_back(basetime);

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
            auto push_notes = [&objBms, m, laneCountOneSide](decltype(notes)& notes, eLanePriority priority, LaneCode code, int area)
            {
                for (unsigned i = 0; i < 10; i++)
                {
                    auto ch = objBms.getLane(code, i, m);
                    unsigned index = i;
                    if (area != 0)
                    {
                        if (index == Sc1)
                        {
                            assert(area == 1);
                            index = Sc2;
                        }
                        else
                        {
                            index += laneCountOneSide * area;
                        }
                    }
                    for (const auto& n : ch.notes)
                    {
                        //              { metre,                               { { lane,              val     } }
                        notes.push_back({ Segment(n.segment, ch.resolution), { { priority, index }, n.value } });
                    }
                }
            };
            auto push_notes_ln = [&objBms, m, laneCountOneSide, &isLnTail](decltype(notes)& notes, LaneCode code, int area)
            {
                for (unsigned i = 0; i < 10; i++)
                {
                    auto ch = objBms.getLane(code, i, m);
                    unsigned index = i;
                    if (area != 0)
                    {
                        if (index == Sc1)
                        {
                            assert(area == 1);
                            index = Sc2;
                        }
                        else
                        {
                            index += laneCountOneSide * area;
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

            // channel misorder (#xxx08, #xxx09) is already handled in chartformat object, do not convert here
            // 0 is Scratch, 1-9 are keys, matching by order

            if (gPlayContext.isBattle && _playerSlot == PLAYER_SLOT_TARGET)
            {
                // load notes into 2P area
                
                // Regular Notes
                push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE1, 1);

                // LN
                push_notes_ln(notes, LaneCode::NOTELN1, 1);

                // invisible
                push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV1, 1);

                // mine, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover
                push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE1, 1);
            }
            else if (isChartDP && gPlayContext.mods[_playerSlot].DPFlip)
            {
                // Regular Notes
                push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE2, 0);
                push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE1, 1);

                // LN
                push_notes_ln(notes, LaneCode::NOTELN2, 0);
                push_notes_ln(notes, LaneCode::NOTELN1, 1);

                // invisible
                push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV2, 0);
                push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV1, 1);

                // mine, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover
                push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE2, 0);
                push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE1, 1);
            }
            else
            {
                // Regular Notes
                push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE1, 0);

                // LN
                push_notes_ln(notes, LaneCode::NOTELN1, 0);

                // invisible
                push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV1, 0);

                // mine, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover
                push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE1, 0);
                if (isChartDP)
                {
                    push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE2, 1);
                    push_notes_ln(notes, LaneCode::NOTELN2, 1);
                    push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV2, 1);
                    push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE2, 1);
                }
                else if (gChartContext.isDoubleBattle)
                {
                    push_notes(notes, eLanePriority::NOTE, LaneCode::NOTE1, 1);
                    push_notes_ln(notes, LaneCode::NOTELN1, 1);
                    push_notes(notes, eLanePriority::INV, LaneCode::NOTEINV1, 1);
                    push_notes(notes, eLanePriority::MINE, LaneCode::NOTEMINE1, 1);
                }
            }

            // BGM
            for (unsigned i = 0; i < objBms.bgmLayersCount[m]; i++)
            {
                auto ch = objBms.getLane(LaneCode::BGM, i, m);
                for (const auto& n : ch.notes)
                    //              { metre,                               { { lane,                       val     } }
                    notes.push_back({ fraction(n.segment, ch.resolution), { { eLanePriority::BGM, i }, n.value } });
            }

            // BGA
            if (State::get(IndexSwitch::SYSTEM_BGA))
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
            auto& [noteSegment, noteinfo] = note;
            auto& [lane, val] = noteinfo;
            double metreFromBPMChange = (noteSegment - lastBPMChangedSegment) * barMetre;
            Metre notemetre = basemetre + noteSegment * barMetre;
			Time notetime = bpmfucked ? LLONG_MAX : basetime + beatLength * (metreFromBPMChange * 4);

            if (!leadInTimeSet && (lane.type == eLanePriority::NOTE || lane.type == eLanePriority::LNHEAD || lane.type == eLanePriority::BGM))
            {
                leadInTimeSet = true;
                _leadInTime = notetime;
            }

            size_t flags = 0;
            switch (lane.index)
            {
            case 0:
                flags |= Note::SCRATCH;
                break;
            case 6:
            case 7:
                flags |= Note::KEY_6_7;
                break;
            }

            if (lane.type >= eLanePriority::NOTE && lane.type <= eLanePriority::MINE)
            {
                lastBarIdx = m;

                if (lane.type == eLanePriority::LNTAIL)
                {
                    flags |= Note::LN_TAIL;
                    NoteLane chartLane = idxToChannel(channelToIdx(NoteLaneCategory::LN, lane.index));
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
                    _noteLists[channelToIdx(chartLane.first, gameLaneIdxLN)].push_back({ m, notemetre, notetime, flags, (long long)val, 0., false });
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
                        chartLane = idxToChannel(channelToIdx(NoteLaneCategory::LN, lane.index));
                        break;
                    case eLanePriority::INV:
                        chartLane = idxToChannel(channelToIdx(NoteLaneCategory::Invs, lane.index));
                        flags |= Note::Flags::INVS;
                        break;
                    case eLanePriority::MINE:
                        chartLane = idxToChannel(channelToIdx(NoteLaneCategory::Mine, lane.index));
                        flags |= Note::Flags::MINE;
                        break;
                    default:
                        assert(false);
                        break;
                    }
                    size_t gameLaneIdx = gameLaneMap[chartLane.second];
                    size_t gameLaneIdxMod = gameLaneIdx;
                    size_t laneMin, laneMax;
                    int laneArea;
                    if (gameLaneIdx == Sc1 || gameLaneIdx >= laneLeftStart && gameLaneIdx <= laneLeftEnd)
                    {
                        laneMin = laneLeftStart; // K1
                        laneMax = laneLeftEnd; // K7
                        laneArea = 0;
                    }
                    else if (gameLaneIdx == Sc2 || gameLaneIdx >= laneRightStart && gameLaneIdx <= laneRightEnd)
                    {
                        laneMin = laneRightStart; // K8
                        laneMax = laneRightEnd; // K14
                        laneArea = 1;
                    }
                    else
                    {
                        assert(false);
                        break;
                    }

                    switch (laneArea == 0 ? gPlayContext.mods[_playerSlot].randomLeft : gPlayContext.mods[_playerSlot].randomRight)
                    {
                    case eModRandom::SRAN:
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
                            gameLaneIdxMod = placable.empty() ? (K1 + rng() % laneCountOneSide) : placable[rng() % placable.size()];
                        }
                        break;

                    case eModRandom::HRAN:
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

                    case eModRandom::ALLSCR:
                    {
                        constexpr int threshold_scr_ms = 33;    // threshold of moving notes to keyboard lanes
                        constexpr int threshold_ms = 250;       // try not to make keyboard jacks
                        size_t laneScratch;
                        int laneStep;
                        
                        if (laneArea == 0)
                        {
                            laneStep = -1;
                            laneScratch = Sc1;
                        }
                        else
                        {
                            // apply MIRROR (put notes from lane 1 to lane 7)
                            laneStep = 1;
                            std::swap(laneMin, laneMax);
                            laneScratch = Sc2;
                        }

                        if (isChartDP)
                        {
                            // for DP, put notes from scratch-side keys prior
                            laneStep = -laneStep;
                            std::swap(laneMin, laneMax);
                        }

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
                                for (size_t i = laneMax; i != laneMin + laneStep; i += laneStep)
                                {
                                    if (laneOccupiedByLN[i])
                                    {
                                        continue;
                                    }
                                    if (_noteLists[channelToIdx(NoteLaneCategory::Note, i)].empty())
                                    {
                                        gameLaneIdxMod = i;
                                        availableLaneFound = true;
                                        break;
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
                lastBarIdx = m;

                if (_bgmNoteLists.size() <= lane.index)
                {
                    _bgmNoteLists.resize(lane.index + 1);
                    _bgmNoteListIters.resize(lane.index + 1);
                }
                _bgmNoteLists[lane.index].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
            }
            else if (!bpmfucked)
            {
                switch (lane.type)
                {
                case eLanePriority::BGABASE:
                    lastBarIdx = m;
                    _specialNoteLists[(size_t)eNoteExt::BGABASE].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
                    break;
                case eLanePriority::BGALAYER:
                    lastBarIdx = m;
                    _specialNoteLists[(size_t)eNoteExt::BGALAYER].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
                    break;
                case eLanePriority::BGAPOOR:
                    lastBarIdx = m;
                    _specialNoteLists[(size_t)eNoteExt::BGAPOOR].push_back({ m, notemetre, notetime, 0, (long long)val, 0. });
                    break;

                case eLanePriority::BPM:
                    if (bpm == static_cast<BPM>(val)) break;
                    lastBarIdx = m;
                    basetime = notetime;
                    lastBPMChangedSegment = noteSegment;
                    bpm = static_cast<BPM>(val) * gSelectContext.pitchSpeed;
                    beatLength = Time::singleBeatLengthFromBPM(bpm);
                    _bpmNoteList.push_back({ m, notemetre, notetime, 0, 0, bpm });
                    if (bpm <= 0) bpmfucked = true;
                    break;

                case eLanePriority::EXBPM:
                    if (bpm == objBms.exBPM[val]) break;
                    lastBarIdx = m;
                    basetime = notetime;
                    lastBPMChangedSegment = noteSegment;
                    bpm = objBms.exBPM[val] * gSelectContext.pitchSpeed;
                    beatLength = Time::singleBeatLengthFromBPM(bpm);
                    _bpmNoteList.push_back({ m, notemetre, notetime, 0, 0, bpm });
                    if (bpm <= 0) bpmfucked = true;
                    break;

                case eLanePriority::STOP:
                    lastBarIdx = m;
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
        }
        basetime += beatLength * (1.0 - lastBPMChangedSegment) * barMetre.toDouble() * 4;
        basemetre += barMetre;

        // add barline for next measure
        _noteLists[channelToIdx(NoteLaneCategory::EXTRA, EXTRA_BARLINE_1P)].push_back(
            { m + 1, basemetre, basetime, long long(0), false });

        _noteLists[channelToIdx(NoteLaneCategory::EXTRA, EXTRA_BARLINE_2P)].push_back(
            { m + 1, basemetre, basetime, long long(0), false });
    }

    _totalLength = lastBarIdx + 1 < _barTimestamp.size() ? _barTimestamp[lastBarIdx + 1] : basetime +
        Time(std::min(2000'000'000ll, std::max(500'000'000ll, Time::singleBeatLengthFromBPM(bpm).hres() * 4)), true);    // last measure + 1

    // get average BPM
    if (_totalLength.norm() > 0)
    {
        std::map<double, long long> bpmLength;
        double bpm = objBms.startBPM * gSelectContext.pitchSpeed;
        double bpmSum = 0.;
        Time prevTime(0);
        long long bpmMainLength = 0;
        for (const auto& n : _bpmNoteList)
        {
            long long length = (n.time - prevTime).norm();
            bpmSum += length * bpm;
            bpmLength[bpm] += length;
            bpm = n.fvalue;
            prevTime = n.time;
        }
        // last part
        {
            long long length = (_totalLength - prevTime).norm();
            bpmSum += length * bpm;
            bpmLength[bpm] += length;
        }
        if (bpmSum > 0)
        {
            _averageBPM = bpmSum / _totalLength.norm();
            _mainBPM = std::max_element(bpmLength.begin(), bpmLength.end(), [](const std::pair<double, long long>& lhs, const std::pair<double, long long>& rhs)
                {
                    return lhs.second < rhs.second;
                })->first;
        }
    }
    else
    {
        _averageBPM = _mainBPM = objBms.startBPM;
    }

    resetNoteListsIterators();
    _currentStopNote = incomingNoteSpecial(size_t(eNoteExt::STOP));
}


NoteLaneIndex ChartObjectBMS::getLaneFromKey(NoteLaneCategory cat, Input::Pad input)
{
    if (input >= Input::S1L && input < Input::LANE_COUNT && KeyToLane(_keys, input) != _)
    {
        NoteLaneIndex idx = KeyToLane(_keys, input);
        if (!isLastNote(cat, idx))
            return idx;
    }
    return _;
}

std::vector<Input::Pad> ChartObjectBMS::getInputFromLane(size_t channel)
{
    return LaneToKey(_keys, channel);
}

void ChartObjectBMS::preUpdate(const Time& vt)
{
    // check stop
    size_t idx = (size_t)eNoteExt::STOP;
    _inStopNote = false;
    while (!isLastNoteSpecial(idx, _currentStopNote))
    {
        if (vt.hres() > _currentStopNote->time.hres() + _currentStopNote->dvalue)
        {
            _stopMetre += _currentStopNote->fvalue;
            _stopBar = _currentStopNote->measure;
            ++_currentStopNote;
        }
        else if (vt >= _currentStopNote->time)
        {
            _inStopNote = true;
            break;
        }
        else // t < _currentStopNote->time
        {
            break;
        }
    }

    // check inbounds BPM change, reset stop metre if found
    auto b = incomingNoteBpm();
    if (!isLastNoteBpm(b) && vt >= b->time)
    {
        _stopMetre = 0.0;
        _stopBar = 0;
        _inStopNote = false;
    }
}

void ChartObjectBMS::postUpdate(const Time& vt)
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