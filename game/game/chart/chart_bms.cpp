#include "chart_bms.h"

namespace bms
{
    const size_t NOPE = -1;

    // Need this since current parser stores channel directly
	// TODO mapping is different between file formats
    const size_t BMSToLaneMap[] = 
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
        channelToIdx(NoteLaneCategory::LN, Sc1),
        channelToIdx(NoteLaneCategory::LN, K1),
        channelToIdx(NoteLaneCategory::LN, K2),
        channelToIdx(NoteLaneCategory::LN, K3),
        channelToIdx(NoteLaneCategory::LN, K4),
        channelToIdx(NoteLaneCategory::LN, K5),
        channelToIdx(NoteLaneCategory::LN, K6),
        channelToIdx(NoteLaneCategory::LN, K7),
        NOPE,
        NOPE,

        channelToIdx(NoteLaneCategory::LN, Sc2),
        channelToIdx(NoteLaneCategory::LN, K8),
        channelToIdx(NoteLaneCategory::LN, K9),
        channelToIdx(NoteLaneCategory::LN, K10),
        channelToIdx(NoteLaneCategory::LN, K11),
        channelToIdx(NoteLaneCategory::LN, K12),
        channelToIdx(NoteLaneCategory::LN, K13),
        channelToIdx(NoteLaneCategory::LN, K14),
        NOPE,
        NOPE,

        // LN Tail
        channelToIdx(NoteLaneCategory::LN, Sc1),
        channelToIdx(NoteLaneCategory::LN, K1),
        channelToIdx(NoteLaneCategory::LN, K2),
        channelToIdx(NoteLaneCategory::LN, K3),
        channelToIdx(NoteLaneCategory::LN, K4),
        channelToIdx(NoteLaneCategory::LN, K5),
        channelToIdx(NoteLaneCategory::LN, K6),
        channelToIdx(NoteLaneCategory::LN, K7),
        NOPE,
        NOPE,

        channelToIdx(NoteLaneCategory::LN, Sc2),
        channelToIdx(NoteLaneCategory::LN, K8),
        channelToIdx(NoteLaneCategory::LN, K9),
        channelToIdx(NoteLaneCategory::LN, K10),
        channelToIdx(NoteLaneCategory::LN, K11),
        channelToIdx(NoteLaneCategory::LN, K12),
        channelToIdx(NoteLaneCategory::LN, K13),
        channelToIdx(NoteLaneCategory::LN, K14),
        NOPE,
        NOPE,
    };
    
    // from NoteLaneIndex to Input::Ingame
    const std::vector<Input::Ingame> LaneToKeyMap[] = 
    {
        // 0: Notes
        {Input::Ingame::S1L, Input::Ingame::S1R}, // Sc1
        {Input::Ingame::K11},
        {Input::Ingame::K12},
        {Input::Ingame::K13},
        {Input::Ingame::K14},
        {Input::Ingame::K15},
        {Input::Ingame::K16},
        {Input::Ingame::K17},
        {Input::Ingame::K21}, //8
        {Input::Ingame::K22}, //9
        {Input::Ingame::K23}, //10
        {Input::Ingame::K24}, //11
        {Input::Ingame::K25}, //12
        {Input::Ingame::K26}, //13
        {Input::Ingame::K27}, //14
        {},{},{},{},{},{},{},{},{},{}, //15~24
        {Input::Ingame::S2L, Input::Ingame::S2R} // Sc2
    };

    // from Input::Ingame to NoteLaneIndex
    const NoteLaneIndex KeyToLaneMap[] = 
    {
        Sc1, Sc1,
        K1, K2, K3, K4, K5, K6, K7,
        _, _, _, _, _, _,

        Sc2, Sc2,
        K8, K9, K10, K11, K12, K13, K14,
        _, _, _, _, _, _,
    };
}

using namespace bms;

chartBMS::chartBMS() : vChart((size_t)eNotePlain::PLAIN_COUNT, (size_t)eNoteExt::EXT_COUNT)
{
}

chartBMS::chartBMS(std::shared_ptr<BMS> b) : chartBMS()
{
    loadBMS(*b);
}
chartBMS::chartBMS(const BMS& b) : chartBMS()
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

    BPM bpm = objBms._itlBPM;
    _currentBPM = bpm;
    _bpmNoteList.push_back({ 0, {0, 1}, 0, bpm });
	_measureLength.fill({ 1, 1 });
    bool bpmfucked = false; // set to true when BPM is changed to zero or negative value
    std::bitset<10> isLnTail[2]{ 0 };

    for (unsigned m = 0; m <= objBms.lastBarIdx; m++)
    {
		_measureLength[m] = objBms._measureLength[m];
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
        Beat measureLength = objBms._measureLength[m];      // visual beat
		Time beatLength = Time::singleBeatLengthFromBPM(bpm);

        for (const auto& note : notes)
        {
            auto[noteSegment, noteinfo] = note;
            auto[lane, val] = noteinfo;
            double beatFromBPMChange = (noteSegment - lastBPMChangedSegment) * measureLength;
            Beat beat = basebeat + noteSegment * measureLength;
			Time notetime = bpmfucked ? LLONG_MAX : basetime + beatLength * beatFromBPMChange;

            if (lane >= 0 && lane < 100)
            {
				// TODO mapping is different between file formats
                if (BMSToLaneMap[lane] == NOPE) continue;
                if (lane >= 40 && lane < 60)
                    _noteLists[BMSToLaneMap[lane]].push_back({ m, beat, notetime, (long long)val, NOTE_INDEX_LN_TAIL, false });// LN tail
                else
                    _noteLists[BMSToLaneMap[lane]].push_back({ m, beat, notetime, (long long)val });// normal, LN head
            }
            else if (lane >= 100 && lane <= 131)
            {
                _commonNoteLists[lane - 100 + (size_t)eNotePlain::BGM0].push_back({ m, beat, notetime, (long long)val, 0 });
            }
            else if (!bpmfucked) switch (lane)
            {
            case 0xE0:  // BGA base
				_commonNoteLists[(size_t)eNotePlain::BGABASE].push_back({ m, beat, notetime, (long long)val, NOTE_INDEX_BGA_BASE });
				break;
            case 0xE1:  // BGA layer
                _commonNoteLists[(size_t)eNotePlain::BGALAYER].push_back({ m, beat, notetime, (long long)val, NOTE_INDEX_BGA_LAYER });
				break;
            case 0xE2:  // BGA poor
                _commonNoteLists[(size_t)eNotePlain::BGAPOOR].push_back({ m, beat, notetime, (long long)val, NOTE_INDEX_BGA_POOR });
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
                //_extLists[(size_t)eNoteExt::STOP].push_back({ m, segment, baseY, noteht, curStopBeat });
				_stopNoteList.push_back({ m, beat, notetime, curStopBeat });
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
        _noteLists[channelToIdx(NoteLaneCategory::EXTRA, EXTRA_BARLINE)].push_back(
			{ m + 1, basebeat, basetime, long long(0), false });

    }

    _totalLength = basetime;    // last measure + 1

    setNoteListsIterators();
}


std::pair<NoteLaneCategory, NoteLaneIndex> chartBMS::getLaneFromKey(Input::Ingame input)
{
    if (input >= Input::S1L && input < Input::ESC && KeyToLaneMap[input] != _)
    {
        using cat = NoteLaneCategory;
        NoteLaneIndex idx = KeyToLaneMap[input];
        std::vector<std::pair<cat, sNote>> note;
        if (!isLastNoteOfLane(cat::Note, idx))
            note.push_back({ cat::Note, *incomingNoteOfLane(cat::Note, idx) });
        if (!isLastNoteOfLane(cat::Invs, idx))
            note.push_back({ cat::Invs, *incomingNoteOfLane(cat::Invs, idx) });
        if (!isLastNoteOfLane(cat::LN, idx))
            note.push_back({ cat::LN,   *incomingNoteOfLane(cat::LN, idx) });
        std::sort(note.begin(), note.end(), [](decltype(note.front())& a, decltype(note.front())& b) { return b.second.time > a.second.time; });
        for (size_t i = 0; i < note.size(); ++i)
            if (!note[i].second.hit) return { note[i].first, idx };
    }
    return { NoteLaneCategory::_, _ };
}

std::vector<Input::Ingame> chartBMS::getInputFromLane(size_t channel)
{
    if (channel >= bms::LaneToKeyMap->size())
        return {};
    else
        return bms::LaneToKeyMap[channel];
}
