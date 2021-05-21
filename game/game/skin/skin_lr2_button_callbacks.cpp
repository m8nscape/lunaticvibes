

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

void number_change(eNumber type, int plus)
{
    gNumbers.set(type, gNumbers.get(type) + plus);

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

void number_change_clamp(eNumber type, int min, int max, int plus)
{
    int val = std::clamp(gNumbers.get(type) + plus, min, max);
    gNumbers.set(type, val);

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
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

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
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

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
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

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 46
void shutter(int plus)
{

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 54
void flip(int plus)
{

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 55
void hs_fix(int plus)
{
    // eModHs
    //OFF/MAXBPM/MINBPM/AVERAGE/CONSTANT
    int val = (gOptions.get(eOption::PLAY_HSFIX_TYPE_1P) + plus) % 5;
    
    gOptions.set(eOption::PLAY_HSFIX_TYPE_1P, val);
    gOptions.set(eOption::PLAY_HSFIX_TYPE_2P, val);

    switch (val)
    {
    case 0: 
        gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::NONE;
        gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::NONE;
        gTexts.set(eText::SCROLL_TYPE, "OFF"); 
        break;
    case 1:
        gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::MAXBPM;
        gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::MAXBPM;
        gTexts.set(eText::SCROLL_TYPE, "MAX");
        break;
    case 2:
        gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::MINBPM;
        gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::MINBPM;
        gTexts.set(eText::SCROLL_TYPE, "MIN");
        break;
    case 3:
        gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::AVERAGE;
        gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::AVERAGE;
        gTexts.set(eText::SCROLL_TYPE, "AVERAGE");
        break;
    case 4:
        gPlayContext.mods[PLAYER_SLOT_1P].hs = eModHs::CONSTANT;
        gPlayContext.mods[PLAYER_SLOT_2P].hs = eModHs::CONSTANT;
        gTexts.set(eText::SCROLL_TYPE, "CONSTANT");
        break;
    default: 
        break;
    }

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 56
void battle(int plus)
{


    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
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
    }
    else
    {
        // open
        gSwitches.set(eSwitch::SYSTEM_SCOREGRAPH, true);
    }

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 71
void ghost_type(int plus)
{
    int val = (gOptions.get(eOption::PLAY_GHOST_TYPE_1P) + plus) % 4;

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

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 72
void bga(int plus)
{
    int val = (gOptions.get(eOption::PLAY_BGA_TYPE) + plus) % 2;

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

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 73
void bga_size(int plus)
{

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 75
void judge_auto_adjust(int plus)
{

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));

}

// 77
void target_type(int plus)
{

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 80
void window_mode(int plus)
{
    int val = (gOptions.get(eOption::SYS_WINDOWED) + plus) % 3;

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

    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 82
void vsync(int plus)
{
    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 83
void save_replay_type(int plus)
{
    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 90
void favorite_ignore(int plus)
{
    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_O_CHANGE));
}

// 91 - 96
void difficulty(int diff, int plus)
{
    gOptions.set(eOption::SELECT_FILTER_DIFF, diff);
    // TODO refresh song list
    SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_DIFFICULTY));
}

#pragma region end

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

    default:
        return [](bool) {};
    }
}


}