#include "scroll_bms.h"

namespace bms
{
    const size_t NOPE = -1;

    // Need this since current parser stores channel directly
    const size_t BMSToChannelMap[] = 
    {
        NOPE, // 0
        K1,
        K2,
        K3,
        K4,
        K5,
        Sc1,  // pme 8 
        NOPE, // pme 9 / FREE-ZONE / FOOT PEDAL
        K6, // pme 6
        K7, // pme 7

        NOPE, // 10
        K8, // OCT FOOT PEDAL
        K9, // pms 6
        K10, // pms 7
        K11, // pms 8
        K12, // pms 9
        Sc2,  // pme 8
        NOPE, // pme 9 / FREE-ZONE / FOOT PEDAL
        K13, // pme 6
        K14, // pme 7
    };
    
    // from NoteChannelIndex to Input::Ingame
    const std::vector<Input::Ingame> ChannelToKeyMap[] = 
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

    // from Input::Ingame to NoteChannelIndex
    const NoteChannelIndex KeyToChannelMap[] = 
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

ScrollBMS::ScrollBMS() : vScroll((size_t)eNotePlain::PLAIN_COUNT, (size_t)eNoteExt::EXT_COUNT)
{
}

ScrollBMS::ScrollBMS(const BMS& b) : ScrollBMS()
{
    loadBMS(b);
}

void ScrollBMS::loadBMS(const BMS& objBms)
{
	timestamp basetime{ 0 };
    BPM bpm = objBms.getInitialBPM();
    _bpmList.push_back({ 0, {0, 1}, 0, bpm });
	_measureLength.fill({ 1, 1 });
    bool bpmfucked = false;
    for (unsigned m = 0; m <= objBms.maxMeasure; m++)
    {
		_measureLength[m] = objBms._measureLength[m];
        _measureTimestamp[m] = basetime;

        // notes [] {beat, {lane, sample/val}}
        std::vector<std::pair<BeatNorm, std::pair<unsigned, unsigned>>> notes;

        // In case the channel order is shuffled, read the data to buffer then sort it out
        // The following patterns must be arranged to keep process order by [Notes > BPM > Stop]
        {
            // Lookup BMSToChannelMap[]
            // Visible Notes, bms: 1x/2x
            for (unsigned i = 0; i < 10; i++)
            {
                auto ch = objBms.getChannel(ChannelCode::NOTE1, i, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ i, n.value } });
            }
            for (unsigned i = 0; i < 10; i++)
            {
                auto ch = objBms.getChannel(ChannelCode::NOTE2, i, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 10 + i, n.value } });
            }

            // LN: , bms: 5x/6x
            // invisible: , bms: 3x/4x
            // mine: , bms: Dx/Ex, specify a damage by [01-ZZ] (decimalize/2) ZZ: instant gameover

            // BGM: 
            for (unsigned i = 0; i < objBms.bgmLayersCount[m]; i++)
            {
                auto ch = objBms.getChannel(ChannelCode::BGM, i, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 100 + i, n.value } });
            }

            // BGA: 0xF0 / 0xF1 / 0xF2
            if (/* BGA switch */ false)
            {
                {
                    auto ch = objBms.getChannel(ChannelCode::BGABASE, 0, m);
                    for (const auto& n : ch.notes)
                        notes.push_back({ fraction(n.segment, ch.resolution),{ 0xF0, n.value } });
                }
                {
                    auto ch = objBms.getChannel(ChannelCode::BGALAYER, 0, m);
                    for (const auto& n : ch.notes)
                        notes.push_back({ fraction(n.segment, ch.resolution),{ 0xF1, n.value } });
                }
                {
                    auto ch = objBms.getChannel(ChannelCode::BGAPOOR, 0, m);
                    for (const auto& n : ch.notes)
                        notes.push_back({ fraction(n.segment, ch.resolution),{ 0xF2, n.value } });
                }
            }

            // BPM Change: 0xFE
            {
                auto ch = objBms.getChannel(ChannelCode::BPM, 0, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 0xFE, n.value } });
            }

            // EX BPM: 0xFD
            {
                auto ch = objBms.getChannel(ChannelCode::EXBPM, 0, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 0xFD, n.value } });
            }

            // Stop: 0xFF
            {
                auto ch = objBms.getChannel(ChannelCode::STOP, 0, m);
                for (const auto& n : ch.notes)
                    notes.push_back({ fraction(n.segment, ch.resolution),{ 0xFF, n.value } });
            }
        }

        // Sort by time / lane value
        std::sort(notes.begin(), notes.end());

        ///////////////////////////////////////////////////////////////////////

        // Calculate note times and push to note list
        BeatNorm lastBPMChangedSegment(0, 1);
        double stopBeat = 0;
        double currentSpd = 1.0;
        Beat measureBeat = objBms._measureLength[m];      // in Beat, not including STOP
		timestamp beatTime = timestamp::fromBPM(bpm);

        for (const auto& note : notes)
        {
            auto[segment, noteinfo] = note;
            auto[lane, val] = noteinfo;
            double piece = (segment - lastBPMChangedSegment) * measureBeat;
            Beat beat = segment * d2fr(measureBeat + stopBeat);
			timestamp notetime = bpmfucked ? LLONG_MAX : basetime + beatTime * piece;

            if (lane >= 0 && lane < 100)
            {
                if (BMSToChannelMap[lane] == NOPE) continue;
                _noteLists[BMSToChannelMap[lane]].push_back({ m, beat, notetime, (long long)val, false });
            }
            else if (lane >= 100 && lane < 131)
            {
                _plainLists[lane - 100 + (size_t)eNotePlain::BGM0].push_back({ m, beat, notetime, (long long)val });
            }
            else if (!bpmfucked) switch (lane)
            {
            case 0xF0:  // BGA base
            case 0xF1:  // BGA layer
            case 0xF2:  // BGA poor
                // TODO BGA parsing
                break;

            case 0xFD:	// ExBPM Change
                basetime = notetime;
                lastBPMChangedSegment = segment;
                bpm = objBms.exBPM[val];
				beatTime = timestamp::fromBPM(bpm);
                _bpmList.push_back({ m, beat, notetime, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case 0xFE:	// BPM Change
                basetime = notetime;
                lastBPMChangedSegment = segment;
                bpm = static_cast<BPM>(val);
				beatTime = timestamp::fromBPM(bpm);
                stopBeat = 0;
                _bpmList.push_back({ m, beat, notetime, bpm });
                if (bpm <= 0) bpmfucked = true;
                break;

            case 0xFF:	// Stop
                double curStopBeat = objBms.stop[val];
				timestamp  curStopTime{ (long long)std::floor(timestamp::fromBPM(bpm).hres() * curStopBeat / 192), true };
                if (curStopBeat <= 0) break;
                //_extLists[(size_t)eNoteExt::STOP].push_back({ m, segment, baseY, noteht, curStopBeat });
				_stopList.push_back({ m, beat, notetime, curStopBeat });
                //_scrollingSpeedList.push_back({ m, beat, noteht, 0.0 });
                //_scrollingSpeedList.push_back({ m, beat + d2fr(curStopBeat), noteht + curStopTime, currentSpd });
                stopBeat += curStopBeat / 192;
                basetime += curStopTime;
                break;
            }
        }
		basetime += beatTime * (1.0 - lastBPMChangedSegment) * measureBeat;
    }

    setIterators();
}


std::pair<NoteChannelCategory, NoteChannelIndex> ScrollBMS::getChannelFromKey(Input::Ingame input)
{
    if (input >= Input::S1L && input < Input::ESC && KeyToChannelMap[input] != _)
    {
        using cat = NoteChannelCategory;
        NoteChannelIndex idx = KeyToChannelMap[input];
        std::vector<std::pair<cat, sNote>> note;
        if (!isLastNoteOfChannel(cat::Note, idx))
            note.push_back({ cat::Note, *incomingNoteOfChannel(cat::Note, idx) });
        if (!isLastNoteOfChannel(cat::Invs, idx))
            note.push_back({ cat::Invs, *incomingNoteOfChannel(cat::Invs, idx) });
        if (!isLastNoteOfChannel(cat::LN, idx))
            note.push_back({ cat::LN,   *incomingNoteOfChannel(cat::LN, idx) });
        std::sort(note.begin(), note.end(), [](decltype(note.front())& a, decltype(note.front())& b) { return b.second.time > a.second.time; });
        for (size_t i = 0; i < note.size(); ++i)
            if (!note[i].second.hit) return { note[i].first, idx };
    }
    return { NoteChannelCategory::_, _ };
}

std::vector<Input::Ingame> ScrollBMS::getInputFromChannel(size_t channel)
{
    if (channel > sizeof(bms::ChannelToKeyMap) / sizeof(bms::ChannelToKeyMap[0]))
        return {};
    return bms::ChannelToKeyMap[channel];
}
