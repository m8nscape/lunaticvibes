#pragma once
#include "scroll.h"
#include "chart/bms.h"

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

class ScrollBMS : public vScroll
{
public:
    virtual std::pair<NoteChannelCategory, NoteChannelIndex> getChannelFromKey(Input::Ingame input);
    virtual std::vector<Input::Ingame> getInputFromChannel(size_t channel);
public:
    ScrollBMS();
    ScrollBMS(const BMS& bms);
protected:
    void loadBMS(const BMS& bms);

public:
    //virtual void update(hTime t);
};