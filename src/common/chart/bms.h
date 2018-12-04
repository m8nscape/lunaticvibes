#pragma once
#include <array>
#include <string>
#include <list>
#include <map>

#include "chart.h"
#include "defs/defs_bms.h"
#include "utils.h"
#include "types.h"


using namespace bms;

class BMS: public vChart
{
public:
    BMS(std::string file);
    int initWithFile(std::string file);
    std::string getError();

protected:
    ErrorCode errorCode = ErrorCode::OK;
    int errorLine;

public:
    struct channel {
        struct NoteParseValue { unsigned segment; unsigned value; };
        std::list<NoteParseValue> notes{};
        int resolution = 1;
    };
    typedef std::array<std::string, MAXSAMPLEIDX + 1> FileIdxArray;
    typedef std::array<channel, MAXMEASUREIDX + 1> ChannelArray;

protected:
    // File properties.

    // Header.
    int player = 1;                // 1: single, 2: couple, 3: double, 4: battle
    int rank = 2;                 // judge, VHARD/HARD/NORMAL/EASY
    double total;
    int playLevel = 0;
    int difficulty = 3;            // N/H/A
    double bpm = 130.0;
    std::map<std::string, StringContent> extraCommands;

    // File assigned by the BMS file.
    // Ported to super class

    
    // Channels.
    int strToChannel36(channel&, const StringContent& str);
    int strToChannel16(channel&, const StringContent& str);
    std::array<ChannelArray, BGMCHANNELS> chBGM{};
    ChannelArray chStop{};
    ChannelArray chBPMChange{};
    ChannelArray chExBPMChange{};
    ChannelArray chBGABase{};
    ChannelArray chBGALayer{};
    ChannelArray chBGAPoor{};
    std::array<ChannelArray, 20> chNotesVisible{};
    std::array<ChannelArray, 20> chNotesInvisible{};
    std::array<ChannelArray, 20> chNotesLN{};
    std::array<ChannelArray, 20> chMines{};
    int strToNoteChannelDispatcher(decltype(chNotesVisible)&, int measure, int layer, int ch, const StringContent& str);

public:
    // Measures related.
    std::array<double, MAXSAMPLEIDX + 1> exBPM{};
    std::array<double, MAXSAMPLEIDX + 1> stop{};

public:
    // Properties detected when parsing.
    bool haveNote = false;
    bool have67 = false;
    bool have89 = false;
    bool haveLN = false;
    bool haveMine = false;
    bool haveInvisible = false;
    bool haveBarChange = false;
    bool haveStop = false;
    bool haveBPMChange = false;
    bool haveBGA = false;
    bool haveRandom = false;
    unsigned bgmLayers = 0;
    unsigned long notes = 0;
    unsigned maxMeasure = 0;
    std::array<unsigned, MAXMEASUREIDX + 1> bgmLayersCount{};

public:
    int getMode() const;
    auto getChannel(ChannelCode, unsigned chIdx, unsigned measureIdx) const -> const decltype(chBGM[0][0])&;
    StringPath getDirectory() const;
};
