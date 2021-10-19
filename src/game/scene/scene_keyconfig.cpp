#include "scene_keyconfig.h"
#include "scene_context.h"
#include "config/config_mgr.h"
#include "game/sound/sound_mgr.h"

SceneKeyConfig::SceneKeyConfig() : vScene(eMode::KEY_CONFIG, 240)
{
    _updateCallback = std::bind(&SceneKeyConfig::updateStart, this);

    gKeyconfigContext.selecting = { Input::Pad::K11, 0 };
    gKeyconfigContext.keys = 7;
    gOptions.set(eOption::KEY_CONFIG_MODE, Option::KEYCFG_7);
    setText(gKeyconfigContext.keys, gKeyconfigContext.selecting.first);

    loopStart();
    _input.loopStart();
    LOG_DEBUG << "[KeyConfig] Start";
}


void SceneKeyConfig::_updateAsync()
{
    _updateCallback();
}

void SceneKeyConfig::updateStart()
{
    Time t;
    if (t.norm() > _skin->info.timeIntro)
    {
        _updateCallback = std::bind(&SceneKeyConfig::updateMain, this);
        using namespace std::placeholders;
        _input.register_p("SCENE_PRESS", std::bind(&SceneKeyConfig::inputGamePress, this, _1, _2));
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
        LOG_DEBUG << "[KeyConfig] State changed to Fadeout";
    }
}

void SceneKeyConfig::updateFadeout()
{
    Time t;
    if (t.norm() > _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        ConfigMgr::Input(gKeyconfigContext.keys)->save();   // this is kinda important
        gNextScene = eScene::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneKeyConfig::inputGamePress(InputMask& m, const Time& t)
{
    if (m[Input::Pad::ESC]) _exiting = true;

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
    for (Input::Keyboard k = Input::Keyboard::K_ESC; k < Input::Keyboard::K_COUNT; ++ * (unsigned*)&k)
    {
        if (mask[k])
        {
            if (k < Input::Keyboard::K_COUNT && slot < Input::MAX_BINDINGS_PER_KEY)
            {
                int keys = gKeyconfigContext.keys;

                // modify slot
                ConfigMgr::Input(keys)->bindKey(pad, k, slot);
                InputMgr::updateBindings(keys, pad);

                // update text
                switch (slot)
                {
                    case 0: gTexts.set(eText::KEYCONFIG_SLOT1, ConfigInput::getKeyString(k)); break;
                    case 1: gTexts.set(eText::KEYCONFIG_SLOT2, ConfigInput::getKeyString(k)); break;
                    case 2: gTexts.set(eText::KEYCONFIG_SLOT3, ConfigInput::getKeyString(k)); break;
                    case 3: gTexts.set(eText::KEYCONFIG_SLOT4, ConfigInput::getKeyString(k)); break;
                    case 4: gTexts.set(eText::KEYCONFIG_SLOT5, ConfigInput::getKeyString(k)); break;
                    case 5: gTexts.set(eText::KEYCONFIG_SLOT6, ConfigInput::getKeyString(k)); break;
                    case 6: gTexts.set(eText::KEYCONFIG_SLOT7, ConfigInput::getKeyString(k)); break;
                    case 7: gTexts.set(eText::KEYCONFIG_SLOT8, ConfigInput::getKeyString(k)); break;
                    case 8: gTexts.set(eText::KEYCONFIG_SLOT9, ConfigInput::getKeyString(k)); break;
                    case 9: gTexts.set(eText::KEYCONFIG_SLOT10, ConfigInput::getKeyString(k)); break;
                    default: break;
                }

                // play sound
                SoundMgr::playSample(eSoundSample::SOUND_O_CHANGE);
            }
        }
    }
}

void SceneKeyConfig::setText(int keys, Input::Pad pad)
{
    auto bindings = ConfigMgr::Input(keys)->getBindings(pad);
    for (size_t i = 0; i < std::min(bindings.size(), Input::MAX_BINDINGS_PER_KEY); ++i)
    {
        gTexts.set(eText(unsigned(eText::KEYCONFIG_SLOT1) + i), ConfigInput::getKeyString(bindings[i]));
    }
}