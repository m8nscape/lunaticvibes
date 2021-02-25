#pragma once
#include "chart.h"
#include "chartformat/format_bms.h"

namespace bms
{
    enum class eNotePlain: unsigned
    {
		BGM0 = 0,
        BGM1,
        BGM2,
        BGM3,
        BGM4,
        BGM5,
        BGM6,
        BGM7,
        BGM8,
        BGM9,
        BGM10,
        BGM11,
        BGM12,
        BGM13,
        BGM14,
        BGM15,
        BGM16,
        BGM17,
        BGM18,
        BGM19,
        BGM20,
        BGM21,
        BGM22,
        BGM23,
        BGM24,
        BGM25,
        BGM26,
        BGM27,
        BGM28,
        BGM29,
        BGM30,
        BGM31,

		BGABASE,
		BGALAYER,
		BGAPOOR,

        PLAIN_COUNT
    };

    enum class eNoteExt: unsigned
    {

        EXT_COUNT
    };
};

class chartBMS : public vChart
{
public:
    virtual std::pair<NoteLaneCategory, NoteLaneIndex> getLaneFromKey(Input::Ingame input);
    virtual std::vector<Input::Ingame> getInputFromLane(size_t channel);
    decltype(_commonNoteLists[0])& getBgaBase() { return _commonNoteLists[(size_t)eNotePlain::BGABASE]; }
    decltype(_commonNoteLists[0])& getBgaLayer() { return _commonNoteLists[(size_t)eNotePlain::BGALAYER]; }
    decltype(_commonNoteLists[0])& getBgaPoor() { return _commonNoteLists[(size_t)eNotePlain::BGAPOOR]; }
public:
    chartBMS();
    chartBMS(std::shared_ptr<BMS> bms);
    chartBMS(const BMS& bms);
protected:
    void loadBMS(const BMS& bms);

public:
    //virtual void update(hTime t);
};