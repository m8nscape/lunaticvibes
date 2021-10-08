#include "scene_decide.h"
#include "scene_context.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

SceneDecide::SceneDecide() : vScene(eMode::DECIDE, 1000)
{
    _inputAvailable = INPUT_MASK_FUNC;

    if (gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    if (gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneDecide::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneDecide::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneDecide::inputGameRelease, this, _1, _2));

    _state = eDecideState::START;
    _updateCallback = std::bind(&SceneDecide::updateStart, this);

    loopStart();
    _input.loopStart();

    SoundMgr::stopSamples();
    SoundMgr::playSample(static_cast<size_t>(eSoundSample::BGM_DECIDE));
}

////////////////////////////////////////////////////////////////////////////////

void SceneDecide::_updateAsync()
{
    _updateCallback();
}

void SceneDecide::updateStart()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeDecideExpiry)
    {
        loopEnd();
        _input.loopEnd();
        gNextScene = eScene::PLAY;
    }
}

void SceneDecide::updateSkip()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft.norm() >= _skin->info.timeDecideSkip)
    {
        loopEnd();
        _input.loopEnd();
        gNextScene = eScene::PLAY;
    }
}

void SceneDecide::updateCancel()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (rt.norm() >= _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        clearContextPlay();
        gNextScene = eScene::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneDecide::inputGamePress(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m & INPUT_MASK_DECIDE).any())
    {
        switch (_state)
        {
        case eDecideState::START:
            _state = eDecideState::SKIP;
            _updateCallback = std::bind(&SceneDecide::updateSkip, this);
            LOG_DEBUG << "[Decide] State changed to SKIP";
            break;

        default:
            break;
        }
    }

    if ((_inputAvailable & m)[Input::ESC])
    {
        switch (_state)
        {
        case eDecideState::START:
            _state = eDecideState::CANCEL;
            _updateCallback = std::bind(&SceneDecide::updateCancel, this);
            LOG_DEBUG << "[Decide] State changed to CANCEL";
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneDecide::inputGameHold(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    auto k = _inputAvailable & m;
    if ((k[Input::K1START] && k[Input::K1SELECT]) || (k[Input::K2START] && k[Input::K2SELECT]))
    {
        switch (_state)
        {
        case eDecideState::START:
            _state = eDecideState::CANCEL;
            _updateCallback = std::bind(&SceneDecide::updateCancel, this);
            LOG_DEBUG << "[Decide] State changed to CANCEL";
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneDecide::inputGameRelease(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}
