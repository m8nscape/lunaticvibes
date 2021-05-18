

#include "skin_lr2_button_callbacks.h"

#include "game/data/timer.h"
#include "game/data/number.h"
#include "game/data/option.h"
#include "game/data/slider.h"
#include "game/data/switch.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

namespace lr2skin::button
{

void panel_switch(int idx, int plus)
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

std::tuple<eOption, eSwitch, eNumber, eNumber, eSlider, eSlider, eOption, SampleChannel> disp_fx(int idx)
{
    eOption op;
    eSwitch sw;
    eNumber num_p1, num_p2;
    eSlider sli_p1, sli_p2;
    eOption target;
    switch (idx)
    {
    case 0: 
        op = eOption::SOUND_FX0; 
        sw = eSwitch::SOUND_FX0;
        num_p1 = eNumber::FX0_P1;
        num_p2 = eNumber::FX0_P2;
        sli_p1 = eSlider::FX0_P1;
        sli_p2 = eSlider::FX0_P2; 
        target = eOption::SOUND_TARGET_FX0;
        break;
    case 1:
        op = eOption::SOUND_FX1; 
        sw = eSwitch::SOUND_FX1;
        num_p1 = eNumber::FX1_P1;
        num_p2 = eNumber::FX1_P2;
        sli_p1 = eSlider::FX1_P1; 
        sli_p2 = eSlider::FX1_P2; 
        target = eOption::SOUND_TARGET_FX1;
        break;
    case 2: 
        op = eOption::SOUND_FX2;
        sw = eSwitch::SOUND_FX2;
        num_p1 = eNumber::FX2_P1;
        num_p2 = eNumber::FX2_P2;
        sli_p1 = eSlider::FX2_P1; 
        sli_p2 = eSlider::FX2_P2; 
        target = eOption::SOUND_TARGET_FX2;
        break;
    default: return {};
    }

    SampleChannel ch = SampleChannel::MASTER;
    switch (gOptions.get(target))
    {
    case 1: ch = SampleChannel::KEY; break;
    case 2: ch = SampleChannel::BGM; break;
    case 0:
    default: break;
    }

    return { op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch };
}

void update_fx(int type, SampleChannel ch, int p1, int p2)
{
    switch (type)
    {
    case 0: // OFF
        SoundMgr::setDSP(DSPType::OFF, ch, 0, 0);
        break;
    case 1: // REVERB
        SoundMgr::setDSP(DSPType::REVERB, ch, p1, p2);
        break;
    case 2: // DELAY
        SoundMgr::setDSP(DSPType::DELAY, ch, p1, p2);
        break;
    case 3: // LOWPASS
        SoundMgr::setDSP(DSPType::LOWPASS, ch, p1, p2);
        break;
    case 4: // HIGHPASS
        SoundMgr::setDSP(DSPType::HIGHPASS, ch, p1, p2);
        break;
    case 5: // FLANGER
        SoundMgr::setDSP(DSPType::FLANGER, ch, p1, p2);
        break;
    case 6: // CHORUS
        SoundMgr::setDSP(DSPType::CHORUS, ch, p1, p2);
        break;
    case 7: // DISTORTION
        SoundMgr::setDSP(DSPType::DISTORTION, ch, p1, p2);
        break;
    default:
        break;
    }
}

void update_fx_all()
{

}

void fx_type(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // OFF/REVERB/DELAY/LOWPASS/HIGHPASS/FLANGER/CHORUS/DISTORTION/PITCH
    // pitch is unused, remaining 8 options
    int val = (gOptions.get(op) + plus) % 8;
    gOptions.set(op, val);

    if (gSwitches.get(sw))
    {
        int p1 = static_cast<int>(gSliders.get(sli_p1) * 100);
        int p2 = static_cast<int>(gSliders.get(sli_p2) * 100);
        gNumbers.set(num_p1, p1);
        gNumbers.set(num_p2, p2);
        //update_fx(val, ch, p1, p2);
    }
    else
    {
        //update_fx(0, ch, 0, 0);
    }

    update_fx_all();
}

void fx_switch(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    if (gSwitches.get(sw))
    {
        // close
        gSwitches.set(sw, false);
        //update_fx(0, ch, 0, 0);
    }
    else
    {
        // open
        gSwitches.set(sw, true);
        int p1 = static_cast<int>(gSliders.get(sli_p1) * 100);
        int p2 = static_cast<int>(gSliders.get(sli_p2) * 100);
        //update_fx(gOptions.get(op), ch, p1, p2);
    }

    update_fx_all();
}

void fx_target(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // MASTER/KEY/BGM
    int val = (gOptions.get(target) + plus) % 3;
    gOptions.set(target, val);

    update_fx_all();
}

}