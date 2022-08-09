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


void SceneKeyConfig::_updateAsync()
{
    if (gNextScene != eScene::KEYCONFIG) return;

    if (gAppIsExiting)
    {
        _skin->stopSpriteVideoUpdate();
        gNextScene = eScene::EXIT_TRANS;
    }

    _updateCallback();
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
        LOG_DEBUG << "[KeyConfig] State changed to Fadeout";
    }
}

void SceneKeyConfig::updateFadeout()
{
    Time t;
    Time rt = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (rt.norm() > _skin->info.timeOutro)
    {
        _skin->stopSpriteVideoUpdate();
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
            gTimers.set(InputGamePressMapSingle[i].tm, t.norm());
            gTimers.set(InputGameReleaseMapSingle[i].tm, TIMER_NEVER);
            gSwitches.set(InputGamePressMapSingle[i].sw, true);
        }
    }
}


void SceneKeyConfig::inputGamePressKeyboard(KeyboardMask& mask, const Time& t)
{
    // update bindings
    auto [pad, slot] = gKeyconfigContext.selecting;
    slot = 0;
    for (Input::Keyboard k = Input::Keyboard::K_1; k != Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
    {
        if (mask[static_cast<size_t>(k)])
        {
            GameModeKeys keys = gKeyconfigContext.keys;

            // modify slot
            KeyMap km(k);
            ConfigMgr::Input(keys)->bind(pad, km);
            InputMgr::updateBindings(keys, pad);

            // update text
            switch (slot)
            {
                case 0: gTexts.set(eText::KEYCONFIG_SLOT1,  km.toString()); break;
                case 1: gTexts.set(eText::KEYCONFIG_SLOT2,  km.toString()); break;
                case 2: gTexts.set(eText::KEYCONFIG_SLOT3,  km.toString()); break;
                case 3: gTexts.set(eText::KEYCONFIG_SLOT4,  km.toString()); break;
                case 4: gTexts.set(eText::KEYCONFIG_SLOT5,  km.toString()); break;
                case 5: gTexts.set(eText::KEYCONFIG_SLOT6,  km.toString()); break;
                case 6: gTexts.set(eText::KEYCONFIG_SLOT7,  km.toString()); break;
                case 7: gTexts.set(eText::KEYCONFIG_SLOT8,  km.toString()); break;
                case 8: gTexts.set(eText::KEYCONFIG_SLOT9,  km.toString()); break;
                case 9: gTexts.set(eText::KEYCONFIG_SLOT10, km.toString()); break;
                default: break;
            }

            // play sound
            SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
        }
    }
}

void SceneKeyConfig::inputGamePressJoystick(JoystickMask& mask, size_t device, const Time& t)
{
    // update bindings
    auto [pad, slot] = gKeyconfigContext.selecting;
    slot = 0;

    auto updateInfo = [&](KeyMap j, int slot)
    {
        // update text
        switch (slot)
        {
        case 0: gTexts.set(eText::KEYCONFIG_SLOT1, j.toString()); break;
        case 1: gTexts.set(eText::KEYCONFIG_SLOT2, j.toString()); break;
        case 2: gTexts.set(eText::KEYCONFIG_SLOT3, j.toString()); break;
        case 3: gTexts.set(eText::KEYCONFIG_SLOT4, j.toString()); break;
        case 4: gTexts.set(eText::KEYCONFIG_SLOT5, j.toString()); break;
        case 5: gTexts.set(eText::KEYCONFIG_SLOT6, j.toString()); break;
        case 6: gTexts.set(eText::KEYCONFIG_SLOT7, j.toString()); break;
        case 7: gTexts.set(eText::KEYCONFIG_SLOT8, j.toString()); break;
        case 8: gTexts.set(eText::KEYCONFIG_SLOT9, j.toString()); break;
        case 9: gTexts.set(eText::KEYCONFIG_SLOT10, j.toString()); break;
        default: break;
        }

        // play sound
        SoundMgr::playSysSample(SoundChannelType::KEY_SYS, eSoundSample::SOUND_O_CHANGE);
    };

    GameModeKeys keys = gKeyconfigContext.keys;
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
        if      (mask[static_cast<size_t>(base + index * 4 + 0)])
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
            KeyMap j(device, Input::Joystick::Type::AXIS_RELATIVE_NEGATIVE, index);
            ConfigMgr::Input(keys)->bind(pad, j);
            InputMgr::updateBindings(keys, pad);
            updateInfo(j, slot);
        }
    }
    base += InputMgr::MAX_JOYSTICK_AXIS_COUNT;
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