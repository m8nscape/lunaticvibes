#include "scene_keyconfig.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"

SceneKeyConfig::SceneKeyConfig() : vScene(eMode::KEY_CONFIG, 240)
{
    _scene = eScene::KEYCONFIG;

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

    LOG_DEBUG << "[KeyConfig] Start";
}

SceneKeyConfig::~SceneKeyConfig()
{
    _input.loopEnd();
    loopEnd();
}

void SceneKeyConfig::_updateAsync()
{
    if (gNextScene != eScene::KEYCONFIG) return;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
    }

    _updateCallback();

    updateForceBargraphs();
}

void SceneKeyConfig::updateStart()
{
    Time t;
    Time rt = t - State::get(IndexTimer::SCENE_START);
    if (rt.norm() > _skin->info.timeIntro)
    {
        _updateCallback = std::bind(&SceneKeyConfig::updateMain, this);
        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneKeyConfig::inputGamePress, this, _1, _2));
        _input.register_kb("SCENE_KEYPRESS", std::bind(&SceneKeyConfig::inputGamePressKeyboard, this, _1, _2));
        _input.register_joy("SCENE_JOYPRESS", std::bind(&SceneKeyConfig::inputGamePressJoystick, this, _1, _2, _3));
        _input.register_aa("SCENE_ABSOLUTEAXIS", std::bind(&SceneKeyConfig::inputGameAbsoluteAxis, this, _1, _2, _3));
        LOG_DEBUG << "[KeyConfig] State changed to Main";
    }
}

void SceneKeyConfig::updateMain()
{
    Time t;
    if (_exiting)
    {
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        _updateCallback = std::bind(&SceneKeyConfig::updateFadeout, this);
        using namespace std::placeholders;
        _input.unregister_p("SCENE_PRESS");
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

    if (rt.norm() > _skin->info.timeOutro)
    {
        ConfigMgr::Input(gKeyconfigContext.keys)->save();   // this is kinda important
        gNextScene = eScene::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneKeyConfig::inputGamePress(InputMask& m, const Time& t)
{
    if (m[Input::Pad::ESC]) _exiting = true;
    if (m[Input::Pad::M2]) _exiting = true;

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
        if (mask[static_cast<size_t>(Input::Keyboard::K_DEL)])
        {
            KeyMap undef;
            ConfigMgr::Input(keys)->bind(pad, undef);
            InputMgr::updateBindings(keys, pad);
            updateInfo(undef, slot);
        }

        if (pad == Input::Pad::S1A || pad == Input::Pad::S2A)
            return;

        if (mask[static_cast<size_t>(Input::Keyboard::K_F1)])
        {
            ConfigMgr::Input(keys)->clearAll();
        }
        else if (mask[static_cast<size_t>(Input::Keyboard::K_F2)])
        {
            ConfigMgr::Input(keys)->setDefaults();
        }
        else
        {
            for (Input::Keyboard k = Input::Keyboard::K_1; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
            {
                if (mask[static_cast<size_t>(k)])
                {
                    // modify slot
                    KeyMap km(k);
                    ConfigMgr::Input(keys)->bind(pad, km);
                    InputMgr::updateBindings(keys, pad);

                    updateInfo(km, slot);
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
                }
                joystickPrev[device][bit] = mask[bit];
            }
        }
        base += InputMgr::MAX_JOYSTICK_POV_COUNT * 4;

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
            }
            joystickPrev[device][bit] = mask[bit];
        }
    }
}

void SceneKeyConfig::inputGameAbsoluteAxis(JoystickAxis& axis, size_t device, const Time&)
{
    auto [pad, slot] = gKeyconfigContext.selecting;
    if (pad != Input::Pad::S1A && pad != Input::Pad::S2A)
        return;

    GameModeKeys keys = gKeyconfigContext.keys;

    for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
    {
        if (axis[index] != -1.0)
        {
            auto& bindings = ConfigMgr::Input(keys)->getBindings(pad);
            if (bindings.getType() != KeyMap::DeviceType::JOYSTICK ||
                bindings.getJoystick().index != index)
            {
                KeyMap j(device, Input::Joystick::Type::AXIS_ABSOLUTE, index);
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);
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
    GameModeKeys keys = gKeyconfigContext.keys;
    auto& input = ConfigMgr::Input(keys);
    Time t;

    // update keyboard force bargraph
    for (Input::Keyboard k = Input::Keyboard::K_1; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
    {
        auto& input = ConfigMgr::Input(keys);
        for (const auto& [p, bar] : forceBargraphMap)
        {
            auto& binding = input->getBindings(p);
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
            auto& binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::BUTTON &&
                binding.getJoystick().index == index)
            {
                if (isButtonPressed(binding.getJoystick(), 0.0));
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
            auto& binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::POV &&
                (binding.getJoystick().index & 0xFFFF) == index)
            {
                if (isButtonPressed(binding.getJoystick(), 0.0));
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
            auto& binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::AXIS_RELATIVE_POSITIVE &&
                binding.getJoystick().index == index)
            {
                double axis = _input.getJoystickAxis(binding.getJoystick().device, Input::Joystick::Type::AXIS_RELATIVE_POSITIVE, index);
                if (axis >= 0.0 && axis <= 1.0)
                {
                    forceBargraphTriggerTimestamp[p] = 0;
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
            auto& binding = input->getBindings(p);
            if (binding.getType() == KeyMap::DeviceType::JOYSTICK &&
                binding.getJoystick().type == Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE &&
                binding.getJoystick().index == index)
            {
                double axis = _input.getJoystickAxis(binding.getJoystick().device, Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE, index);
                if (axis >= 0.0 && axis <= 1.0)
                {
                    forceBargraphTriggerTimestamp[p] = 0;
                    State::set(bar, axis);
                }
            }
        }
    }

    for (auto& [pad, bar] : forceBargraphMap)
    {
        if (forceBargraphTriggerTimestamp.find(pad) != forceBargraphTriggerTimestamp.end() &&
            forceBargraphTriggerTimestamp[pad] != 0 &&
            t - forceBargraphTriggerTimestamp[pad] > 500)  // 1s timeout
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

    // play sound
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}