#pragma once
#include "chart.h"
#include "common/chartformat/chartformat_bms.h"

namespace bms
{
    // 32 -> 40 (9_7_L.bms, 2022-05-04)
    const size_t BGM_LANE_COUNT = 40;

    enum class eNoteExt: unsigned
    {
        BGABASE,
        BGALAYER,
        BGAPOOR,

        STOP,

        EXT_COUNT
    };
};

class chartBMS : public chart::vChart
{
public:
    virtual chart::NoteLaneIndex getLaneFromKey(chart::NoteLaneCategory cat, Input::Pad input);
    virtual std::vector<Input::Pad> getInputFromLane(size_t channel);
    decltype(_specialNoteLists[0])& getBgaBase()  { return _specialNoteLists[(size_t)eNoteExt::BGABASE]; }
    decltype(_specialNoteLists[0])& getBgaLayer() { return _specialNoteLists[(size_t)eNoteExt::BGALAYER]; }
    decltype(_specialNoteLists[0])& getBgaPoor()  { return _specialNoteLists[(size_t)eNoteExt::BGAPOOR]; }
public:
    chartBMS() = delete;
    chartBMS(int slot);
    chartBMS(int slot, std::shared_ptr<BMS> bms);
    chartBMS(int slot, const BMS& bms);
protected:
    void loadBMS(const BMS& bms);

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