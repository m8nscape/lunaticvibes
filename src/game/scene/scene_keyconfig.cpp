#include "scene_keyconfig.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"
#include "game/runtime/i18n.h"

SceneKeyConfig::SceneKeyConfig() : SceneBase(SkinType::KEY_CONFIG, 240)
{
    _type = SceneType::KEYCONFIG;
    gKeyconfigContext.skinHasAbsAxis = pSkin->isSupportKeyConfigAbsAxis;

    InputMgr::updateDevices();

    _updateCallback = std::bind(&SceneKeyConfig::updateStart, this);

    State::set(IndexSwitch::K11_CONFIG, false);
    State::set(IndexSwitch::K12_CONFIG, false);
    State::set(IndexSwitch::K13_CONFIG, false);
    State::set(IndexSwitch::K14_CONFIG, false);
    State::set(IndexSwitch::K15_CONFIG, false);
    State::set(IndexSwitch::K16_CONFIG, false);
    State::set(IndexSwitch::K17_CONFIG, false);
    State::set(IndexSwitch::K18_CONFIG, false);
    State::set(IndexSwitch::K19_CONFIG, false);
    State::set(IndexSwitch::S1L_CONFIG, false);
    State::set(IndexSwitch::S1R_CONFIG, false);
    State::set(IndexSwitch::S1A_CONFIG, false);
    State::set(IndexSwitch::K1START_CONFIG, false);
    State::set(IndexSwitch::K1SELECT_CONFIG, false);
    State::set(IndexSwitch::K1SPDUP_CONFIG, false);
    State::set(IndexSwitch::K1SPDDN_CONFIG, false);
    State::set(IndexSwitch::K21_CONFIG, false);
    State::set(IndexSwitch::K22_CONFIG, false);
    State::set(IndexSwitch::K23_CONFIG, false);
    State::set(IndexSwitch::K24_CONFIG, false);
    State::set(IndexSwitch::K25_CONFIG, false);
    State::set(IndexSwitch::K26_CONFIG, false);
    State::set(IndexSwitch::K27_CONFIG, false);
    State::set(IndexSwitch::K28_CONFIG, false);
    State::set(IndexSwitch::K29_CONFIG, false);
    State::set(IndexSwitch::S2L_CONFIG, false);
    State::set(IndexSwitch::S2R_CONFIG, false);
    State::set(IndexSwitch::S2A_CONFIG, false);
    State::set(IndexSwitch::K2START_CONFIG, false);
    State::set(IndexSwitch::K2SELECT_CONFIG, false);
    State::set(IndexSwitch::K2SPDUP_CONFIG, false);
    State::set(IndexSwitch::K2SPDDN_CONFIG, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT0, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT1, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT2, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT3, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT4, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT5, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT6, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT7, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT8, false);
    State::set(IndexSwitch::KEY_CONFIG_SLOT9, false);

    gKeyconfigContext.selecting = { Input::Pad::K11, 0 };
    gKeyconfigContext.keys = 7;
    State::set(IndexSwitch::K11_CONFIG, true);
    State::set(IndexSwitch::KEY_CONFIG_SLOT0, true);
    State::set(IndexOption::KEY_CONFIG_MODE, Option::KEYCFG_7);
    State::set(IndexOption::KEY_CONFIG_KEY5, 1);
    State::set(IndexOption::KEY_CONFIG_KEY7, 1);
    State::set(IndexOption::KEY_CONFIG_KEY9, 1);
    setInputBindingText(gKeyconfigContext.keys, gKeyconfigContext.selecting.first);

    State::set(IndexText::KEYCONFIG_HINT_KEY, i18n::s(i18nText::KEYCONFIG_HINT_KEY));
    State::set(IndexText::KEYCONFIG_HINT_BIND, i18n::s(i18nText::KEYCONFIG_HINT_BIND));
    State::set(IndexText::KEYCONFIG_HINT_DEADZONE, i18n::s(i18nText::KEYCONFIG_HINT_DEADZONE));
    State::set(IndexText::KEYCONFIG_HINT_F1, i18n::s(i18nText::KEYCONFIG_HINT_F1));
    State::set(IndexText::KEYCONFIG_HINT_F2, i18n::s(i18nText::KEYCONFIG_HINT_F2));
    State::set(IndexText::KEYCONFIG_HINT_DEL, i18n::s(i18nText::KEYCONFIG_HINT_DEL));
    State::set(IndexText::KEYCONFIG_HINT_SCRATCH_ABS, i18n::s(i18nText::KEYCONFIG_HINT_SCRATCH_ABS));
    State::set(IndexText::KEYCONFIG_1P_1, "1P 1");
    State::set(IndexText::KEYCONFIG_1P_2, "1P 2");
    State::set(IndexText::KEYCONFIG_1P_3, "1P 3");
    State::set(IndexText::KEYCONFIG_1P_4, "1P 4");
    State::set(IndexText::KEYCONFIG_1P_5, "1P 5");
    State::set(IndexText::KEYCONFIG_1P_6, "1P 6");
    State::set(IndexText::KEYCONFIG_1P_7, "1P 7");
    State::set(IndexText::KEYCONFIG_1P_8, "1P 8");
    State::set(IndexText::KEYCONFIG_1P_9, "1P 9");
    State::set(IndexText::KEYCONFIG_1P_START, "1P START");
    State::set(IndexText::KEYCONFIG_1P_SELECT, "1P SELECT");
    State::set(IndexText::KEYCONFIG_1P_SCRATCH_L, "1P SC-L");
    State::set(IndexText::KEYCONFIG_1P_SCRATCH_R, "1P SC-R");
    State::set(IndexText::KEYCONFIG_1P_SCRATCH_ABS, "1P SC ABS");
    State::set(IndexText::KEYCONFIG_2P_1, "2P 1");
    State::set(IndexText::KEYCONFIG_2P_2, "2P 2");
    State::set(IndexText::KEYCONFIG_2P_3, "2P 3");
    State::set(IndexText::KEYCONFIG_2P_4, "2P 4");
    State::set(IndexText::KEYCONFIG_2P_5, "2P 5");
    State::set(IndexText::KEYCONFIG_2P_6, "2P 6");
    State::set(IndexText::KEYCONFIG_2P_7, "2P 7");
    State::set(IndexText::KEYCONFIG_2P_8, "2P 8");
    State::set(IndexText::KEYCONFIG_2P_9, "2P 9");
    State::set(IndexText::KEYCONFIG_2P_START, "2P START");
    State::set(IndexText::KEYCONFIG_2P_SELECT, "2P SELECT");
    State::set(IndexText::KEYCONFIG_2P_SCRATCH_L, "2P SC-L");
    State::set(IndexText::KEYCONFIG_2P_SCRATCH_R, "2P SC-R");
    State::set(IndexText::KEYCONFIG_2P_SCRATCH_ABS, "2P SC ABS");

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
    if (gNextScene != SceneType::KEYCONFIG) return;

    if (gAppIsExiting)
    {
        gNextScene = SceneType::EXIT_TRANS;
    }

    _updateCallback();

    updateForceBargraphs();

    if (gKeyconfigContext.modeChanged)
    {
        gKeyconfigContext.modeChanged = false;
        updateInfo(KeyMap(), 0);
    }

    State::set(IndexNumber::_ANGLE_TT_1P, int(playerTurntableAngleAdd[0]) % 360);
    State::set(IndexNumber::_ANGLE_TT_2P, int(playerTurntableAngleAdd[1]) % 360);
    State::set(IndexNumber::SCRATCH_AXIS_1P_ANGLE, (360 + int(playerTurntableAngleAdd[0]) % 360) % 360 / 360.0 * 256);
    State::set(IndexNumber::SCRATCH_AXIS_2P_ANGLE, (360 + int(playerTurntableAngleAdd[1]) % 360) % 360 / 360.0 * 256);
    State::set(IndexText::KEYCONFIG_1P_SCRATCH_ABS_VALUE, std::to_string(State::get(IndexNumber::SCRATCH_AXIS_1P_ANGLE)));
    State::set(IndexText::KEYCONFIG_2P_SCRATCH_ABS_VALUE, std::to_string(State::get(IndexNumber::SCRATCH_AXIS_2P_ANGLE)));
}

void SceneKeyConfig::updateStart()
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);
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
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
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
    Time rt = t - State::get(IndexTimer::FADEOUT_BEGIN);

    if (rt.norm() > pSkin->info.timeOutro)
    {
        ConfigMgr::Input(gKeyconfigContext.keys)->save();   // this is kinda important
        gNextScene = SceneType::SELECT;
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
            State::set(InputGamePressMap[i].tm, t.norm());
            State::set(InputGameReleaseMap[i].tm, TIMER_NEVER);
            State::set(InputGamePressMap[i].sw, true);
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

static const std::map<Input::Pad, IndexBargraph> forceBargraphMap =
{
    { Input::Pad::S1L,      IndexBargraph::FORCE_S1L },
    { Input::Pad::S1R,      IndexBargraph::FORCE_S1R },
    { Input::Pad::K1START,  IndexBargraph::FORCE_K1Start },
    { Input::Pad::K1SELECT, IndexBargraph::FORCE_K1Select },
    { Input::Pad::S2L,      IndexBargraph::FORCE_S2L },
    { Input::Pad::S2R,      IndexBargraph::FORCE_S2R },
    { Input::Pad::K2START,  IndexBargraph::FORCE_K2Start },
    { Input::Pad::K2SELECT, IndexBargraph::FORCE_K2Select },
    { Input::Pad::K11,      IndexBargraph::FORCE_K11 },
    { Input::Pad::K12,      IndexBargraph::FORCE_K12 },
    { Input::Pad::K13,      IndexBargraph::FORCE_K13 },
    { Input::Pad::K14,      IndexBargraph::FORCE_K14 },
    { Input::Pad::K15,      IndexBargraph::FORCE_K15 },
    { Input::Pad::K16,      IndexBargraph::FORCE_K16 },
    { Input::Pad::K17,      IndexBargraph::FORCE_K17 },
    { Input::Pad::K18,      IndexBargraph::FORCE_K18 },
    { Input::Pad::K19,      IndexBargraph::FORCE_K19 },
    { Input::Pad::K21,      IndexBargraph::FORCE_K21 },
    { Input::Pad::K22,      IndexBargraph::FORCE_K22 },
    { Input::Pad::K23,      IndexBargraph::FORCE_K23 },
    { Input::Pad::K24,      IndexBargraph::FORCE_K24 },
    { Input::Pad::K25,      IndexBargraph::FORCE_K25 },
    { Input::Pad::K26,      IndexBargraph::FORCE_K26 },
    { Input::Pad::K27,      IndexBargraph::FORCE_K27 },
    { Input::Pad::K28,      IndexBargraph::FORCE_K28 },
    { Input::Pad::K29,      IndexBargraph::FORCE_K29 },
};

void SceneKeyConfig::inputGamePressKeyboard(KeyboardMask& mask, const Time& t)
{
    // update bindings
    auto [pad, slot] = gKeyconfigContext.selecting;
    GameModeKeys keys = gKeyconfigContext.keys;

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
    auto [pad, slot] = gKeyconfigContext.selecting;
    if (pad == Input::Pad::S1A || pad == Input::Pad::S2A)
        return;

    GameModeKeys keys = gKeyconfigContext.keys;

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
    auto [pad, slot] = gKeyconfigContext.selecting;
    if (pad != Input::Pad::S1A && pad != Input::Pad::S2A)
        return;

    std::unique_lock l(_mutex);

    GameModeKeys keys = gKeyconfigContext.keys;

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
    State::set(IndexText::KEYCONFIG_SLOT1, bindings.toString());
    for (size_t i = 1; i < 10; ++i)
    {
        State::set(IndexText(unsigned(IndexText::KEYCONFIG_SLOT1) + i), "-");
    }
}

void SceneKeyConfig::updateForceBargraphs()
{
    std::shared_lock l(_mutex);

    GameModeKeys keys = gKeyconfigContext.keys;
    const auto input = ConfigMgr::Input(keys);
    Time t;

    // update keyboard force bargraph
    for (Input::Keyboard k = Input::Keyboard::K_1; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
    {
        const auto input = ConfigMgr::Input(keys);
        for (const auto& [p, bar] : forceBargraphMap)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::KEYBOARD && binding.getKeyboard() == k)
            {
                if (isKeyPressed(k))
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    State::set(bar, 1.0);
                }
            }
        }
    }
    // update joystick force bargraph
    size_t base = 0;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_BUTTON_COUNT; ++index)
    {
        for (const auto& [p, bar] : forceBargraphMap)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::BUTTON &&
                binding.getJoystick().index == index)
            {
                if (isButtonPressed(binding.getJoystick(), 0.0))
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    State::set(bar, 1.0);
                }
            }
        }
    }
    base += InputMgr::MAX_JOYSTICK_BUTTON_COUNT;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_POV_COUNT; ++index)
    {
        for (const auto& [p, bar] : forceBargraphMap)
        {
            const auto binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::POV &&
                (binding.getJoystick().index & 0xFFFF) == index)
            {
                if (isButtonPressed(binding.getJoystick(), 0.0))
                {
                    forceBargraphTriggerTimestamp[p] = t.norm();
                    State::set(bar, 1.0);
                }
            }
        }
    }
    base += InputMgr::MAX_JOYSTICK_POV_COUNT * 4;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
    {
        for (const auto& [p, bar] : forceBargraphMap)
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
                    State::set(bar, axis);
                }
            }
        }
    }
    base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
    {
        for (const auto& [p, bar] : forceBargraphMap)
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
                    State::set(bar, axis);
                }
            }
        }
    }

    for (auto& [pad, bar] : forceBargraphMap)
    {
        if (forceBargraphTriggerTimestamp.find(pad) != forceBargraphTriggerTimestamp.end() &&
            forceBargraphTriggerTimestamp[pad] != 0 &&
            t - forceBargraphTriggerTimestamp[pad] > 200)  // 1s timeout
        {
            State::set(bar, 0.0);
        }
    }
}

void SceneKeyConfig::updateInfo(KeyMap k, int slot)
{
    slot = 0;

    // update text
    switch (slot)
    {
    case 0: State::set(IndexText::KEYCONFIG_SLOT1, k.toString()); break;
    case 1: State::set(IndexText::KEYCONFIG_SLOT2, k.toString()); break;
    case 2: State::set(IndexText::KEYCONFIG_SLOT3, k.toString()); break;
    case 3: State::set(IndexText::KEYCONFIG_SLOT4, k.toString()); break;
    case 4: State::set(IndexText::KEYCONFIG_SLOT5, k.toString()); break;
    case 5: State::set(IndexText::KEYCONFIG_SLOT6, k.toString()); break;
    case 6: State::set(IndexText::KEYCONFIG_SLOT7, k.toString()); break;
    case 7: State::set(IndexText::KEYCONFIG_SLOT8, k.toString()); break;
    case 8: State::set(IndexText::KEYCONFIG_SLOT9, k.toString()); break;
    case 9: State::set(IndexText::KEYCONFIG_SLOT10, k.toString()); break;
    default: break;
    }

    // update sliders
    int keys = 0;
    switch (State::get(IndexOption::KEY_CONFIG_MODE))
    {
    case Option::KEYCFG_5: keys = 5; break;
    case Option::KEYCFG_7: keys = 7; break;
    case Option::KEYCFG_9: keys = 9; break;
    default: return;
    }
    if (keys != 0)
    {
        using namespace cfg;
        State::set(IndexSlider::DEADZONE_S1L, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S1L, 0.2));
        State::set(IndexSlider::DEADZONE_S1R, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S1R, 0.2));
        State::set(IndexSlider::DEADZONE_K1Start, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K1Start, 0.2));
        State::set(IndexSlider::DEADZONE_K1Select, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K1Select, 0.2));
        State::set(IndexSlider::DEADZONE_K11, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K11, 0.2));
        State::set(IndexSlider::DEADZONE_K12, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K12, 0.2));
        State::set(IndexSlider::DEADZONE_K13, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K13, 0.2));
        State::set(IndexSlider::DEADZONE_K14, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K14, 0.2));
        State::set(IndexSlider::DEADZONE_K15, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K15, 0.2));
        State::set(IndexSlider::DEADZONE_K16, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K16, 0.2));
        State::set(IndexSlider::DEADZONE_K17, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K17, 0.2));
        State::set(IndexSlider::DEADZONE_K18, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K18, 0.2));
        State::set(IndexSlider::DEADZONE_K19, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K19, 0.2));
        State::set(IndexSlider::DEADZONE_S2L, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S2L, 0.2));
        State::set(IndexSlider::DEADZONE_S2R, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_S2R, 0.2));
        State::set(IndexSlider::DEADZONE_K2Start, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K2Start, 0.2));
        State::set(IndexSlider::DEADZONE_K2Select, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K2Select, 0.2));
        State::set(IndexSlider::DEADZONE_K21, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K21, 0.2));
        State::set(IndexSlider::DEADZONE_K22, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K22, 0.2));
        State::set(IndexSlider::DEADZONE_K23, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K23, 0.2));
        State::set(IndexSlider::DEADZONE_K24, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K24, 0.2));
        State::set(IndexSlider::DEADZONE_K25, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K25, 0.2));
        State::set(IndexSlider::DEADZONE_K26, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K26, 0.2));
        State::set(IndexSlider::DEADZONE_K27, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K27, 0.2));
        State::set(IndexSlider::DEADZONE_K28, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K28, 0.2));
        State::set(IndexSlider::DEADZONE_K29, ConfigMgr::Input(keys)->get(I_INPUT_DEADZONE_K29, 0.2));
        State::set(IndexSlider::SPEED_S1A, ConfigMgr::Input(keys)->get(I_INPUT_SPEED_S1A, 0.5));
        State::set(IndexSlider::SPEED_S2A, ConfigMgr::Input(keys)->get(I_INPUT_SPEED_S2A, 0.5));
    }

    updateAllText();
}

void SceneKeyConfig::updateAllText()
{
    auto [pad, slot] = gKeyconfigContext.selecting;
    GameModeKeys keys = gKeyconfigContext.keys;

    const auto input = ConfigMgr::Input(keys);

    State::set(IndexText::KEYCONFIG_1P_BIND_1, input->getBindings(Input::Pad::K11).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_2, input->getBindings(Input::Pad::K12).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_3, input->getBindings(Input::Pad::K13).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_4, input->getBindings(Input::Pad::K14).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_5, input->getBindings(Input::Pad::K15).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_6, input->getBindings(Input::Pad::K16).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_7, input->getBindings(Input::Pad::K17).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_8, input->getBindings(Input::Pad::K18).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_9, input->getBindings(Input::Pad::K19).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_SCRATCH_L, input->getBindings(Input::Pad::S1L).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_SCRATCH_R, input->getBindings(Input::Pad::S1R).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_START, input->getBindings(Input::Pad::K1START).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_SELECT, input->getBindings(Input::Pad::K1SELECT).toString());
    State::set(IndexText::KEYCONFIG_1P_BIND_SCRATCH_ABS, input->getBindings(Input::Pad::S1A).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_1, input->getBindings(Input::Pad::K21).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_2, input->getBindings(Input::Pad::K22).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_3, input->getBindings(Input::Pad::K23).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_4, input->getBindings(Input::Pad::K24).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_5, input->getBindings(Input::Pad::K25).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_6, input->getBindings(Input::Pad::K26).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_7, input->getBindings(Input::Pad::K27).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_8, input->getBindings(Input::Pad::K28).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_9, input->getBindings(Input::Pad::K29).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_SCRATCH_L, input->getBindings(Input::Pad::S2L).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_SCRATCH_R, input->getBindings(Input::Pad::S2R).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_START, input->getBindings(Input::Pad::K2START).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_SELECT, input->getBindings(Input::Pad::K2SELECT).toString());
    State::set(IndexText::KEYCONFIG_2P_BIND_SCRATCH_ABS, input->getBindings(Input::Pad::S2A).toString());
}