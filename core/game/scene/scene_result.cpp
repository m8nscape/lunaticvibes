#include "scene_result.h"
#include "scene_context.h"

SceneResult::SceneResult() : vScene(eMode::RESULT, 1000)
{
    _inputAvailable = INPUT_MASK_FUNC;

    if (context_play.chartObj[PLAYER_SLOT_1P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    if (context_play.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    _state = eResultState::DRAW;

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneResult::inputGameRelease, this, _1, _2));

    Time t;
    gTimers.set(eTimer::RESULT_GRAPH_START, t.norm());

    loopStart();
    _input.loopStart();
}

////////////////////////////////////////////////////////////////////////////////

void SceneResult::_updateAsync()
{
    std::unique_lock<decltype(_mutex)> _lock(_mutex, std::try_to_lock);
    if (!_lock.owns_lock()) return;

    switch (_state)
    {
    case eResultState::DRAW:
        updateDraw();
        break;
    case eResultState::STOP:
        updateStop();
        break;
    case eResultState::RECORD:
        updateRecord();
        break;
    case eResultState::FADEOUT:
        updateFadeout();
        break;
    }
}

void SceneResult::updateDraw()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeResultDrawing)
    {
        _state = eResultState::STOP;
        // TODO play hit sound
        LOG_DEBUG << "[Result] State changed to STOP";
    }
}

void SceneResult::updateStop()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneResult::updateRecord()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
}

void SceneResult::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        loopEnd();
        _input.loopEnd();
        if (_retryRequested && context_play.canRetry)
        {
            clearContextPlayForRetry();
            __next_scene = eScene::PLAY;
        }
        else
        {
            clearContextPlay();
            // TODO return to select
            __next_scene = eScene::EXIT;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneResult::inputGamePress(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m & INPUT_MASK_DECIDE).any())
    {
        switch (_state)
        {
        case eResultState::DRAW:
            _state = eResultState::STOP;
            gTimers.set(eTimer::RESULT_RANK_START, t.norm());
            // TODO play hit sound
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eResultState::STOP:
            _state = eResultState::RECORD;
            gTimers.set(eTimer::RESULT_HIGHSCORE_START, t.norm());
            // TODO stop result sound
            // TODO play record sound
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eResultState::RECORD:
            if (_scoreSyncFinished || true) // debug
            {
                _state = eResultState::FADEOUT;
                gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eResultState::FADEOUT:
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneResult::inputGameHold(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if (_state == eResultState::FADEOUT)
    {
        _retryRequested =
            (_inputAvailable & m & INPUT_MASK_DECIDE).any() && 
            (_inputAvailable & m & INPUT_MASK_CANCEL).any();
    }
}

// CALLBACK
void SceneResult::inputGameRelease(InputMask& m, Time t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}
