

#include "skin_lr2_slider_callbacks.h"

#include "game/runtime/state.h"

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
    if (idx_new != gSelectContext.selectedEntryIndex)
    {
        State::set(IndexSlider::SELECT_LIST, (double)idx_new / gSelectContext.entries.size());
        gSelectContext.draggingListSlider = true;
    }
}

void customize_scrollbar(double p)
{
    State::set(IndexSlider::SKIN_CONFIG_OPTIONS, p);
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
    State::set(IndexSlider(idx + (int)IndexSlider::EQ0), p);
    State::set(IndexNumber(idx + (int)IndexNumber::EQ0), val);

    if (State::get(IndexSwitch::SOUND_EQ))
    {
        SoundMgr::setEQ((EQFreq)idx, val);
    }
}

void master_volume(double p)
{
    State::set(IndexSlider::VOLUME_MASTER, p);
    State::set(IndexNumber::VOLUME_MASTER, int(std::round(p * 100)));

    if (State::get(IndexSwitch::SOUND_VOLUME))
        SoundMgr::setVolume(SampleChannel::MASTER, (float)p);
}

void key_volume(double p)
{
    State::set(IndexSlider::VOLUME_KEY, p);
    State::set(IndexNumber::VOLUME_KEY, int(std::round(p * 100)));

    if (State::get(IndexSwitch::SOUND_VOLUME))
        SoundMgr::setVolume(SampleChannel::KEY, (float)p);
}

void bgm_volume(double p)
{
    State::set(IndexSlider::VOLUME_BGM, p);
    State::set(IndexNumber::VOLUME_BGM, int(std::round(p * 100)));

    if (State::get(IndexSwitch::SOUND_VOLUME))
        SoundMgr::setVolume(SampleChannel::BGM, (float)p);
}

void fx0(int idx, double p)
{
    State::set(idx == 0 ? IndexSlider::FX0_P1 : IndexSlider::FX0_P2, p);
    State::set(idx == 0 ? IndexNumber::FX0_P1 : IndexNumber::FX0_P2, int(std::round(p * 100)));

    float p1 = (idx == 0) ? (float)p : State::get(IndexSlider::FX0_P1);
    float p2 = (idx != 0) ? (float)p : State::get(IndexSlider::FX0_P2);

    if (State::get(IndexSwitch::SOUND_FX0))
    {
        SampleChannel ch;
        switch (State::get(IndexOption::SOUND_TARGET_FX0))
        {
        case 0: ch = SampleChannel::MASTER; break;
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        }
        switch (State::get(IndexOption::SOUND_FX0))
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
    State::set(idx == 0 ? IndexSlider::FX1_P1 : IndexSlider::FX1_P2, p);
    State::set(idx == 0 ? IndexNumber::FX1_P1 : IndexNumber::FX1_P2, int(std::round(p * 100)));

    float p1 = (idx == 0) ? (float)p : State::get(IndexSlider::FX1_P1);
    float p2 = (idx != 0) ? (float)p : State::get(IndexSlider::FX1_P2);

    if (State::get(IndexSwitch::SOUND_FX1))
    {
        SampleChannel ch;
        switch (State::get(IndexOption::SOUND_TARGET_FX1))
        {
        case 0: ch = SampleChannel::MASTER; break;
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        }
        switch (State::get(IndexOption::SOUND_FX1))
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
    State::set(idx == 0 ? IndexSlider::FX2_P1 : IndexSlider::FX2_P2, p);
    State::set(idx == 0 ? IndexNumber::FX2_P1 : IndexNumber::FX2_P2, int(std::round(p * 100)));

    float p1 = (idx == 0) ? (float)p : State::get(IndexSlider::FX2_P1);
    float p2 = (idx != 0) ? (float)p : State::get(IndexSlider::FX2_P2);

    if (State::get(IndexSwitch::SOUND_FX2))
    {
        SampleChannel ch;
        switch (State::get(IndexOption::SOUND_TARGET_FX2))
        {
        case 0: ch = SampleChannel::MASTER; break;
        case 1: ch = SampleChannel::KEY; break;
        case 2: ch = SampleChannel::BGM; break;
        }
        switch (State::get(IndexOption::SOUND_FX2))
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
    State::set(IndexSlider::PITCH, ps);
    State::set(IndexNumber::PITCH, val);

    if (State::get(IndexSwitch::SOUND_PITCH))
    {
        static const double tick = std::pow(2, 1.0 / 12);
        double f = std::pow(tick, val);
        switch (State::get(IndexOption::SOUND_PITCH_TYPE))
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

    const auto [score, lamp] = getSaveScoreType();
    State::set(IndexSwitch::CHART_CAN_SAVE_SCORE, score);
    State::set(IndexOption::CHART_SAVE_LAMP_TYPE, lamp);
}

void deadzone(int type, double p)
{
    using namespace cfg;
    int keys = 7;
    switch (State::get(IndexOption::KEY_CONFIG_MODE))
    {
    case Option::KEYCFG_5: keys = 5; break;
    case Option::KEYCFG_7: keys = 7; break;
    case Option::KEYCFG_9: keys = 9; break; 
    default: return;
    }
    switch (type)
    {
    case 31: State::set(IndexSlider::DEADZONE_K11, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K11, p); break;
    case 32: State::set(IndexSlider::DEADZONE_K12, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K12, p); break;
    case 33: State::set(IndexSlider::DEADZONE_K13, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K13, p); break;
    case 34: State::set(IndexSlider::DEADZONE_K14, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K14, p); break;
    case 35: State::set(IndexSlider::DEADZONE_K15, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K15, p); break;
    case 36: State::set(IndexSlider::DEADZONE_K16, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K16, p); break;
    case 37: State::set(IndexSlider::DEADZONE_K17, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K17, p); break;
    case 38: State::set(IndexSlider::DEADZONE_K18, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K18, p); break;
    case 39: State::set(IndexSlider::DEADZONE_K19, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K19, p); break;
    case 40: State::set(IndexSlider::DEADZONE_K1Start, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K1Start, p); break;
    case 41: State::set(IndexSlider::DEADZONE_K1Select, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K1Select, p); break;
    case 42: State::set(IndexSlider::DEADZONE_S1L, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S1L, p); break;
    case 43: State::set(IndexSlider::DEADZONE_S1R, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S1R, p); break;
    case 44: State::set(IndexSlider::SPEED_S1A, p); ConfigMgr::Input(keys)->set(I_INPUT_SPEED_S1A, p); break;
    case 51: State::set(IndexSlider::DEADZONE_K21, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K21, p); break;
    case 52: State::set(IndexSlider::DEADZONE_K22, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K22, p); break;
    case 53: State::set(IndexSlider::DEADZONE_K23, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K23, p); break;
    case 54: State::set(IndexSlider::DEADZONE_K24, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K24, p); break;
    case 55: State::set(IndexSlider::DEADZONE_K25, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K25, p); break;
    case 56: State::set(IndexSlider::DEADZONE_K26, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K26, p); break;
    case 57: State::set(IndexSlider::DEADZONE_K27, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K27, p); break;
    case 58: State::set(IndexSlider::DEADZONE_K28, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K28, p); break;
    case 59: State::set(IndexSlider::DEADZONE_K29, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K29, p); break;
    case 60: State::set(IndexSlider::DEADZONE_K2Start, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K2Start, p); break;
    case 61: State::set(IndexSlider::DEADZONE_K2Select, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K2Select, p); break;
    case 62: State::set(IndexSlider::DEADZONE_S2L, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S2L, p); break;
    case 63: State::set(IndexSlider::DEADZONE_S2R, p); ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S2R, p); break;
    case 64: State::set(IndexSlider::SPEED_S2A, p); ConfigMgr::Input(keys)->set(I_INPUT_SPEED_S2A, p); break;
    }
    InputMgr::updateDeadzones(keys);
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

    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
    case 36:
    case 37:
    case 38:
    case 39:
    case 40:
    case 41:
    case 42:
    case 43:
    case 44:
    case 51:
    case 52:
    case 53:
    case 54:
    case 55:
    case 56:
    case 57:
    case 58:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
        return std::bind(deadzone, type, _1);

    default:
        return [](double) {};
    }
}


}