

#include "skin_lr2_button_callbacks.h"

#include "game/data/timer.h"
#include "game/data/option.h"
#include "game/data/switch.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

namespace lr2skin::button
{

void panel_switch(int idx)
{
    if (idx < 1 || idx > 9) return;
    eSwitch panel = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + idx);
    Time t{};

    if (gSwitches.get(panel))
    {
        // close panel
        gSwitches.set(panel, false);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + idx), -1);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + idx), t.norm());
        SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CLOSE));
    }
    else
    {
        // open panel
        gSwitches.set(panel, true);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + idx), t.norm());
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + idx), -1);
        SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_OPEN));
    }
}


}