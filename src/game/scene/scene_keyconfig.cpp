#include "scene_keyconfig.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"

SceneKeyConfig::SceneKeyConfig() : vScene(eMode::KEY_CONFIG, 240)
{
    _scene = eScene::KEYCONFIG;

    _updateCallback = std::bind(&SceneKeyConfig::updateStart, this);

    gSwitches.set(eSwitch::K11_CONFIG, false);
    gSwitches.set(eSwitch::K12_CONFIG, false);
    gSwitches.set(eSwitch::K13_CONFIG, false);
    gSwitches.set(eSwitch::K14_CONFIG, false);
    gSwitches.set(eSwitch::K15_CONFIG, false);
    gSwitches.set(eSwitch::K16_CONFIG, false);
    gSwitches.set(eSwitch::K17_CONFIG, false);
    gSwitches.set(eSwitch::K18_CONFIG, false);
    gSwitches.set(eSwitch::K19_CONFIG, false);
    gSwitches.set(eSwitch::S1L_CONFIG, false);
    gSwitches.set(eSwitch::S1R_CONFIG, false);
    gSwitches.set(eSwitch::S1A_CONFIG, false);
    gSwitches.set(eSwitch::K1START_CONFIG, false);
    gSwitches.set(eSwitch::K1SELECT_CONFIG, false);
    gSwitches.set(eSwitch::K1SPDUP_CONFIG, false);
    gSwitches.set(eSwitch::K1SPDDN_CONFIG, false);
    gSwitches.set(eSwitch::K21_CONFIG, false);
    gSwitches.set(eSwitch::K22_CONFIG, false);
    gSwitches.set(eSwitch::K23_CONFIG, false);
    gSwitches.set(eSwitch::K24_CONFIG, false);
    gSwitches.set(eSwitch::K25_CONFIG, false);
    gSwitches.set(eSwitch::K26_CONFIG, false);
    gSwitches.set(eSwitch::K27_CONFIG, false);
    gSwitches.set(eSwitch::K28_CONFIG, false);
    gSwitches.set(eSwitch::K29_CONFIG, false);
    gSwitches.set(eSwitch::S2L_CONFIG, false);
    gSwitches.set(eSwitch::S2R_CONFIG, false);
    gSwitches.set(eSwitch::S2A_CONFIG, false);
    gSwitches.set(eSwitch::K2START_CONFIG, false);
    gSwitches.set(eSwitch::K2SELECT_CONFIG, false);
    gSwitches.set(eSwitch::K2SPDUP_CONFIG, false);
    gSwitches.set(eSwitch::K2SPDDN_CONFIG, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT0, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT1, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT2, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT3, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT4, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT5, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT6, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT7, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT8, false);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT9, false);

    gKeyconfigContext.selecting = { Input::Pad::K11, 0 };
    gKeyconfigContext.keys = 7;
    gSwitches.set(eSwitch::K11_CONFIG, true);
    gSwitches.set(eSwitch::KEY_CONFIG_SLOT0, true);
    gOptions.set(eOption::KEY_CONFIG_MODE, Option::KEYCFG_7);
    gOptions.set(eOption::KEY_CONFIG_KEY5, 1);
    gOptions.set(eOption::KEY_CONFIG_KEY7, 1);
    gOptions.set(eOption::KEY_CONFIG_KEY9, 1);
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
    Time rt = t - gTimers.get(eTimer::SCENE_START);
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
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
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
    Time rt = t - gTimers.get(eTimer::FADEOUT_BEGIN);

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
            gTimers.set(InputGamePressMap[i].tm, t.norm());
            gTimers.set(InputGameReleaseMap[i].tm, TIMER_NEVER);
            gSwitches.set(InputGamePressMap[i].sw, true);
        }
    }
}

static const std::map<Input::Pad, eBargraph> forceBargraphMap =
{
    { Input::Pad::S1L,      eBargraph::FORCE_S1L },
    { Input::Pad::S1R,      eBargraph::FORCE_S1R },
    { Input::Pad::K1START,  eBargraph::FORCE_K1Start },
    { Input::Pad::K1SELECT, eBargraph::FORCE_K1Select },
    { Input::Pad::S2L,      eBargraph::FORCE_S2L },
    { Input::Pad::S2R,      eBargraph::FORCE_S2R },
    { Input::Pad::K2START,  eBargraph::FORCE_K2Start },
    { Input::Pad::K2SELECT, eBargraph::FORCE_K2Select },
    { Input::Pad::K11,      eBargraph::FORCE_K11 },
    { Input::Pad::K12,      eBargraph::FORCE_K12 },
    { Input::Pad::K13,      eBargraph::FORCE_K13 },
    { Input::Pad::K14,      eBargraph::FORCE_K14 },
    { Input::Pad::K15,      eBargraph::FORCE_K15 },
    { Input::Pad::K16,      eBargraph::FORCE_K16 },
    { Input::Pad::K17,      eBargraph::FORCE_K17 },
    { Input::Pad::K18,      eBargraph::FORCE_K18 },
    { Input::Pad::K19,      eBargraph::FORCE_K19 },
    { Input::Pad::K21,      eBargraph::FORCE_K21 },
    { Input::Pad::K22,      eBargraph::FORCE_K22 },
    { Input::Pad::K23,      eBargraph::FORCE_K23 },
    { Input::Pad::K24,      eBargraph::FORCE_K24 },
    { Input::Pad::K25,      eBargraph::FORCE_K25 },
    { Input::Pad::K26,      eBargraph::FORCE_K26 },
    { Input::Pad::K27,      eBargraph::FORCE_K27 },
    { Input::Pad::K28,      eBargraph::FORCE_K28 },
    { Input::Pad::K29,      eBargraph::FORCE_K29 },
};

void SceneKeyConfig::inputGamePressKeyboard(KeyboardMask& mask, const Time& t)
{
    // update bindings
    auto [pad, slot] = gKeyconfigContext.selecting;
    if (pad == Input::Pad::S1A || pad == Input::Pad::S2A)
        return;

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
        else if (mask[static_cast<size_t>(Input::Keyboard::K_F1)])
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
        for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_BUTTON_COUNT; ++index)
        {
            if (mask[static_cast<size_t>(base + index)])
            {
                KeyMap j(device, Input::Joystick::Type::BUTTON, index);
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);
            }
        }
        base += InputMgr::MAX_JOYSTICK_BUTTON_COUNT;

        for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_POV_COUNT; ++index)
        {
            if (mask[static_cast<size_t>(base + index * 4 + 0)])
            {
                KeyMap j(device, Input::Joystick::Type::POV, index | (1ul << 31));
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);
            }
            else if (mask[static_cast<size_t>(base + index * 4 + 1)])
            {
                KeyMap j(device, Input::Joystick::Type::POV, index | (1ul << 30));
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);
            }
            else if (mask[static_cast<size_t>(base + index * 4 + 2)])
            {
                KeyMap j(device, Input::Joystick::Type::POV, index | (1ul << 29));
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);
            }
            else if (mask[static_cast<size_t>(base + index * 4 + 3)])
            {
                KeyMap j(device, Input::Joystick::Type::POV, index | (1ul << 28));
                ConfigMgr::Input(keys)->bind(pad, j);
                InputMgr::updateBindings(keys, pad);
                updateInfo(j, slot);
            }
        }
        base += InputMgr::MAX_JOYSTICK_POV_COUNT * 4;

        for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
        {
            if (mask[static_cast<size_t>(base + index)])
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
        }
        base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;

        for (size_t index = 0; index < InputMgr::MAX_JOYSTICK_AXIS_COUNT; ++index)
        {
            if (mask[static_cast<size_t>(base + index)])
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
    gTexts.set(eText::KEYCONFIG_SLOT1, bindings.toString());
    for (size_t i = 1; i < 10; ++i)
    {
        gTexts.set(eText(unsigned(eText::KEYCONFIG_SLOT1) + i), "-");
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
                    gBargraphs.set(bar, 1.0);
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
                    gBargraphs.set(bar, 1.0);
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
                    gBargraphs.set(bar, 1.0);
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
                    gBargraphs.set(bar, axis);
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
                    gBargraphs.set(bar, axis);
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
            gBargraphs.set(bar, 0.0);
        }
    }
}

void SceneKeyConfig::updateInfo(KeyMap k, int slot)
{
    slot = 0;

    // update text
    switch (slot)
    {
    case 0: gTexts.set(eText::KEYCONFIG_SLOT1, k.toString()); break;
    case 1: gTexts.set(eText::KEYCONFIG_SLOT2, k.toString()); break;
    case 2: gTexts.set(eText::KEYCONFIG_SLOT3, k.toString()); break;
    case 3: gTexts.set(eText::KEYCONFIG_SLOT4, k.toString()); break;
    case 4: gTexts.set(eText::KEYCONFIG_SLOT5, k.toString()); break;
    case 5: gTexts.set(eText::KEYCONFIG_SLOT6, k.toString()); break;
    case 6: gTexts.set(eText::KEYCONFIG_SLOT7, k.toString()); break;
    case 7: gTexts.set(eText::KEYCONFIG_SLOT8, k.toString()); break;
    case 8: gTexts.set(eText::KEYCONFIG_SLOT9, k.toString()); break;
    case 9: gTexts.set(eText::KEYCONFIG_SLOT10, k.toString()); break;
    default: break;
    }

    // play sound
    SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
}