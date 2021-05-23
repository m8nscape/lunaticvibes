#include "scene_result.h"
#include "scene_context.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

SceneResult::SceneResult() : vScene(eMode::RESULT, 1000)
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

    _state = eResultState::DRAW;

    // set options
    if (gPlayContext.ruleset[PLAYER_SLOT_1P])
    {
        auto d1p = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();

        if (d1p.total_acc >= 100.0)      gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_0);
        else if (d1p.total_acc >= 88.88) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_1);
        else if (d1p.total_acc >= 77.77) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_2);
        else if (d1p.total_acc >= 66.66) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_3);
        else if (d1p.total_acc >= 55.55) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_4);
        else if (d1p.total_acc >= 44.44) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_5);
        else if (d1p.total_acc >= 33.33) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_6);
        else if (d1p.total_acc >= 22.22) gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_7);
        else                             gOptions.set(eOption::RESULT_RANK_1P, Option::RANK_8);
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_2P])
    {
        auto d2p = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();
        if (d2p.total_acc >= 100.0)      gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_0);
        else if (d2p.total_acc >= 88.88) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_1);
        else if (d2p.total_acc >= 77.77) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_2);
        else if (d2p.total_acc >= 66.66) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_3);
        else if (d2p.total_acc >= 55.55) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_4);
        else if (d2p.total_acc >= 44.44) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_5);
        else if (d2p.total_acc >= 33.33) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_6);
        else if (d2p.total_acc >= 22.22) gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_7);
        else                             gOptions.set(eOption::RESULT_RANK_2P, Option::RANK_8);
    }

    // TODO compare to db record
    auto dp = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();

    bool cleared = false;

    switch (gPlayContext.mode)
    {
    case eMode::PLAY5_2:
    case eMode::PLAY7_2:
    case eMode::PLAY9_2:
    {
        auto d1p = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();
        auto d2p = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();

        // TODO WIN/LOSE
        /*
        switch (context_play.rulesetType)
        {
        case eRuleset::CLASSIC:
            if (d1p.score2 > d2p.score2)
                // TODO
                break;

        default:
            if (d1p.score > d2p.score)
                break;
        }
        */

        // clear or failed?
        bool c1, c2;
        switch (gPlayContext.gaugeType[PLAYER_SLOT_1P])
        {
        case eGaugeOp::GROOVE:
            c1 = d1p.health >= 0.80;
            break;
        default:
            c1 = d1p.health > 0.0;
            break;
        }
        switch (gPlayContext.gaugeType[PLAYER_SLOT_2P])
        {
        case eGaugeOp::GROOVE:
            c2 = d2p.health >= 0.80;
            break;
        default:
            c2 = d2p.health > 0.0;
            break;
        }
        cleared = c1 || c2;

        break;
    }

    default:
        switch (gPlayContext.gaugeType[PLAYER_SLOT_1P])
        {
        case eGaugeOp::GROOVE:
            cleared = dp.health >= 0.80;
            break;
        default:
            cleared = dp.health > 0.0;
            break;
        }
        break;
    }

    gSwitches.set(eSwitch::RESULT_CLEAR, cleared);

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneResult::inputGameRelease, this, _1, _2));

    Time t;
    gTimers.set(eTimer::RESULT_GRAPH_START, t.norm());

    loopStart();
    _input.loopStart();

    SoundMgr::stopSamples();
    if (cleared) 
        SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_CLEAR));
    else
        SoundMgr::playSample(static_cast<size_t>(eSoundSample::SOUND_FAIL));
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

    if (rt.norm() >= _skin->info.timeResultRank)
    {
        _state = eResultState::STOP;
        gTimers.set(eTimer::RESULT_RANK_START, t.norm());
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
        SoundMgr::stopKeySamples();
        if (_retryRequested && gPlayContext.canRetry)
        {
            clearContextPlayForRetry();
            gNextScene = eScene::PLAY;
        }
        else
        {
            clearContextPlay();
            gNextScene = gQuitOnFinish ? eScene::EXIT : eScene::SELECT;
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
            LOG_DEBUG << "[Result] State changed to RECORD";
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
