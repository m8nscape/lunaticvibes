#include "common/pch.h"
#include "data_system.h"
#include "game/sound/sound_mgr.h"

namespace lunaticvibes
{

void setFreqModifier(FreqModifierType type, int val)
{
    SystemData.freqType = type;
    SystemData.freqVal = val;

    if (type != FreqModifierType::Off)
    {
        static const double tick = std::pow(2, 1.0 / 12);
        double f = std::pow(tick, val);
        switch (type)
        {
        case FreqModifierType::Frequency: // FREQUENCY
            SoundMgr::setFreqFactor(f);
            SystemData.pitchSpeed = f;
            break;
        case FreqModifierType::PitchOnly: // PITCH
            SoundMgr::setFreqFactor(1.0);
            SoundMgr::setPitch(f);
            SystemData.pitchSpeed = 1.0;
            break;
        case FreqModifierType::SpeedOnly: // SPEED (freq up, pitch down)
            SoundMgr::setFreqFactor(1.0);
            SoundMgr::setSpeed(f);
            SystemData.pitchSpeed = f;
            break;
        default:
            break;
        }
    }
    else
    {
        SoundMgr::setFreqFactor(1.0);
        SystemData.pitchSpeed = 1.0;
    }
}

}