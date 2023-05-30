#include "common/pch.h"
#include "scene_keyconfig.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/runtime/i18n.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

SceneKeyConfig::SceneKeyConfig() : SceneBase(SkinType::KEY_CONFIG, 240)
{
    _type = SceneType::KEYCONFIG;
    KeyConfigData.skinHasAbsAxis = pSkin->isSupportKeyConfigAbsAxis;

    InputMgr::updateDevices();

    _updateCallback = std::bind(&SceneKeyConfig::updateStart, this);

    KeyConfigData.selecting = { Input::Pad::K11, 0 };
    KeyConfigData.currentMode = 7;
    setInputBindingText(KeyConfigData.currentMode, KeyConfigData.selecting.first);

    KeyConfigData.force[Input::Pad::S1L] = 0.0;
    KeyConfigData.force[Input::Pad::S1R] = 0.0;
    KeyConfigData.force[Input::Pad::K1START] = 0.0;
    KeyConfigData.force[Input::Pad::K1SELECT] = 0.0;
    KeyConfigData.force[Input::Pad::S2L] = 0.0;
    KeyConfigData.force[Input::Pad::S2R] = 0.0;
    KeyConfigData.force[Input::Pad::K2START] = 0.0;
    KeyConfigData.force[Input::Pad::K2SELECT] = 0.0;
    KeyConfigData.force[Input::Pad::K11] = 0.0;
    KeyConfigData.force[Input::Pad::K12] = 0.0;
    KeyConfigData.force[Input::Pad::K13] = 0.0;
    KeyConfigData.force[Input::Pad::K14] = 0.0;
    KeyConfigData.force[Input::Pad::K15] = 0.0;
    KeyConfigData.force[Input::Pad::K16] = 0.0;
    KeyConfigData.force[Input::Pad::K17] = 0.0;
    KeyConfigData.force[Input::Pad::K18] = 0.0;
    KeyConfigData.force[Input::Pad::K19] = 0.0;
    KeyConfigData.force[Input::Pad::K21] = 0.0;
    KeyConfigData.force[Input::Pad::K22] = 0.0;
    KeyConfigData.force[Input::Pad::K23] = 0.0;
    KeyConfigData.force[Input::Pad::K24] = 0.0;
    KeyConfigData.force[Input::Pad::K25] = 0.0;
    KeyConfigData.force[Input::Pad::K26] = 0.0;
    KeyConfigData.force[Input::Pad::K27] = 0.0;
    KeyConfigData.force[Input::Pad::K28] = 0.0;
    KeyConfigData.force[Input::Pad::K29] = 0.0;

    updateInfo(KeyMap(), 0);

    LOG_DEBUG << "[KeyConfig] Start";
}

SceneKeyConfig::~SceneKeyConfig()
{
    _input.loopEnd();
    loopEnd();
}

void SceneKeyConfig::_updateAsync()
{
    if (SystemData.gNextScene != SceneType::KEYCONFIG) return;

    if (SystemData.isAppExiting)
    {
        SystemData.gNextScene = SceneType::EXIT_TRANS;
    }

    _updateCallback();

    updateForceBargraphs();

    if (KeyConfigData.modeChanged)
    {
        KeyConfigData.modeChanged = false;
        updateInfo(KeyMap(), 0);
    }

    SystemData.scratchAxisValue[0] = int(playerTurntableAngleAdd[0]) % 360;
    SystemData.scratchAxisValue[1] = int(playerTurntableAngleAdd[1]) % 360;
    PlayData.scratchAxis[0] = playerTurntableAngleAdd[0];
    PlayData.scratchAxis[1] = playerTurntableAngleAdd[1];
    KeyConfigData.scratchAxisValueText[0] = std::to_string(int((360 + int(PlayData.scratchAxis[0]) % 360) % 360 / 360.0 * 256));
    KeyConfigData.scratchAxisValueText[1] = std::to_string(int((360 + int(PlayData.scratchAxis[1]) % 360) % 360 / 360.0 * 256));
}

void SceneKeyConfig::updateStart()
{
    Time t;
    Time rt = t - SystemData.timers["scene_start"];
    if (rt.norm() > pSkin->info.timeIntro)
    {
        _updateCallback = std::bind(&SceneKeyConfig::updateMain, this);
        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneKeyConfig::inputGamePress, this, _1, _2));
        _input.register_a("SCENE_AXIS", std::bind(&SceneKeyConfig::inputGameAxis, this, _1, _2, _3));
        _input.register_kb("SCENE_KEYPRESS", std::bind(&SceneKeyConfig::inputGamePressKeyboard, this, _1, _2));
        _input.register_joy("SCENE_JOYPRESS", std::bind(&SceneKeyConfig::inputGamePressJoystick, this, _1, _2, _3));
        _input.register_aa("SCENE_ABSOLUTEAXIS", std::bind(&SceneKeyConfig::inputGameAbsoluteAxis, this, _1, _2, _3));
        LOG_DEBUG << "[KeyConfig] State changed to Main";
    }
}

void SceneKeyConfig::updateMain()
{
    Time t;
    if (exiting)
    {
        SystemData.timers["fadeout_start"] = t.norm();
        _updateCallback = std::bind(&SceneKeyConfig::updateFadeout, this);
        using namespace std::placeholders;
        _input.unregister_p("SCENE_PRESS");
        _input.unregister_a("SCENE_AXIS");
        _input.unregister_kb("SCENE_KEYPRESS");
        _input.unregister_joy("SCENE_JOYPRESS");
        _input.unregister_aa("SCENE_ABSOLUTEAXIS");
        LOG_DEBUG << "[KeyConfig] State changed to Fadeout";
    }
}

void SceneKeyConfig::updateFadeout()
{
    Time t;
    Time rt = t - SystemData.timers["fadeout_start"];

    if (rt.norm() > pSkin->info.timeOutro)
    {
        ConfigMgr::Input(KeyConfigData.currentMode)->save();   // this is kinda important
        SystemData.gNextScene = SceneType::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneKeyConfig::inputGamePress(InputMask& m, const Time& t)
{
    if (m[Input::Pad::ESC]) exiting = true;
    if (m[Input::Pad::M2]) exiting = true;

    // individual keys
    size_t sampleCount = 0;
    for (size_t i = 0; i < Input::Pad::ESC; ++i)
    {
        if (m[i])
        {
            PlayData.timers[InputGamePressMap.at(Input::Pad(i))] = t.norm();
            PlayData.timers[InputGameReleaseMap.at(Input::Pad(i))] = TIMER_NEVER;
        }
    }
}

void SceneKeyConfig::inputGameAxis(double S1, double S2, const Time& t)
{
    using namespace Input;

    // turntable spin
    playerTurntableAngleAdd[PLAYER_SLOT_PLAYER] += S1 * 2.0 * 360;
    playerTurntableAngleAdd[PLAYER_SLOT_TARGET] += S2 * 2.0 * 360;
}

void SceneKeyConfig::inputGamePressKeyboard(KeyboardMask& mask, const Time& t)
{
    // update bindings
    auto [pad, slot] = KeyConfigData.selecting;
    GameModeKeys keys = KeyConfigData.currentMode;

    if (pad != Input::Pad::INVALID)
    {
        std::unique_lock l(_mutex);

        if (mask[static_cast<size_t>(Input::Keyboard::K_DEL)])
        {
            KeyMap undef;
            ConfigMgr::Input(keys)->bind(pad, undef);
            InputMgr::updateBindings(keys, pad);
            updateInfo(undef, slot);
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
            return;
        }

        if (pad == Input::Pad::S1A || pad == Input::Pad::S2A)
            return;

        if (mask[static_cast<size_t>(Input::Keyboard::K_F1)])
        {
            ConfigMgr::Input(keys)->clearAll();
            updateInfo(KeyMap(), slot);
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
        }
        else if (mask[static_cast<size_t>(Input::Keyboard::K_F2)])
        {
            ConfigMgr::Input(keys)->setDefaults();
            updateInfo(ConfigMgr::Input(keys)->getBindings(pad), slot);
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
        }
        else
        {
            KeyboardMask filtered = mask;
            filtered[(size_t)Input::Keyboard::K_F1] = false;
            filtered[(size_t)Input::Keyboard::K_F2] = false;
            filtered[(size_t)Input::Keyboard::K_F3] = false;
            filtered[(size_t)Input::Keyboard::K_F4] = false;
            filtered[(size_t)Input::Keyboard::K_F5] = false;
            filtered[(size_t)Input::Keyboard::K_F6] = false;
            filtered[(size_t)Input::Keyboard::K_F7] = false;
            filtered[(size_t)Input::Keyboard::K_F8] = false;
            filtered[(size_t)Input::Keyboard::K_F9] = false;
            filtered[(size_t)Input::Keyboard::K_F10] = false;
            filtered[(size_t)Input::Keyboard::K_F11] = false;
            filtered[(size_t)Input::Keyboard::K_F12] = false;
            filtered[(size_t)Input::Keyboard::K_ESC] = false;
            filtered[(size_t)Input::Keyboard::K_DEL] = false;

            for (Input::Keyboard k = Input::Keyboard::K_1; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
            {
                if (filtered[static_cast<size_t>(k)])
                {
                    // modify slot
                    KeyMap km(k);
                    ConfigMgr::Input(keys)->bind(pad, km);
                    InputMgr::updateBindings(keys, pad);

                    updateInfo(km, slot);

                    // play sound
                    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
                }
            }
        }
    }
}

void SceneKeyConfig::inputGamePressJoystick(JoystickMask& mask, size_t device, const Time& t)
{
    // update bindings
    auto [pad, slot] = KeyConfigData.selecting;
    if (pad == Input::Pad::S1A || pad == Input::Pad::S2A)
        return;

    GameModeKeys keys = KeyConfigData.currentMode;

    if (pad != Input::Pad::INVALID)
    {
        std::unique_lock l(_mutex);

        size_t base = 0;

        // Buttons
        for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_BUTTON_COUNT; ++index)
        {
            size_t bit = static_cast<size_t>(base + index);
            if (mask[bit] && !joystickPrev[device][bit])
            {
                KeyMap j(device, Input::Joystick::Type::BUTTON, index);
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);

                // play sound
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
            }
            joystickPrev[device][bit] = mask[bit];
        }
        base += InputMgr::MAX_JOYSTICK_BUTTON_COUNT;

        // Pov, 4 directions each
        for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_POV_COUNT; ++index)
        {
            static const std::pair<size_t, size_t> bitmask[] =
            {
                {static_cast<size_t>(base + index * 4 + 0), (1ul << 31)},   // LEFT
                {static_cast<size_t>(base + index * 4 + 1), (1ul << 30)},   // DOWN
                {static_cast<size_t>(base + index * 4 + 2), (1ul << 29)},   // UP
                {static_cast<size_t>(base + index * 4 + 3), (1ul << 28)},   // RIGHT
            };
            for (size_t i = 0; i < 4; ++i)
            {
                const auto& [bit, indexMask] = bitmask[i];
                if (mask[bit] && !joystickPrev[device][bit])
                {
                    KeyMap j(device, Input::Joystick::Type::POV, index | indexMask);
                    ConfigMgr::Input(keys)->bind(pad, j);
                    InputMgr::updateBindings(keys, pad);
                    updateInfo(j, slot);

                    // play sound
                    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
                }
                joystickPrev[device][bit] = mask[bit];
            }
        }
        base += InputMgr::MAX_JOYSTICK_POV_COUNT * 4;

        auto s1a = ConfigMgr::Input(keys)->getBindings(Input::Pad::S1A);
        auto s2a = ConfigMgr::Input(keys)->getBindings(Input::Pad::S2A);

        if (pad != Input::Pad::S1A && pad != Input::Pad::S2A ||
            (pad == Input::Pad::S1L || pad == Input::Pad::S1R) && s1a.getType() == KeyMap::DeviceType::UNDEF ||
            (pad == Input::Pad::S2L || pad == Input::Pad::S2R) && s2a.getType() == KeyMap::DeviceType::UNDEF)
        {
            // Relative Axis +
            for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
            {
                size_t bit = static_cast<size_t>(base + index);
                if (mask[bit] && !joystickPrev[device][bit])
                {
                    auto b = ConfigMgr::Input(keys)->getBindings(pad);
                    if (b.getType() == KeyMap::DeviceType::JOYSTICK &&
                        b.getJoystick().type == Input::Joystick::Type::AXIS_RELATIVE_POSITIVE &&
                        b.getJoystick().index == index)
                        continue;
                    KeyMap j(device, Input::Joystick::Type::AXIS_RELATIVE_POSITIVE, index);
                    ConfigMgr::Input(keys)->bind(pad, j);
                    InputMgr::updateBindings(keys, pad);
                    updateInfo(j, slot);

                    // play sound
                    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
                }
                joystickPrev[device][bit] = mask[bit];
            }
            base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

            // Relative Axis -
            for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
            {
                size_t bit = static_cast<size_t>(base + index);
                if (mask[bit] && !joystickPrev[device][bit])
                {
                    auto b = ConfigMgr::Input(keys)->getBindings(pad);
                    if (b.getType() == KeyMap::DeviceType::JOYSTICK &&
                        b.getJoystick().type == Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE &&
                        b.getJoystick().index == index)
                        continue;
                    KeyMap j(device, Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE, index);
                    ConfigMgr::Input(keys)->bind(pad, j);
                    InputMgr::updateBindings(keys, pad);
                    updateInfo(j, slot);

                    // play sound
                    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
                }
                joystickPrev[device][bit] = mask[bit];
            }
        }
    }
}

void SceneKeyConfig::inputGameAbsoluteAxis(JoystickAxis& axis, size_t device, const Time&)
{
    auto [pad, slot] = KeyConfigData.selecting;
    if (pad != Input::Pad::S1A && pad != Input::Pad::S2A)
        return;

    std::unique_lock l(_mutex);

    GameModeKeys keys = KeyConfigData.currentMode;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
    {
        if (axis[index] != -1.0)
        {
            const auto bindings = ConfigMgr::Input(keys)->getBindings(pad);
            if (bindings.getType() != KeyMap::DeviceType::JOYSTICK ||
                bindings.getJoystick().index != index)
            {
                KeyMap j(device, Input::Joystick::Type::AXIS_ABSOLUTE, index);
                ConfigMgr::Input(keys)->bind(pad, j);

                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);

                // play sound
                SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
            }
        }
    }
}


void SceneKeyConfig::setInputBindingText(GameModeKeys keys, Input::Pad pad)
{
    auto bindings = ConfigMgr::Input(keys)->getBindings(pad);
    KeyConfigData.bindName[pad] = bindings.toString();
}

void SceneKeyConfig::updateForceBargraphs()
{
    std::shared_lock l(_mutex);

    GameModeKeys keys = KeyConfigData.currentMode;
    const auto input = ConfigMgr::Input(keys);
    Time t;

    // update keyboard force bargraph
    for (Input::Keyboard k = Input::Keyboard::K_1; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
    {
        const auto input = ConfigMgr::Input(keys);
        for (auto& [p, bar] : KeyConfigData.force)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::KEYBOARD && binding.getKeyboard() == k)
            {
                if (isKeyPressed(k))
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    bar = 1.0;
                }
            }
        }
    }
    // update joystick force bargraph
    size_t base = 0;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_BUTTON_COUNT; ++index)
    {
        for (auto& [p, bar] : KeyConfigData.force)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::BUTTON &&
                binding.getJoystick().index == index)
            {
                if (isButtonPressed(binding.getJoystick(), 0.0))
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    bar = 1.0;
                }
            }
        }
    }
    base += InputMgr::MAX_JOYSTICK_BUTTON_COUNT;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_POV_COUNT; ++index)
    {
        for (auto& [p, bar] : KeyConfigData.force)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::POV &&
                (binding.getJoystick().index & 0xFFFF) == index)
            {
                if (isButtonPressed(binding.getJoystick(), 0.0))
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    bar = 1.0;
                }
            }
        }
    }
    base += InputMgr::MAX_JOYSTICK_POV_COUNT * 4;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
    {
        for (auto& [p, bar] : KeyConfigData.force)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::AXIS_RELATIVE_POSITIVE &&
                binding.getJoystick().index == index)
            {
                double axis = _input.getJoystickAxis(binding.getJoystick().device, Input::Joystick::Type::AXIS_RELATIVE_POSITIVE, index);
                if (axis > 0.01 && axis <= 1.0)
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    bar = axis;
                }
            }
        }
    }
    base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
    {
        for (auto& [p, bar] : KeyConfigData.force)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE &&
                binding.getJoystick().index == index)
            {
                double axis = _input.getJoystickAxis(binding.getJoystick().device, Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE, index);
                if (axis > 0.01 && axis <= 1.0)
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    bar = axis;
                }
            }
        }
    }

    for (auto& [pad, bar] : KeyConfigData.force)
    {
        if (forceBargraphTriggerTimestamp.find(pad) != forceBargraphTriggerTimestamp.end() &&
            forceBargraphTriggerTimestamp[pad] != 0 &&
            t - forceBargraphTriggerTimestamp[pad] > 200)  // 1s timeout
        {
            bar = 0.0;
        }
    }
}

void SceneKeyConfig::updateInfo(KeyMap k, int slot)
{
    slot = 0;

    // update sliders
    if (KeyConfigData.currentMode != 0)
    {
        using namespace cfg;
        int keys = KeyConfigData.currentMode;
        KeyConfigData.deadzone[Input::Pad::S1L] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S1L, 0.2);
        KeyConfigData.deadzone[Input::Pad::S1R] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S1R, 0.2);
        KeyConfigData.deadzone[Input::Pad::K1START] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K1Start, 0.2);
        KeyConfigData.deadzone[Input::Pad::K1SELECT] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K1Select, 0.2);
        KeyConfigData.deadzone[Input::Pad::K11] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K11, 0.2);
        KeyConfigData.deadzone[Input::Pad::K12] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K12, 0.2);
        KeyConfigData.deadzone[Input::Pad::K13] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K13, 0.2);
        KeyConfigData.deadzone[Input::Pad::K14] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K14, 0.2);
        KeyConfigData.deadzone[Input::Pad::K15] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K15, 0.2);
        KeyConfigData.deadzone[Input::Pad::K16] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K16, 0.2);
        KeyConfigData.deadzone[Input::Pad::K17] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K17, 0.2);
        KeyConfigData.deadzone[Input::Pad::K18] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K18, 0.2);
        KeyConfigData.deadzone[Input::Pad::K19] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K19, 0.2);
        KeyConfigData.deadzone[Input::Pad::S2L] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S2L, 0.2);
        KeyConfigData.deadzone[Input::Pad::S2R] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S2R, 0.2);
        KeyConfigData.deadzone[Input::Pad::K2START] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K2Start, 0.2);
        KeyConfigData.deadzone[Input::Pad::K2SELECT] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K2Select, 0.2);
        KeyConfigData.deadzone[Input::Pad::K21] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K21, 0.2);
        KeyConfigData.deadzone[Input::Pad::K22] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K22, 0.2);
        KeyConfigData.deadzone[Input::Pad::K23] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K23, 0.2);
        KeyConfigData.deadzone[Input::Pad::K24] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K24, 0.2);
        KeyConfigData.deadzone[Input::Pad::K25] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K25, 0.2);
        KeyConfigData.deadzone[Input::Pad::K26] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K26, 0.2);
        KeyConfigData.deadzone[Input::Pad::K27] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K27, 0.2);
        KeyConfigData.deadzone[Input::Pad::K28] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K28, 0.2);
        KeyConfigData.deadzone[Input::Pad::K29] = ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K29, 0.2);
        KeyConfigData.scratchAxisSpeed[0] = ConfigMgr::Input(keys)->get(I_INPUT_SPEED_S1A, 0.5);
        KeyConfigData.scratchAxisSpeed[1] = ConfigMgr::Input(keys)->get(I_INPUT_SPEED_S2A, 0.5);
    }

    updateAllText();
}

void SceneKeyConfig::updateAllText()
{
    auto [pad, slot] = KeyConfigData.selecting;
    GameModeKeys keys = KeyConfigData.currentMode;

    const auto input = ConfigMgr::Input(keys);

    KeyConfigData.bindName[Input::Pad::K11] = input->getBindings(Input::Pad::K11).toString();
    KeyConfigData.bindName[Input::Pad::K12] = input->getBindings(Input::Pad::K12).toString();
    KeyConfigData.bindName[Input::Pad::K13] = input->getBindings(Input::Pad::K13).toString();
    KeyConfigData.bindName[Input::Pad::K14] = input->getBindings(Input::Pad::K14).toString();
    KeyConfigData.bindName[Input::Pad::K15] = input->getBindings(Input::Pad::K15).toString();
    KeyConfigData.bindName[Input::Pad::K16] = input->getBindings(Input::Pad::K16).toString();
    KeyConfigData.bindName[Input::Pad::K17] = input->getBindings(Input::Pad::K17).toString();
    KeyConfigData.bindName[Input::Pad::K18] = input->getBindings(Input::Pad::K18).toString();
    KeyConfigData.bindName[Input::Pad::K19] = input->getBindings(Input::Pad::K19).toString();
    KeyConfigData.bindName[Input::Pad::S1L] = input->getBindings(Input::Pad::S1L).toString();
    KeyConfigData.bindName[Input::Pad::S1R] = input->getBindings(Input::Pad::S1R).toString();
    KeyConfigData.bindName[Input::Pad::K1START] = input->getBindings(Input::Pad::K1START).toString();
    KeyConfigData.bindName[Input::Pad::K1SELECT] = input->getBindings(Input::Pad::K1SELECT).toString();
    KeyConfigData.bindName[Input::Pad::S1A] = input->getBindings(Input::Pad::S1A).toString();
    KeyConfigData.bindName[Input::Pad::K21] = input->getBindings(Input::Pad::K21).toString();
    KeyConfigData.bindName[Input::Pad::K22] = input->getBindings(Input::Pad::K22).toString();
    KeyConfigData.bindName[Input::Pad::K23] = input->getBindings(Input::Pad::K23).toString();
    KeyConfigData.bindName[Input::Pad::K24] = input->getBindings(Input::Pad::K24).toString();
    KeyConfigData.bindName[Input::Pad::K25] = input->getBindings(Input::Pad::K25).toString();
    KeyConfigData.bindName[Input::Pad::K26] = input->getBindings(Input::Pad::K26).toString();
    KeyConfigData.bindName[Input::Pad::K27] = input->getBindings(Input::Pad::K27).toString();
    KeyConfigData.bindName[Input::Pad::K28] = input->getBindings(Input::Pad::K28).toString();
    KeyConfigData.bindName[Input::Pad::K29] = input->getBindings(Input::Pad::K29).toString();
    KeyConfigData.bindName[Input::Pad::S2L] = input->getBindings(Input::Pad::S2L).toString();
    KeyConfigData.bindName[Input::Pad::S2R] = input->getBindings(Input::Pad::S2R).toString();
    KeyConfigData.bindName[Input::Pad::K2START] = input->getBindings(Input::Pad::K2START).toString();
    KeyConfigData.bindName[Input::Pad::K2SELECT] = input->getBindings(Input::Pad::K2SELECT).toString();
    KeyConfigData.bindName[Input::Pad::S2A] = input->getBindings(Input::Pad::S2A).toString();
}

}
