#pragma once
#include <array>
#include <string>
#include <list>
#include <map>
#include <set>
#include <regex>

#include "chartformat.h"
#include "common/utils.h"
#include "common/types.h"

namespace bms
{
    const unsigned BGMCHANNELS = 32;
    const unsigned MAXSAMPLEIDX = 36 * 36;
    const unsigned MAXBARIDX = 999;
    enum GameMode {
        MODE_5KEYS,
        MODE_7KEYS,
        MODE_9KEYS,
        MODE_10KEYS,
        MODE_14KEYS
    };
    enum class ErrorCode
    {
        OK = 0,
        FILE_ERROR = 1,
        ALREADY_INITIALIZED,
        VALUE_ERROR,
        TYPE_MISMATCH,
        NOTE_LINE_ERROR,
    };
    enum class LaneCode
    {
        BGM = 0,
        BPM,
        EXBPM,
        STOP,
        BGABASE,
        BGALAYER,
        BGAPOOR,
        NOTE1,
        NOTE2,
        NOTEINV1,
        NOTEINV2,
        NOTELN1,
        NOTELN2,
        NOTEMINE1,
        NOTEMINE2,
    };
}

using namespace bms;

class SceneSelect;
class SongDB;

class ChartFormatBMSMeta : public ChartFormatBase
{
public:
    // File properties.
    // Header.
    int player = 0;                // 1: single, 2: couple, 3: double, 4: battle
    int gamemode = 7;               // 5, 7, 9, 10, 14, 24?, 48?
    int rank = 2;                 // judge, VHARD/HARD/NORMAL/EASY
    int total = -1;
    int playLevel = 0;
    int difficulty = 3;            // N/H/A
    double bpm = 130.0;
    std::map<std::string, StringContent> extraCommands;

    // File assigned by the BMS file.
    // Ported to super class

public:
    // Properties detected when parsing.
    bool isPMS = false;
    bool haveNote = false;
    bool haveAny_2 = false;
    bool have67 = false;
    bool have67_2 = false;
    bool have89 = false;
    bool have89_2 = false;
    bool haveLN = false;
    bool haveMine = false;
    bool haveInvisible = false;
    bool haveMetricMod = false;
    bool haveStop = false;
    bool haveBPMChange = false;
    bool haveBGA = false;
    bool haveRandom = false;
    unsigned long notes_total = 0;
    unsigned long notes_key = 0;
    unsigned long notes_scratch = 0;
    unsigned long notes_key_ln = 0;
    unsigned long notes_scratch_ln = 0;
    unsigned long notes_mine = 0;
    unsigned lastBarIdx = 0;

public:
    ChartFormatBMSMeta() { _type = eChartFormat::BMS; }
    virtual ~ChartFormatBMSMeta() = default;
};

// the size of parsing result is kinda large..
class ChartFormatBMS: public ChartFormatBMSMeta
{
    friend class SceneSelect;
    friend class SongDB;

public:
    virtual int getExtendedProperty(const std::string& key, void* ret) override;

public:
    ChartFormatBMS();
    ChartFormatBMS(const Path& absolutePath, uint64_t randomSeed = 0);
    virtual ~ChartFormatBMS() = default;
    std::string getError();
    int initWithPathParam(const SongDB& db, uint64_t randomSeed = 0);

protected:
    int initWithFile(const Path& absolutePath, uint64_t randomSeed = 0);

protected:
    ErrorCode errorCode = ErrorCode::OK;
    int errorLine;

public:
    struct channel {
        struct NoteParseValue
        {
            unsigned segment;
            unsigned value;

            enum Flags
            {
                LN = 1 << 1,
            };
            unsigned flags;
        };
        std::list<NoteParseValue> notes{};
        unsigned resolution = 1;

        unsigned relax(unsigned target_resolution);
        void sortNotes();
    };
    typedef std::array<std::string, MAXSAMPLEIDX + 1> FileIdxArray;
    typedef std::array<channel, MAXBARIDX + 1> LaneArray;
    
protected:
    // Lanes.
    int strToLane36(channel&, const StringContent& str, unsigned flags = 0);
    int strToLane36(channel&, StringContentView str, unsigned flags = 0);
    int strToLane16(channel&, const StringContent& str);
    int strToLane16(channel&, StringContentView str);

    std::vector<LaneArray> chBGM{};
    LaneArray chStop{};
    LaneArray chBPMChange{};
    LaneArray chExBPMChange{};
    LaneArray chBGABase{};
    LaneArray chBGALayer{};
    LaneArray chBGAPoor{};
    
    struct PlayAreaLanes
    {
        static constexpr size_t LANE_COUNT = 10;
        std::array<LaneArray, LANE_COUNT> lanes[2];
    };
    PlayAreaLanes chNotesRegular{};
    PlayAreaLanes chNotesInvisible{};
    PlayAreaLanes chNotesLN{};
    PlayAreaLanes chMines{};

    std::pair<int, int> normalizeIndexesBME(int layer, int ch);
    std::pair<int, int> normalizeIndexesPMS(int layer, int ch);

public:
    std::set<unsigned> lnobjSet;
    bool haveLNchannels = false;

public:
    // Measures related.
    std::array<double, MAXSAMPLEIDX + 1> exBPM{};
    std::array<double, MAXSAMPLEIDX + 1> stop{};

    std::array<unsigned, MAXBARIDX + 1> bgmLayersCount{};

public:
    int getMode() const;
    auto getLane(LaneCode, unsigned chIdx, unsigned measureIdx) const -> const decltype(chBGM[0][0])&;
};
