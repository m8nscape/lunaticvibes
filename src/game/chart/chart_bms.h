#pragma once
#include "chart.h"
#include "common/chartformat/chartformat_bms.h"

class ChartObjectBMS : public ChartObjectBase
{
public:
    // 32 -> 40 (9_7_L.bms, 2022-05-04)
    static inline const size_t BGM_LANE_COUNT = 40;

protected:
    unsigned _noteCount_scratch;
    unsigned _noteCount_scratch_ln;
public:
    unsigned constexpr getScratchCount() const { return _noteCount_scratch; }
    unsigned constexpr getScratchLnCount() const { return _noteCount_scratch_ln; }

public:
    virtual chart::NoteLaneIndex getLaneFromKey(chart::NoteLaneCategory cat, Input::Pad input);
    virtual std::vector<Input::Pad> getInputFromLane(size_t channel);

    enum class eNoteExt : unsigned
    {
        BGABASE,
        BGALAYER,
        BGAPOOR,

        STOP,

        EXT_COUNT
    };
    decltype(_specialNoteLists[0])& getBgaBase() { return _specialNoteLists[(size_t)eNoteExt::BGABASE]; }
    decltype(_specialNoteLists[0])& getBgaLayer() { return _specialNoteLists[(size_t)eNoteExt::BGALAYER]; }
    decltype(_specialNoteLists[0])& getBgaPoor() { return _specialNoteLists[(size_t)eNoteExt::BGAPOOR]; }

public:
    ChartObjectBMS() = delete;
    ChartObjectBMS(int slot);
    ChartObjectBMS(int slot, std::shared_ptr<ChartFormatBMS> bms);
    ChartObjectBMS(int slot, const ChartFormatBMS& bms);
protected:
    void loadBMS(const ChartFormatBMS& bms);

protected:
    decltype(_specialNoteLists.front().begin()) _currentStopNote;
    bool _inStopNote = false;
    double _stopMetre = 0.0;
    size_t _stopBar = 0;

public:
    //virtual void update(hTime t);
    virtual void preUpdate(const Time& t) override;
    virtual void postUpdate(const Time& t) override;
};
