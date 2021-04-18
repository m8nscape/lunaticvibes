

#include "skin_lr2_button_callbacks.h"

#include "game/data/timer.h"
#include "game/data/option.h"
#include "game/data/switch.h"

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
        // TODO play sound
        gSwitches.set(panel, false);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + idx), -1);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + idx), t.norm());
    }
    else
    {
        // open panel
        // TODO play sound
        gSwitches.set(panel, true);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + idx), t.norm());
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + idx), -1);
    }
}


}