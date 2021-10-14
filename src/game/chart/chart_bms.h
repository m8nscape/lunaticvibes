#pragma once
#include "chart.h"
#include "common/chartformat/chartformat_bms.h"

namespace bms
{
    const size_t BGM_LANE_COUNT = 32;

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
    virtual chart::NoteLane getLaneFromKey(Input::Pad input);
    virtual std::vector<Input::Pad> getInputFromLane(size_t channel);
    decltype(_specialNoteLists[0])& getBgaBase()  { return _specialNoteLists[(size_t)eNoteExt::BGABASE]; }
    decltype(_specialNoteLists[0])& getBgaLayer() { return _specialNoteLists[(size_t)eNoteExt::BGALAYER]; }
    decltype(_specialNoteLists[0])& getBgaPoor()  { return _specialNoteLists[(size_t)eNoteExt::BGAPOOR]; }
public:
    chartBMS();
    chartBMS(std::shared_ptr<BMS> bms);
    chartBMS(const BMS& bms);
protected:
    void loadBMS(const BMS& bms);

protected:
    double   _currentStopBeat = 0;
    bool     _currentStopBeatGuard = false;

public:
    //virtual void update(hTime t);
    virtual void preUpdate(const Time& t) override;
    virtual void postUpdate(const Time& t) override;
};