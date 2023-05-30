#include "common/pch.h"
#include "scene_decide.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"
#include "game/data/data_types.h"

namespace lunaticvibes
{

SceneDecide::SceneDecide() : SceneBase(SkinType::DECIDE, 1000)
{
    _type = SceneType::DECIDE;

    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P | INPUT_MASK_2P;

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneDecide::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneDecide::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneDecide::inputGameRelease, this, _1, _2));

    state = eDecideState::START;
    _updateCallback = std::bind(&SceneDecide::updateStart, this);

    //if (!LR2CustomizeData.isInCustomize)
    {
        SoundMgr::stopSysSamples();
        SoundMgr::setSysVolume(1.0);
        SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::BGM_DECIDE);
    }
}

SceneDecide::~SceneDecide()
{
    _input.loopEnd();
    loopEnd();
}

////////////////////////////////////////////////////////////////////////////////

void SceneDecide::_updateAsync()
{
    if (SystemData.gNextScene != SceneType::DECIDE) return;

    if (SystemData.isAppExiting)
    {
        SystemData.gNextScene = SceneType::EXIT_TRANS;
    }

    _updateCallback();
}

void SceneDecide::updateStart()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];

    if (rt.norm() >= pSkin->info.timeDecideExpiry)
    {
        SystemData.gNextScene = SceneType::PLAY;
    }
}

void SceneDecide::updateSkip()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];
    auto ft = t - SystemData.timers["fadeout_start"];

    if (ft.norm() >= pSkin->info.timeOutro)
    {
        SystemData.gNextScene = SceneType::PLAY;
    }
}

void SceneDecide::updateCancel()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];
    auto ft = t - SystemData.timers["fadeout_start"];

    if (ft.norm() >= pSkin->info.timeOutro)
    {
        PlayData.clearContextPlay();
        PlayData.isAuto = false;
        PlayData.isReplay = false;
        SystemData.gNextScene = SceneType::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneDecide::inputGamePress(InputMask& m, Time t)
{
    unsigned rt = (t - SystemData.timers["scene_start"]).norm();
    if (rt < pSkin->info.timeIntro) return;

    auto k = _inputAvailable & m;
    if ((k & INPUT_MASK_DECIDE).any() && rt >= pSkin->info.timeDecideSkip)
    {
        switch (state)
        {
        case eDecideState::START:
            SystemData.timers["fadeout_start"] = t.norm();
            _updateCallback = std::bind(&SceneDecide::updateSkip, this);
            state = eDecideState::SKIP;
            LOG_DEBUG << "[Decide] State changed to SKIP";
            break;

        default:
            break;
        }
    }

    if (!ArenaData.isOnline())
    {
        if (k[Input::ESC])
        {
            switch (state)
            {
            case eDecideState::START:
                SystemData.timers["fadeout_start"] = t.norm();
                SoundMgr::stopSysSamples();
                _updateCallback = std::bind(&SceneDecide::updateCancel, this);
                state = eDecideState::CANCEL;
                LOG_DEBUG << "[Decide] State changed to CANCEL";
                break;

            default:
                break;
            }
        }
    }
}

// CALLBACK
void SceneDecide::inputGameHold(InputMask& m, Time t)
{
    unsigned rt = (t - SystemData.timers["scene_start"]).norm();
    if (rt < pSkin->info.timeIntro) return;

    auto k = _inputAvailable & m;

    if (!ArenaData.isOnline())
    {
        if ((k[Input::K1START] && k[Input::K1SELECT]) || (k[Input::K2START] && k[Input::K2SELECT]))
        {
            switch (state)
            {
            case eDecideState::START:
                SystemData.timers["fadeout_start"] = t.norm();
                SoundMgr::stopSysSamples();
                _updateCallback = std::bind(&SceneDecide::updateCancel, this);
                state = eDecideState::CANCEL;
                LOG_DEBUG << "[Decide] State changed to CANCEL";
                break;

            default:
                break;
            }
        }
    }
}

// CALLBACK
void SceneDecide::inputGameRelease(InputMask& m, Time t)
{
    unsigned rt = (t - SystemData.timers["scene_start"]).norm();
    if (rt < pSkin->info.timeIntro) return;
}

}
