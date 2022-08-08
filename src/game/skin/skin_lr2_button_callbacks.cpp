

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

void update_fx(int type, int index, SampleChannel ch, float p1, float p2)
{
    switch (type)
    {
    case 0: // OFF
        SoundMgr::setDSP(DSPType::OFF, index, ch, 0.f, 0.f);
        break;
    case 1: // REVERB
        SoundMgr::setDSP(DSPType::REVERB, index, ch, p1, p2);
        break;
    case 2: // DELAY
        SoundMgr::setDSP(DSPType::DELAY, index, ch, p1, p2);
        break;
    case 3: // LOWPASS
        SoundMgr::setDSP(DSPType::LOWPASS, index, ch, p1, p2);
        break;
    case 4: // HIGHPASS
        SoundMgr::setDSP(DSPType::HIGHPASS, index, ch, p1, p2);
        break;
    case 5: // FLANGER
        SoundMgr::setDSP(DSPType::FLANGER, index, ch, p1, p2);
        break;
    case 6: // CHORUS
        SoundMgr::setDSP(DSPType::CHORUS, index, ch, p1, p2);
        break;
    case 7: // DISTORTION
        SoundMgr::setDSP(DSPType::DISTORTION, index, ch, p1, p2);
        break;
    default:
        break;
    }
}

void update_pitch()
{
    int p = static_cast<int>(std::round((gSliders.get(eSlider::PITCH) - 0.5) * 2 * 12));
    static const double tick = std::pow(2, 1.0 / 12);
    double f = std::pow(tick, p);
    switch (gOptions.get(eOption::SOUND_PITCH_TYPE))
    {
    case 0: // FREQUENCY
        SoundMgr::setFreqFactor(f);
        gSelectContext.pitchSpeed = f;
        break;
    case 1: // PITCH
        SoundMgr::setFreqFactor(1.0);
        SoundMgr::setPitch(f);
        gSelectContext.pitchSpeed = 1.0;
        break;
    case 2: // SPEED (freq up, pitch down)
        SoundMgr::setFreqFactor(1.0);
        SoundMgr::setSpeed(f);
        gSelectContext.pitchSpeed = f;
        break;
    default:
        break;
    }
}

void number_change(eNumber type, int plus)
{
    gNumbers.set(type, gNumbers.get(type) + plus);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

void number_change_clamp(eNumber type, int min, int max, int plus)
{
    int val = std::clamp(gNumbers.get(type) + plus, min, max);
    gNumbers.set(type, val);

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

#pragma endregion

#pragma region button type callbacks

// 1 - 9
void panel_switch(int idx, int plus)
{
    if (idx < 1 || idx > 9) return;
    eSwitch panel = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + idx);
    Time t{};

    // close other panels
    for (int i = 1; i <= 9; ++i)
    {
        if (i == idx) continue;
        eSwitch p = static_cast<eSwitch>(int(eSwitch::SELECT_PANEL1) - 1 + i);
        if (gSwitches.get(p))
        {
            gSwitches.set(p, false);
            gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + i), TIMER_NEVER);
            gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + i), t.norm());
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
        }
    }

    if (gSwitches.get(panel))
    {
        // close panel
        gSwitches.set(panel, false);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + idx), TIMER_NEVER);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + idx), t.norm());
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CLOSE);
    }
    else
    {
        // open panel
        gSwitches.set(panel, true);
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_START) - 1 + idx), t.norm());
        gTimers.set(static_cast<eTimer>(int(eTimer::PANEL1_END) - 1 + idx), TIMER_NEVER);
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_OPEN);
    }
}

// 13
void enter_key_config()
{
    gSelectContext.isGoingToKeyConfig = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 14
void enter_skin_config()
{
    gSelectContext.isGoingToSkinSelect = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 16
void autoplay()
{
    gSelectContext.isGoingToAutoPlay = true;
}

// 19
void replay()
{
    gSelectContext.isGoingToReplay = true;
}

// 20, 21, 22
void fx_type(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // OFF/REVERB/DELAY/LOWPASS/HIGHPASS/FLANGER/CHORUS/DISTORTION/PITCH
    // pitch is unused, remaining 8 options
    int val = (gOptions.get(op) + 8 + plus) % 8;
    gOptions.set(op, val);

    if (gSwitches.get(sw))
    {
        float p1 = float(gSliders.get(sli_p1));
        float p2 = float(gSliders.get(sli_p2));
        gNumbers.set(num_p1, static_cast<int>(p1 * 100));
        gNumbers.set(num_p2, static_cast<int>(p2 * 100));
        update_fx(val, idx, ch, p1, p2);
    }
    else
    {
        update_fx(0, idx, ch, 0.f, 0.f);
    }
}

// 23, 24, 25
void fx_switch(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    if (gSwitches.get(sw))
    {
        // button clicked, close fx
        gSwitches.set(sw, false);
        update_fx(0, idx, ch, 0.f, 0.f);
    }
    else
    {
        // button clicked, open fx
        gSwitches.set(sw, true);
        float p1 = float(gSliders.get(sli_p1));
        float p2 = float(gSliders.get(sli_p2));
        gNumbers.set(num_p1, static_cast<int>(p1 * 100));
        gNumbers.set(num_p2, static_cast<int>(p2 * 100));
        update_fx(gOptions.get(op), idx, ch, p1, p2);
    }
}

// 26, 27, 28
void fx_target(int idx, int plus)
{
    if (idx < 0 || idx > 2) return;
    auto [op, sw, num_p1, num_p2, sli_p1, sli_p2, target, ch] = disp_fx(idx);

    // MASTER/KEY/BGM
    int val = (gOptions.get(target) + 3 + plus) % 3;
    gOptions.set(target, val);

    if (gSwitches.get(sw))
    {
        // target is modified, thus we should get ch once again
        switch (gOptions.get(target))
        {
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        case 0: ch = SampleChannel::MASTER; break;
        default: break;
        }

        float p1 = float(gSliders.get(sli_p1));
        float p2 = float(gSliders.get(sli_p2));
        gNumbers.set(num_p1, static_cast<int>(p1 * 100));
        gNumbers.set(num_p2, static_cast<int>(p2 * 100));
        update_fx(gOptions.get(op), idx, ch, p1, p2);
    }
}

// 29
void eq_switch(int plus)
{
    if (gSwitches.get(eSwitch::SOUND_EQ))
    {
        // close
        gSwitches.set(eSwitch::SOUND_EQ, false);

        for (int idx = 0; idx < 7; ++idx)
        {
            SoundMgr::setEQ((EQFreq)idx, 0);
        }
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SOUND_EQ, true);

        for (int idx = 0; idx < 7; ++idx)
        {
            int val = gNumbers.get(eNumber(idx + (int)eNumber::EQ0));
            SoundMgr::setEQ((EQFreq)idx, val);
        }
    }
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
        SoundMgr::setFreqFactor(1.0);
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SOUND_PITCH, true);
        update_pitch();
    }
}

// 33
void pitch_type(int plus)
{
    // FREQENCY/PITCH/SPEED
    int val = (gOptions.get(eOption::SOUND_PITCH_TYPE) + 3 + plus) % 3;
    gOptions.set(eOption::SOUND_PITCH_TYPE, val);

    if (gSwitches.get(eSwitch::SOUND_PITCH))
    {
        int p = static_cast<int>(std::round((gSliders.get(eSlider::PITCH) - 0.5) * 2 * 12));
        gNumbers.set(eNumber::PITCH, p);
        update_pitch();
    }
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
    int val = (gOptions.get(op) + plus) % 4;
    gOptions.set(op, val);
    switch (val)
    {
    case 0: gTexts.set(tx, "NORMAL"); break;
    case 1: gTexts.set(tx, "HARD"); break;
    case 2: gTexts.set(tx, "DEATH"); break;
    case 3: gTexts.set(tx, "EASY"); break;
    //case 4: gTexts.set(tx, "P-ATTACK"); break;
    //case 5: gTexts.set(tx, "G-ATTACK"); break;
    //case 6: gTexts.set(tx, "ASSIST-E"); break;
    //case 7: break;
    default: break;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
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
    int val = (gOptions.get(op) + 6 + plus) % 6;
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

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
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
    bool val = gSwitches.get(sw);
    gSwitches.set(sw, !val);
    gTexts.set(tx, (!val) ? "AUTO-SCR" : "NONE");

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 46
void shutter(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 54
void flip(int plus)
{
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 55
void hs_fix(int plus)
{
    // eModHs
    //OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT
    int val = (gOptions.get(eOption::PLAY_HSFIX_TYPE_1P) + 5 + plus) % 5;
    
    gOptions.set(eOption::PLAY_HSFIX_TYPE_1P, val);
    gOptions.set(eOption::PLAY_HSFIX_TYPE_2P, val);

    switch (val)
    {
    case 0: 
        gTexts.set(eText::SCROLL_TYPE, "OFF"); 
        break;
    case 1:
        gTexts.set(eText::SCROLL_TYPE, "MAX");
        break;
    case 2:
        gTexts.set(eText::SCROLL_TYPE, "MIN");
        break;
    case 3:
        gTexts.set(eText::SCROLL_TYPE, "AVERAGE");
        break;
    case 4:
        gTexts.set(eText::SCROLL_TYPE, "CONSTANT");
        break;
    default: 
        break;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 56
void battle(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 57, 58
void hs(int player, int plus)
{
    // TODO margin, min/max
    plus = plus > 0 ? 25 : -25;
    switch (player)
    {
    case 0: number_change_clamp(eNumber::HS_1P, 50, 1000, plus); break;
    case 1: number_change_clamp(eNumber::HS_2P, 50, 1000, plus); break;
    default: break;
    }
}

// 70
void score_graph(int plus)
{
    if (gSwitches.get(eSwitch::SYSTEM_SCOREGRAPH))
    {
        // close
        gSwitches.set(eSwitch::SYSTEM_SCOREGRAPH, false);
        gTexts.set(eText::SCORE_GRAPH, "OFF");
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SYSTEM_SCOREGRAPH, true);
        gTexts.set(eText::SCORE_GRAPH, "ON");
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 71
void ghost_type(int plus)
{
    int val = (gOptions.get(eOption::PLAY_GHOST_TYPE_1P) + 4 + plus) % 4;

    gOptions.set(eOption::PLAY_GHOST_TYPE_1P, val);
    gOptions.set(eOption::PLAY_GHOST_TYPE_2P, val);

    switch (val)
    {
    case 0:
        gTexts.set(eText::GHOST, "OFF");
        break;
    case 1:
        gTexts.set(eText::GHOST, "TYPE A");
        break;
    case 2:
        gTexts.set(eText::GHOST, "TYPE B");
        break;
    case 3:
        gTexts.set(eText::GHOST, "TYPE C");
        break;
    default:
        break;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 72
void bga(int plus)
{
    int val = (gOptions.get(eOption::PLAY_BGA_TYPE) + 2 + plus) % 2;

    gOptions.set(eOption::PLAY_BGA_TYPE, val);

    switch (val)
    {
    case 0:
        gTexts.set(eText::BGA, "OFF");
        break;
    case 1:
        gTexts.set(eText::BGA, "ON");
        break;
    //case 2:
    //    gTexts.set(eText::BGA, "AUTOPLAY");
    //    break;
    default:
        break;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 73
void bga_size(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 75
void judge_auto_adjust(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);

}

// 77
void target_type(int plus)
{

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 80
void window_mode(int plus)
{
    int val = (gOptions.get(eOption::SYS_WINDOWED) + 3 + plus) % 3;

    gOptions.set(eOption::SYS_WINDOWED, val);

    // TODO recreate window
    switch (val)
    {
    case Option::WIN_FULLSCREEN: 
        gTexts.set(eText::WINDOWMODE, "FULLSCREEN");
        break;
    case Option::WIN_BORDERLESS:
        gTexts.set(eText::WINDOWMODE, "BORDERLESS");
        break;
    case Option::WIN_WINDOWED:
        gTexts.set(eText::WINDOWMODE, "WINDOWED");
        break;
    default:
        break;
    }

    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 82
void vsync(int plus)
{
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 83
void save_replay_type(int plus)
{
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 90
void favorite_ignore(int plus)
{
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 91 - 96
void difficulty(int diff, int plus)
{
    gOptions.set(eOption::SELECT_FILTER_DIFF, diff);
    // TODO refresh song list
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_DIFFICULTY);
}

// 100 - 139
void key_config_pad(Input::Pad pad)
{
    auto& sel = gKeyconfigContext.selecting;
    auto old = sel.first;
    if (old != pad)
    {
        auto setSwitch = [](Input::Pad pad, bool sw)
        {
            switch (pad)
            {
            case Input::Pad::K11:      gSwitches.set(eSwitch::K11_CONFIG, sw); break;
            case Input::Pad::K12:      gSwitches.set(eSwitch::K12_CONFIG, sw); break;
            case Input::Pad::K13:      gSwitches.set(eSwitch::K13_CONFIG, sw); break;
            case Input::Pad::K14:      gSwitches.set(eSwitch::K14_CONFIG, sw); break;
            case Input::Pad::K15:      gSwitches.set(eSwitch::K15_CONFIG, sw); break;
            case Input::Pad::K16:      gSwitches.set(eSwitch::K16_CONFIG, sw); break;
            case Input::Pad::K17:      gSwitches.set(eSwitch::K17_CONFIG, sw); break;
            case Input::Pad::K18:      gSwitches.set(eSwitch::K18_CONFIG, sw); break;
            case Input::Pad::K19:      gSwitches.set(eSwitch::K19_CONFIG, sw); break;
            case Input::Pad::S1L:      gSwitches.set(eSwitch::S1L_CONFIG, sw); break;
            case Input::Pad::S1R:      gSwitches.set(eSwitch::S1R_CONFIG, sw); break;
            case Input::Pad::K1START:  gSwitches.set(eSwitch::K1START_CONFIG, sw); break;
            case Input::Pad::K1SELECT: gSwitches.set(eSwitch::K1SELECT_CONFIG, sw); break;
            case Input::Pad::K1SPDUP:  gSwitches.set(eSwitch::K1SPDUP_CONFIG, sw); break;
            case Input::Pad::K1SPDDN:  gSwitches.set(eSwitch::K1SPDDN_CONFIG, sw); break;
            case Input::Pad::K21:      gSwitches.set(eSwitch::K21_CONFIG, sw); break;
            case Input::Pad::K22:      gSwitches.set(eSwitch::K22_CONFIG, sw); break;
            case Input::Pad::K23:      gSwitches.set(eSwitch::K23_CONFIG, sw); break;
            case Input::Pad::K24:      gSwitches.set(eSwitch::K24_CONFIG, sw); break;
            case Input::Pad::K25:      gSwitches.set(eSwitch::K25_CONFIG, sw); break;
            case Input::Pad::K26:      gSwitches.set(eSwitch::K26_CONFIG, sw); break;
            case Input::Pad::K27:      gSwitches.set(eSwitch::K27_CONFIG, sw); break;
            case Input::Pad::K28:      gSwitches.set(eSwitch::K28_CONFIG, sw); break;
            case Input::Pad::K29:      gSwitches.set(eSwitch::K29_CONFIG, sw); break;
            case Input::Pad::S2L:      gSwitches.set(eSwitch::S2L_CONFIG, sw); break;
            case Input::Pad::S2R:      gSwitches.set(eSwitch::S2R_CONFIG, sw); break;
            case Input::Pad::K2START:  gSwitches.set(eSwitch::K2START_CONFIG, sw); break;
            case Input::Pad::K2SELECT: gSwitches.set(eSwitch::K2SELECT_CONFIG, sw); break;
            case Input::Pad::K2SPDUP:  gSwitches.set(eSwitch::K2SPDUP_CONFIG, sw); break;
            case Input::Pad::K2SPDDN:  gSwitches.set(eSwitch::K2SPDDN_CONFIG, sw); break;
            default: break;
            }
        };
        auto setOption = [](Input::Pad pad)
        {
            int idx = 0;
            switch (gKeyconfigContext.keys)
            {
            case 5:
                switch (pad)
                {
                case Input::Pad::K11:      idx = 1; break;
                case Input::Pad::K12:      idx = 2; break;
                case Input::Pad::K13:      idx = 3; break;
                case Input::Pad::K14:      idx = 4; break;
                case Input::Pad::K15:      idx = 5; break;
                case Input::Pad::S1L:      idx = 6; break;
                case Input::Pad::S1R:      idx = 7; break;
                case Input::Pad::K1START:  idx = 8; break;
                case Input::Pad::K1SELECT: idx = 9; break;
                case Input::Pad::K21:      idx = 10; break;
                case Input::Pad::K22:      idx = 11; break;
                case Input::Pad::K23:      idx = 12; break;
                case Input::Pad::K24:      idx = 13; break;
                case Input::Pad::K25:      idx = 14; break;
                case Input::Pad::S2L:      idx = 15; break;
                case Input::Pad::S2R:      idx = 16; break;
                case Input::Pad::K2START:  idx = 17; break;
                case Input::Pad::K2SELECT: idx = 18; break;
                default: break;
                }
                gOptions.set(eOption::KEY_CONFIG_KEY5, idx);
                break;
            case 7:
                switch (pad)
                {
                case Input::Pad::K11:      idx = 1; break;
                case Input::Pad::K12:      idx = 2; break;
                case Input::Pad::K13:      idx = 3; break;
                case Input::Pad::K14:      idx = 4; break;
                case Input::Pad::K15:      idx = 5; break;
                case Input::Pad::K16:      idx = 6; break;
                case Input::Pad::K17:      idx = 7; break;
                case Input::Pad::S1L:      idx = 8; break;
                case Input::Pad::S1R:      idx = 9; break;
                case Input::Pad::K1START:  idx = 10; break;
                case Input::Pad::K1SELECT: idx = 11; break;
                case Input::Pad::K21:      idx = 12; break;
                case Input::Pad::K22:      idx = 13; break;
                case Input::Pad::K23:      idx = 14; break;
                case Input::Pad::K24:      idx = 15; break;
                case Input::Pad::K25:      idx = 16; break;
                case Input::Pad::K26:      idx = 17; break;
                case Input::Pad::K27:      idx = 18; break;
                case Input::Pad::S2L:      idx = 19; break;
                case Input::Pad::S2R:      idx = 20; break;
                case Input::Pad::K2START:  idx = 21; break;
                case Input::Pad::K2SELECT: idx = 22; break;
                default: break;
                }
                gOptions.set(eOption::KEY_CONFIG_KEY7, idx);
                break;
            case 9:
                switch (pad)
                {
                case Input::Pad::K11:      idx = 1; break;
                case Input::Pad::K12:      idx = 2; break;
                case Input::Pad::K13:      idx = 3; break;
                case Input::Pad::K14:      idx = 4; break;
                case Input::Pad::K15:      idx = 5; break;
                case Input::Pad::K16:      idx = 6; break;
                case Input::Pad::K17:      idx = 7; break;
                case Input::Pad::K18:      idx = 8; break;
                case Input::Pad::K19:      idx = 9; break;
                case Input::Pad::K1START:  idx = 10; break;
                case Input::Pad::K1SELECT: idx = 11; break;
                default: break;
                }
                gOptions.set(eOption::KEY_CONFIG_KEY9, idx);
                break;
            default:
                break;
            }
        };
        setSwitch(old, false);
        setSwitch(pad, true);
        setOption(pad);

        sel.first = pad;

        auto bindings = ConfigMgr::Input(gKeyconfigContext.keys)->getBindings(pad);
        for (size_t i = 0; i < bindings.size(); ++i)
        {
            gTexts.set(eText(unsigned(eText::KEYCONFIG_SLOT1) + i), bindings[i].toString());
        }
        for (size_t i = bindings.size(); i < InputMgr::MAX_BINDINGS_PER_KEY; ++i)
        {
            gTexts.set(eText(unsigned(eText::KEYCONFIG_SLOT1) + i), "-");
        }
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

// 143
void key_config_mode_rotate()
{
    auto& keys = gKeyconfigContext.keys;
    switch (keys)
    {
    case 7: keys = 9; break;
    case 9: keys = 5; break;
    case 5: keys = 7; break;
    default: keys = 7; break;
    }
    switch (keys)
    {
    case 7: gOptions.set(eOption::KEY_CONFIG_MODE, Option::KEYCFG_7); break;
    case 9: gOptions.set(eOption::KEY_CONFIG_MODE, Option::KEYCFG_9); break;
    case 5: gOptions.set(eOption::KEY_CONFIG_MODE, Option::KEYCFG_5); break;
    default: return;
    }

    key_config_pad(Input::Pad::K11);
}

// 150 - 159
void key_config_slot(int slot)
{
    auto& sel = gKeyconfigContext.selecting;
    auto old = sel.second;
    if (old != slot)
    {
        auto setSwitch = [](int slot, bool sw)
        {
            switch (slot)
            {
            case 0:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT0, sw); break;
            case 1:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT1, sw); break;
            case 2:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT2, sw); break;
            case 3:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT3, sw); break;
            case 4:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT4, sw); break;
            case 5:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT5, sw); break;
            case 6:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT6, sw); break;
            case 7:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT7, sw); break;
            case 8:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT8, sw); break;
            case 9:      gSwitches.set(eSwitch::KEY_CONFIG_SLOT9, sw); break;
            default: break;
            }
        };
        setSwitch(old, false);
        setSwitch(slot, true);
        sel.second = slot;
    }
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}


void skinselect_mode(int mode)
{
    eMode modeOld = gCustomizeContext.mode;
    switch (mode)
    {
    case 0: gCustomizeContext.mode = eMode::PLAY7; break;
    case 1: gCustomizeContext.mode = eMode::PLAY5; break;
    case 2: gCustomizeContext.mode = eMode::PLAY14; break;
    case 3: gCustomizeContext.mode = eMode::PLAY10; break;
    case 4: gCustomizeContext.mode = eMode::PLAY9; break;
    case 5: gCustomizeContext.mode = eMode::MUSIC_SELECT; break;
    case 6: gCustomizeContext.mode = eMode::DECIDE; break;
    case 7: gCustomizeContext.mode = eMode::RESULT; break;
    case 8: gCustomizeContext.mode = eMode::KEY_CONFIG; break;
    case 9: gCustomizeContext.mode = eMode::THEME_SELECT; break;
    case 10: gCustomizeContext.mode = eMode::SOUNDSET; break;
    case 11: break;
    case 12: gCustomizeContext.mode = eMode::PLAY7_2; break;
    case 13: gCustomizeContext.mode = eMode::PLAY5_2; break;
    case 14: gCustomizeContext.mode = eMode::PLAY9_2; break;
    case 15: gCustomizeContext.mode = eMode::COURSE_RESULT; break;
    default: break;
    }

    if (modeOld != gCustomizeContext.mode)
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

void skinselect_skin(int plus)
{
    gCustomizeContext.skinDir = plus;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

void skinselect_option(int index, int plus)
{
    gCustomizeContext.optionIdx = index;
    gCustomizeContext.optionDir = plus;
    gCustomizeContext.optionUpdate = true;
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}

#pragma endregion

std::function<void(int)> getButtonCallback(int type)
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

    case 13:
        return std::bind(enter_key_config);

    case 14:
        return std::bind(enter_skin_config);

    case 16:
        return std::bind(autoplay);

    case 19:
        return std::bind(replay);

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

    case 54:
        return std::bind(flip, _1);

    case 55: 
        return std::bind(hs_fix, _1);

    case 56:
        return std::bind(battle, _1);

    case 57:
        return std::bind(hs, 0, _1);
    case 58:
        return std::bind(hs, 1, _1);

    case 70:
        return std::bind(score_graph, _1);

    case 71:
        return std::bind(ghost_type, _1);

    case 72:
        return std::bind(bga, _1);

    case 73:
        return std::bind(bga_size, _1);

    case 74:
        return std::bind(number_change_clamp, eNumber::TIMING_ADJUST_VISUAL, -99, 99, _1);

    case 75:
        return std::bind(judge_auto_adjust, _1);

    case 76:
        return std::bind(number_change_clamp, eNumber::DEFAULT_TARGET_RATE, 0, 100, _1);

    case 77:
        return std::bind(target_type, _1);

    case 80:
        return std::bind(window_mode, _1);

    case 82:
        return std::bind(vsync, _1);

    case 83:
        return std::bind(save_replay_type, _1);

    case 90:
        return std::bind(favorite_ignore, _1);

    case 91:
    case 92:
    case 93:
    case 94:
    case 95:
    case 96:
        return std::bind(difficulty, type - 91, _1);

    case 101:
    case 102:
    case 103:
    case 104:
    case 105:
    case 106:
    case 107:
    case 108:
    case 109:
        return std::bind(key_config_pad, Input::Pad(unsigned(Input::Pad::K11) + type - 101));

    case 110:
        return std::bind(key_config_pad, Input::Pad::S1L);
    case 111:
        return std::bind(key_config_pad, Input::Pad::S1R);
    case 112:
        return std::bind(key_config_pad, Input::Pad::K1START);
    case 113:
        return std::bind(key_config_pad, Input::Pad::K1SELECT);

    case 121:
    case 122:
    case 123:
    case 124:
    case 125:
    case 126:
    case 127:
    case 128:
    case 129:
        return std::bind(key_config_pad, Input::Pad(unsigned(Input::Pad::K21) + type - 121));

    case 130:
        return std::bind(key_config_pad, Input::Pad::S2L);
    case 131:
        return std::bind(key_config_pad, Input::Pad::S2R);
    case 132:
        return std::bind(key_config_pad, Input::Pad::K2START);
    case 133:
        return std::bind(key_config_pad, Input::Pad::K2SELECT);

    case 143:
        return std::bind(key_config_mode_rotate);

    case 150:
    case 151:
    case 152:
    case 153:
    case 154:
    case 155:
    case 156:
    case 157:
    case 158:
    case 159:
        return std::bind(key_config_slot, type - 150);

    case 170:
    case 171:
    case 172:
    case 173:
    case 174:
    case 175:
    case 176:
    case 177:
    case 178:
    case 179:
    case 180:
    case 181:
    case 182:
    case 183:
    case 184:
    case 185:
        return std::bind(skinselect_mode, type - 170);

    case 190:
        return std::bind(skinselect_skin, _1);

    case 220:
    case 221:
    case 222:
    case 223:
    case 224:
    case 225:
    case 226:
    case 227:
    case 228:
    case 229:
        return std::bind(skinselect_option, type - 220, _1);

    default:
        return [](bool) {};
    }
}


}