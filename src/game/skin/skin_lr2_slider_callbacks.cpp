

#include "skin_lr2_slider_callbacks.h"

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

namespace lr2skin::slider
{

#pragma region slider type callbacks

void select_pos(double p)
{
    if (gSelectContext.entries.empty()) return;

    size_t idx_new = (size_t)std::floor(p * gSelectContext.entries.size());
    if (idx_new == gSelectContext.entries.size())
        idx_new = 0;
    if (idx_new != gSelectContext.idx)
    {
        gSelectContext.idx = idx_new;
        gSliders.set(eSlider::SELECT_LIST, (double)idx_new / gSelectContext.entries.size());

        setBarInfo();
        setEntryInfo();
        setDynamicTextures();

        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_SCRATCH);
    }
}

void customize_scrollbar(double p)
{
    gSliders.set(eSlider::SKIN_CONFIG_OPTIONS, p);
    gCustomizeContext.optionDragging = true;
}

void ir_ranking_scrollbar(double p)
{
    // no ir support right now
}

void eq(int idx, double p)
{
    int val = int(p * 24) - 12;
    p = (val + 12) / 24.0;
    gSliders.set(eSlider(idx + (int)eSlider::EQ0), p);
    gNumbers.set(eNumber(idx + (int)eNumber::EQ0), val);

    if (gSwitches.get(eSwitch::SOUND_EQ))
    {
        SoundMgr::setEQ((EQFreq)idx, val);
    }
}

void master_volume(double p)
{
    gSliders.set(eSlider::VOLUME_MASTER, p);
    gNumbers.set(eNumber::VOLUME_MASTER, int(std::round(p * 100)));

    if (gSwitches.get(eSwitch::SOUND_VOLUME))
        SoundMgr::setVolume(SampleChannel::MASTER, (float)p);
}

void key_volume(double p)
{
    gSliders.set(eSlider::VOLUME_KEY, p);
    gNumbers.set(eNumber::VOLUME_KEY, int(std::round(p * 100)));

    if (gSwitches.get(eSwitch::SOUND_VOLUME))
        SoundMgr::setVolume(SampleChannel::KEY, (float)p);
}

void bgm_volume(double p)
{
    gSliders.set(eSlider::VOLUME_BGM, p);
    gNumbers.set(eNumber::VOLUME_BGM, int(std::round(p * 100)));

    if (gSwitches.get(eSwitch::SOUND_VOLUME))
        SoundMgr::setVolume(SampleChannel::BGM, (float)p);
}

void fx0(int idx, double p)
{
    gSliders.set(idx == 0 ? eSlider::FX0_P1 : eSlider::FX0_P2, p);
    gNumbers.set(idx == 0 ? eNumber::FX0_P1 : eNumber::FX0_P2, int(std::round(p * 100)));

    float p1 = (idx == 0) ? (float)p : gSliders.get(eSlider::FX0_P1);
    float p2 = (idx != 0) ? (float)p : gSliders.get(eSlider::FX0_P2);

    if (gSwitches.get(eSwitch::SOUND_FX0))
    {
        SampleChannel ch;
        switch (gOptions.get(eOption::SOUND_TARGET_FX0))
        {
        case 0: ch = SampleChannel::MASTER; break;
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        }
        switch (gOptions.get(eOption::SOUND_FX0))
        {
        case 0: SoundMgr::setDSP(DSPType::OFF, 0, ch, 0.f, 0.f); break;
        case 1: SoundMgr::setDSP(DSPType::REVERB, 0, ch, p1, p2); break;
        case 2: SoundMgr::setDSP(DSPType::DELAY, 0, ch, p1, p2); break;
        case 3: SoundMgr::setDSP(DSPType::LOWPASS, 0, ch, p1, p2); break;
        case 4: SoundMgr::setDSP(DSPType::HIGHPASS, 0, ch, p1, p2); break;
        case 5: SoundMgr::setDSP(DSPType::FLANGER, 0, ch, p1, p2); break;
        case 6: SoundMgr::setDSP(DSPType::CHORUS, 0, ch, p1, p2); break;
        case 7: SoundMgr::setDSP(DSPType::DISTORTION, 0, ch, p1, p2); break;
        }
    }
}

void fx1(int idx, double p)
{
    gSliders.set(idx == 0 ? eSlider::FX1_P1 : eSlider::FX1_P2, p);
    gNumbers.set(idx == 0 ? eNumber::FX1_P1 : eNumber::FX1_P2, int(std::round(p * 100)));

    float p1 = (idx == 0) ? (float)p : gSliders.get(eSlider::FX1_P1);
    float p2 = (idx != 0) ? (float)p : gSliders.get(eSlider::FX1_P2);

    if (gSwitches.get(eSwitch::SOUND_FX1))
    {
        SampleChannel ch;
        switch (gOptions.get(eOption::SOUND_TARGET_FX1))
        {
        case 0: ch = SampleChannel::MASTER; break;
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        }
        switch (gOptions.get(eOption::SOUND_FX1))
        {
        case 0: SoundMgr::setDSP(DSPType::OFF, 1, ch, 0.f, 0.f); break;
        case 1: SoundMgr::setDSP(DSPType::REVERB, 1, ch, p1, p2); break;
        case 2: SoundMgr::setDSP(DSPType::DELAY, 1, ch, p1, p2); break;
        case 3: SoundMgr::setDSP(DSPType::LOWPASS, 1, ch, p1, p2); break;
        case 4: SoundMgr::setDSP(DSPType::HIGHPASS, 1, ch, p1, p2); break;
        case 5: SoundMgr::setDSP(DSPType::FLANGER, 1, ch, p1, p2); break;
        case 6: SoundMgr::setDSP(DSPType::CHORUS, 1, ch, p1, p2); break;
        case 7: SoundMgr::setDSP(DSPType::DISTORTION, 1, ch, p1, p2); break;
        }
    }
}

void fx2(int idx, double p)
{
    gSliders.set(idx == 0 ? eSlider::FX2_P1 : eSlider::FX2_P2, p);
    gNumbers.set(idx == 0 ? eNumber::FX2_P1 : eNumber::FX2_P2, int(std::round(p * 100)));

    float p1 = (idx == 0) ? (float)p : gSliders.get(eSlider::FX2_P1);
    float p2 = (idx != 0) ? (float)p : gSliders.get(eSlider::FX2_P2);

    if (gSwitches.get(eSwitch::SOUND_FX2))
    {
        SampleChannel ch;
        switch (gOptions.get(eOption::SOUND_TARGET_FX2))
        {
        case 0: ch = SampleChannel::MASTER; break;
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        }
        switch (gOptions.get(eOption::SOUND_FX2))
        {
        case 0: SoundMgr::setDSP(DSPType::OFF, 2, ch, 0.f, 0.f); break;
        case 1: SoundMgr::setDSP(DSPType::REVERB, 2, ch, p1, p2); break;
        case 2: SoundMgr::setDSP(DSPType::DELAY, 2, ch, p1, p2); break;
        case 3: SoundMgr::setDSP(DSPType::LOWPASS, 2, ch, p1, p2); break;
        case 4: SoundMgr::setDSP(DSPType::HIGHPASS, 2, ch, p1, p2); break;
        case 5: SoundMgr::setDSP(DSPType::FLANGER, 2, ch, p1, p2); break;
        case 6: SoundMgr::setDSP(DSPType::CHORUS, 2, ch, p1, p2); break;
        case 7: SoundMgr::setDSP(DSPType::DISTORTION, 2, ch, p1, p2); break;
        }
    }
}

void pitch(double p)
{
    int val = int(p * 24) - 12;
    double ps = (val + 12) / 24.0;
    gSliders.set(eSlider::PITCH, ps);
    gNumbers.set(eNumber::PITCH, val);

    if (gSwitches.get(eSwitch::SOUND_PITCH))
    {
        static const double tick = std::pow(2, 1.0 / 12);
        double f = std::pow(tick, val);
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

    auto& [score, lamp] = getSaveScoreType();
    gSwitches.set(eSwitch::CHART_CAN_SAVE_SCORE, score);
    gOptions.set(eOption::CHART_SAVE_LAMP_TYPE, lamp);
}

void deadzone(int type, double p)
{
    using namespace cfg;
    switch (type)
    {
    case 30: ConfigMgr::set('P', P_INPUT_DEADZONE_S1L, p); break;
    case 31: ConfigMgr::set('P', P_INPUT_DEADZONE_S1R, p); break;
    case 32: ConfigMgr::set('P', P_INPUT_DEADZONE_K1Start, p); break;
    case 33: ConfigMgr::set('P', P_INPUT_DEADZONE_K1Select, p); break;
    case 40: ConfigMgr::set('P', P_INPUT_SPEED_S1A, p); break;
    case 41: ConfigMgr::set('P', P_INPUT_DEADZONE_K11, p); break;
    case 42: ConfigMgr::set('P', P_INPUT_DEADZONE_K12, p); break;
    case 43: ConfigMgr::set('P', P_INPUT_DEADZONE_K13, p); break;
    case 44: ConfigMgr::set('P', P_INPUT_DEADZONE_K14, p); break;
    case 45: ConfigMgr::set('P', P_INPUT_DEADZONE_K15, p); break;
    case 46: ConfigMgr::set('P', P_INPUT_DEADZONE_K16, p); break;
    case 47: ConfigMgr::set('P', P_INPUT_DEADZONE_K17, p); break;
    case 48: ConfigMgr::set('P', P_INPUT_DEADZONE_K18, p); break;
    case 49: ConfigMgr::set('P', P_INPUT_DEADZONE_K19, p); break;
    case 35: ConfigMgr::set('P', P_INPUT_DEADZONE_S2L, p); break;
    case 36: ConfigMgr::set('P', P_INPUT_DEADZONE_S2R, p); break;
    case 37: ConfigMgr::set('P', P_INPUT_DEADZONE_K2Start, p); break;
    case 38: ConfigMgr::set('P', P_INPUT_DEADZONE_K2Select, p); break;
    case 50: ConfigMgr::set('P', P_INPUT_SPEED_S2A, p); break;
    case 51: ConfigMgr::set('P', P_INPUT_DEADZONE_K21, p); break;
    case 52: ConfigMgr::set('P', P_INPUT_DEADZONE_K22, p); break;
    case 53: ConfigMgr::set('P', P_INPUT_DEADZONE_K23, p); break;
    case 54: ConfigMgr::set('P', P_INPUT_DEADZONE_K24, p); break;
    case 55: ConfigMgr::set('P', P_INPUT_DEADZONE_K25, p); break;
    case 56: ConfigMgr::set('P', P_INPUT_DEADZONE_K26, p); break;
    case 57: ConfigMgr::set('P', P_INPUT_DEADZONE_K27, p); break;
    case 58: ConfigMgr::set('P', P_INPUT_DEADZONE_K28, p); break;
    case 59: ConfigMgr::set('P', P_INPUT_DEADZONE_K29, p); break;
    }
    InputMgr::updateDeadzones();
}

#pragma endregion

std::function<void(double)> getSliderCallback(int type)
{
    using namespace lr2skin::slider;
    using namespace std::placeholders;
    switch (type)
    {
    case 1:
        return std::bind(select_pos, _1);

    case 7:
        return std::bind(customize_scrollbar, _1);

    case 8:
        return std::bind(ir_ranking_scrollbar, _1);

    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
        return std::bind(eq, type - 10, _1);

    case 17:
        return std::bind(master_volume, _1);
    case 18:
        return std::bind(key_volume, _1);
    case 19:
        return std::bind(bgm_volume, _1);

    case 20:
        return std::bind(fx0, 0, _1);
    case 21:
        return std::bind(fx0, 1, _1);
    case 22:
        return std::bind(fx1, 0, _1);
    case 23:
        return std::bind(fx1, 1, _1);
    case 24:
        return std::bind(fx2, 0, _1);
    case 25:
        return std::bind(fx2, 1, _1);
    case 26:
        return std::bind(pitch, _1);

    case 30:
    case 31:
    case 32:
    case 33:
        return std::bind(deadzone, type, _1);

    default:
        return [](double) {};
    }
}


}