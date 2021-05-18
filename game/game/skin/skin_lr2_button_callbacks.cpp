

#include "skin_lr2_button_callbacks.h"

#include "game/data/timer.h"
#include "game/data/number.h"
#include "game/data/option.h"
#include "game/data/slider.h"
#include "game/data/switch.h"
#include "game/data/text.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "game/scene/scene_context.h"

#include "config/config_mgr.h"

namespace lr2skin::button
{

#pragma region helpers

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
    // SoundMgr::updateDSP();
}

#pragma region end

#pragma region button type callbacks

// 1 - 9
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

// 20, 21, 22
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

// 23, 24, 25
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

// 26, 27, 28
void fx_target(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // MASTER/KEY/BGM
    int val = (gOptions.get(target) + plus) % 3;
    gOptions.set(target, val);

    update_fx_all();
}

// 29
void eq_switch(int plus)
{
    if (gSwitches.get(eSwitch::SOUND_EQ))
    {
        // close
        gSwitches.set(eSwitch::SOUND_EQ, false);
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SOUND_EQ, true);
    }
    
    update_fx_all();
}

// 31
void vol_switch(int plus)
{
    if (gSwitches.get(eSwitch::SOUND_VOLUME))
    {
        // close
        gSwitches.set(eSwitch::SOUND_VOLUME, false);
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SOUND_VOLUME, true);
    }

    // TODO volume
}

// 32
void pitch_switch(int plus)
{
    if (gSwitches.get(eSwitch::SOUND_PITCH))
    {
        // close
        gSwitches.set(eSwitch::SOUND_PITCH, false);
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SOUND_PITCH, true);
    }

    update_fx_all();
}

// 33
void pitch_type(int plus)
{
    // FREQENCY/PITCH/SPEED
    int val = (gOptions.get(eOption::SOUND_PITCH_TYPE) + plus) % 3;
    gOptions.set(eOption::SOUND_PITCH_TYPE, val);

    if (gSwitches.get(eSwitch::SOUND_PITCH))
    {
        int p = static_cast<int>(std::round((gSliders.get(eSlider::PITCH) - 0.5) * 2 * 12));
        gNumbers.set(eNumber::PITCH, p);

        static const double tick = std::log(2) / std::log(12);
        double f = std::pow(tick, p);
        double f2 = std::pow(tick, -p);
        switch (val)
        {
        case 0: // FREQUENCY
            SoundMgr::setFrequencyFactor(f);
            SoundMgr::setPitch(1.0);
            break;
        case 1: // PITCH
            SoundMgr::setFrequencyFactor(1.0);
            SoundMgr::setPitch(f);
            break;
        case 2: // SPEED (freq up, pitch down)
            SoundMgr::setFrequencyFactor(f);
            SoundMgr::setPitch(f2);
            break;
        default:
            break;
        }
    }
    else
    {
    }

    update_fx_all();
}

// 40, 41
void gauge_type(int player, int plus)
{
    int slot = 0;
    eOption op;
    eText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_1P; op = eOption::PLAY_GAUGE_TYPE_1P; tx = eText::GAUGE_1P; break;
    case 1: slot = PLAYER_SLOT_2P; op = eOption::PLAY_GAUGE_TYPE_2P; tx = eText::GAUGE_2P; break;
    default: return;
    }

    // eModGauge
    int val = (gOptions.get(op) + plus) % 6;
    gOptions.set(op, val);
    switch (val)
    {
    case 0: gPlayContext.mods[slot].gauge = eModGauge::NORMAL; gTexts.set(tx, "NORMAL"); break;
    case 1: gPlayContext.mods[slot].gauge = eModGauge::HARD; gTexts.set(tx, "HARD"); break;
    case 2: gPlayContext.mods[slot].gauge = eModGauge::EASY; gTexts.set(tx, "EASY"); break;
    case 3: gPlayContext.mods[slot].gauge = eModGauge::DEATH; gTexts.set(tx, "DEATH"); break;
    case 4: gPlayContext.mods[slot].gauge = eModGauge::PATTACK; gTexts.set(tx, "P-ATTACK"); break;
    case 5: gPlayContext.mods[slot].gauge = eModGauge::GATTACK; gTexts.set(tx, "G-ATTACK"); break;
    //case 6: gPlayContext.mods[slot].gauge = eModGauge::ASSISTEASY; gTexts.set(tx, "ASSIST-E"); break;
    //case 7: gPlayContext.mods[slot].gauge = eModGauge::EXHARD; break;
    default: break;
    }
}

// 42, 43
void random_type(int player, int plus)
{
    int slot = 0;
    eOption op;
    eText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_1P; op = eOption::PLAY_RANDOM_TYPE_1P; tx = eText::RANDOM_1P; break;
    case 1: slot = PLAYER_SLOT_2P; op = eOption::PLAY_RANDOM_TYPE_2P; tx = eText::RANDOM_2P; break;
    default: return;
    }

    // eModChart
    int val = (gOptions.get(op) + plus) % 6;
    gOptions.set(op, val);
    switch (val)
    {
    case 0: gPlayContext.mods[slot].chart = eModChart::NONE; gTexts.set(tx, "NORMAL"); break;
    case 1: gPlayContext.mods[slot].chart = eModChart::MIRROR; gTexts.set(tx, "MIRROR"); break;
    case 2: gPlayContext.mods[slot].chart = eModChart::RANDOM; gTexts.set(tx, "RANDOM"); break;
    case 3: gPlayContext.mods[slot].chart = eModChart::SRAN; gTexts.set(tx, "S-RANDOM"); break;
    case 4: gPlayContext.mods[slot].chart = eModChart::HRAN; gTexts.set(tx, "H-RANDOM"); break;
    case 5: gPlayContext.mods[slot].chart = eModChart::ALLSCR; gTexts.set(tx, "ALL-SCR"); break;
    default: break;
    }
}

// 44, 45
void autoscr(int player, int plus)
{
    int slot = 0;
    eSwitch sw;
    eText tx;
    switch (player)
    {
    case 0: slot = PLAYER_SLOT_1P; sw = eSwitch::PLAY_OPTION_AUTOSCR_1P; tx = eText::ASSIST_1P; break;
    case 1: slot = PLAYER_SLOT_2P; sw = eSwitch::PLAY_OPTION_AUTOSCR_2P; tx = eText::ASSIST_2P; break;
    default: return;
    }

    // eModChart
    bool val = gPlayContext.mods[slot].assist_mask & PLAY_MOD_ASSIST_AUTOSCR;
    gSwitches.set(sw, !val);
    gPlayContext.mods[slot].assist_mask ^= PLAY_MOD_ASSIST_AUTOSCR;
    gTexts.set(tx, (!val) ? "AUTO-SCR" : "NONE");
}

void shutter(int plus)
{
}

#pragma region end

std::function<void(bool)> getButtonCallback(int type)
{
    using namespace lr2skin::button;
    using namespace std::placeholders;
    switch (type)
    {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        return std::bind(panel_switch, type, _1);

    case 20:
    case 21:
    case 22:
        return std::bind(fx_type, type - 20, _1);

    case 23:
    case 24:
    case 25:
        return std::bind(fx_switch, type - 23, _1);

    case 26:
    case 27:
    case 28:
        return std::bind(fx_target, type - 26, _1);

    case 29:
        return std::bind(eq_switch, _1);

    case 31:
        return std::bind(vol_switch, _1);

    case 32:
        return std::bind(pitch_switch, _1);

    case 33:
        return std::bind(pitch_type, _1);

    case 40:
        return std::bind(gauge_type, 0, _1);
    case 41:
        return std::bind(gauge_type, 1, _1);

    case 42:
        return std::bind(random_type, 0, _1);
    case 43:
        return std::bind(random_type, 1, _1);

    case 44:
        return std::bind(autoscr, 0, _1);
    case 45:
        return std::bind(autoscr, 1, _1);

    case 46:
        return std::bind(shutter, _1);

    default:
        return [](bool) {};
    }
}


}