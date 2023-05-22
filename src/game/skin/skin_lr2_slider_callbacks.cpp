
#include "common/pch.h"
#include "skin_lr2_slider_callbacks.h"

#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/data/data_types.h"
#include "game/input/input_mgr.h"

namespace lunaticvibes
{

namespace lr2skin::slider
{

#pragma region slider type callbacks

void select_pos(double p)
{
    if (SelectData.entries.empty()) return;

    size_t idx_new = (size_t)std::floor(p * SelectData.entries.size());
    if (idx_new != SelectData.selectedEntryIndex)
    {
        SelectData.selectedEntryIndexRolling = (double)idx_new / SelectData.entries.size();
        SelectData.draggingListSlider = true;
    }
}

void customize_scrollbar(double p)
{
    // LR2CustomizeData.optionDragging = true;
}

void ir_ranking_scrollbar(double p)
{
    // no ir support right now
}

void eq(int idx, double p)
{
    int val = int(p * 24) - 12;
    p = (val + 12) / 24.0;
    switch (idx)
    {
    case 0: SystemData.equalizerVal62_5hz = val; break;
    case 1: SystemData.equalizerVal160hz = val; break;
    case 2: SystemData.equalizerVal400hz = val; break;
    case 3: SystemData.equalizerVal1khz = val; break;
    case 4: SystemData.equalizerVal2_5khz = val; break;
    case 5: SystemData.equalizerVal6_25khz = val; break;
    case 6: SystemData.equalizerVal16khz = val; break;
    }

    if (SystemData.equalizerEnabled)
    {
        SoundMgr::setEQ((EQFreq)idx, val);
    }
}

void master_volume(double p)
{
    SystemData.volumeMaster = p;
    SoundMgr::setVolume(SampleChannel::MASTER, (float)p);
}

void key_volume(double p)
{
    SystemData.volumeKey = p;
    SoundMgr::setVolume(SampleChannel::KEY, (float)p);
}

void bgm_volume(double p)
{
    SystemData.volumeBgm = p;
    SoundMgr::setVolume(SampleChannel::BGM, (float)p);
}

void fx0(int idx, double p)
{
    if (idx == 0)
    {
        SystemData.fxVal = p;
        if (SystemData.fxType == FXType::Off)
            SoundMgr::setDSP(DSPType::OFF, 0, SampleChannel::MASTER, 0.f, 0.f);
        else
            SoundMgr::setDSP(SystemData.fxType, 0, SampleChannel::MASTER, p, p);
    }
}

void fx1(int idx, double p)
{
}

void fx2(int idx, double p)
{
}

void pitch(double p)
{
    int val = int(p * 24) - 12;
    setFreqModifier(SystemData.freqType, val);
}

void deadzone(int type, double p)
{
    using namespace cfg;
    int keys = KeyConfigData.currentMode;
    switch (type)
    {
    case 31: KeyConfigData.deadzone[Input::Pad::K11] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K11, p); break;
    case 32: KeyConfigData.deadzone[Input::Pad::K12], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K12, p); break;
    case 33: KeyConfigData.deadzone[Input::Pad::K13], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K13, p); break;
    case 34: KeyConfigData.deadzone[Input::Pad::K14], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K14, p); break;
    case 35: KeyConfigData.deadzone[Input::Pad::K15], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K15, p); break;
    case 36: KeyConfigData.deadzone[Input::Pad::K16], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K16, p); break;
    case 37: KeyConfigData.deadzone[Input::Pad::K17], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K17, p); break;
    case 38: KeyConfigData.deadzone[Input::Pad::K18], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K18, p); break;
    case 39: KeyConfigData.deadzone[Input::Pad::K19], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K19, p); break;
    case 40: KeyConfigData.deadzone[Input::Pad::K1START], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K1Start, p); break;
    case 41: KeyConfigData.deadzone[Input::Pad::K1SELECT], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K1Select, p); break;
    case 42: KeyConfigData.deadzone[Input::Pad::S1L], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S1L, p); break;
    case 43: KeyConfigData.deadzone[Input::Pad::S1R], p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S1R, p); break;
    case 44: KeyConfigData.scratchAxisSpeed[0] = p; ConfigMgr::Input(keys)->set(I_INPUT_SPEED_S1A, p); break;
    case 51: KeyConfigData.deadzone[Input::Pad::K21] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K21, p); break;
    case 52: KeyConfigData.deadzone[Input::Pad::K22] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K22, p); break;
    case 53: KeyConfigData.deadzone[Input::Pad::K23] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K23, p); break;
    case 54: KeyConfigData.deadzone[Input::Pad::K24] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K24, p); break;
    case 55: KeyConfigData.deadzone[Input::Pad::K25] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K25, p); break;
    case 56: KeyConfigData.deadzone[Input::Pad::K26] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K26, p); break;
    case 57: KeyConfigData.deadzone[Input::Pad::K27] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K27, p); break;
    case 58: KeyConfigData.deadzone[Input::Pad::K28] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K28, p); break;
    case 59: KeyConfigData.deadzone[Input::Pad::K29] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K29, p); break;
    case 60: KeyConfigData.deadzone[Input::Pad::K2START] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K2Start, p); break;
    case 61: KeyConfigData.deadzone[Input::Pad::K2SELECT] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_K2Select, p); break;
    case 62: KeyConfigData.deadzone[Input::Pad::S2L] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S2L, p); break;
    case 63: KeyConfigData.deadzone[Input::Pad::S2R] = p; ConfigMgr::Input(keys)->set(I_INPUT_DEADZONE_S2R, p); break;
    case 64: KeyConfigData.scratchAxisSpeed[1] = p; ConfigMgr::Input(keys)->set(I_INPUT_SPEED_S2A, p); break;
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

}