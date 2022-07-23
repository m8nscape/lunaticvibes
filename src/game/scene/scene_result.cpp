#include "scene_result.h"
#include "scene_context.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

SceneResult::SceneResult(ePlayMode gamemode) : vScene(eMode::RESULT, 1000), _playmode(gamemode)
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

        if (d1p.total_acc >= 100.0)      gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_0);
        else if (d1p.total_acc >= 88.88) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_1);
        else if (d1p.total_acc >= 77.77) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_2);
        else if (d1p.total_acc >= 66.66) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_3);
        else if (d1p.total_acc >= 55.55) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_4);
        else if (d1p.total_acc >= 44.44) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_5);
        else if (d1p.total_acc >= 33.33) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_6);
        else if (d1p.total_acc >= 22.22) gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_7);
        else                             gOptions.queue(eOption::RESULT_RANK_1P, Option::RANK_8);
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_2P])
    {
        auto d2p = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();
        if (d2p.total_acc >= 100.0)      gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_0);
        else if (d2p.total_acc >= 88.88) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_1);
        else if (d2p.total_acc >= 77.77) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_2);
        else if (d2p.total_acc >= 66.66) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_3);
        else if (d2p.total_acc >= 55.55) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_4);
        else if (d2p.total_acc >= 44.44) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_5);
        else if (d2p.total_acc >= 33.33) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_6);
        else if (d2p.total_acc >= 22.22) gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_7);
        else                             gOptions.queue(eOption::RESULT_RANK_2P, Option::RANK_8);
    }

    // TODO set chart info (total notes, etc.)

    // compare to db record
    auto dp = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();
    auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
    if (pScore)
    {
        gNumbers.queue(eNumber::RESULT_MYBEST_EX, pScore->exscore);
        gNumbers.queue(eNumber::RESULT_MYBEST_DIFF, dp.score2 - pScore->exscore);
        gNumbers.queue(eNumber::RESULT_MYBEST_RATE, (int)std::floor(pScore->rate));
        gNumbers.queue(eNumber::RESULT_MYBEST_RATE_DECIMAL2, (int)std::floor(pScore->rate * 100.0) % 100);

        gNumbers.queue(eNumber::RESULT_RECORD_EX_BEFORE, pScore->exscore);
        gNumbers.queue(eNumber::RESULT_RECORD_EX_NOW, (int)dp.score2);
        gNumbers.queue(eNumber::RESULT_RECORD_EX_DIFF, (int)dp.score2 - pScore->exscore);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_BEFORE, pScore->maxcombo);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_NOW, (int)dp.maxCombo);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_DIFF, (int)dp.maxCombo - pScore->exscore);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_BEFORE, pScore->bp);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_NOW, (int)dp.miss);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_DIFF, (int)dp.miss - pScore->bp);
        gNumbers.queue(eNumber::RESULT_RECORD_MYBEST_RATE, (int)std::floor(pScore->rate));
        gNumbers.queue(eNumber::RESULT_RECORD_MYBEST_RATE_DECIMAL2, (int)std::floor(pScore->rate * 100.0) % 100);

        if      (dp.total_acc >= 100.0) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_0);
        else if (dp.total_acc >= 88.88) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_1);
        else if (dp.total_acc >= 77.77) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_2);
        else if (dp.total_acc >= 66.66) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_3);
        else if (dp.total_acc >= 55.55) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_4);
        else if (dp.total_acc >= 44.44) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_5);
        else if (dp.total_acc >= 33.33) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_6);
        else if (dp.total_acc >= 22.22) gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_7);
        else                            gOptions.queue(eOption::RESULT_UPDATED_RANK, Option::RANK_8);

        if      (pScore->rate >= 100.0) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_0);
        else if (pScore->rate >= 88.88) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_1);
        else if (pScore->rate >= 77.77) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_2);
        else if (pScore->rate >= 66.66) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_3);
        else if (pScore->rate >= 55.55) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_4);
        else if (pScore->rate >= 44.44) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_5);
        else if (pScore->rate >= 33.33) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_6);
        else if (pScore->rate >= 22.22) gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_7);
        else                            gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_8);

        gOptions.queue(eOption::RESULT_UPDATED_RANK, gOptions.get((pScore->exscore > (int)dp.score2) ? eOption::RESULT_MYBEST_RANK : eOption::RESULT_RANK_1P));

        if (pScore->exscore < dp.score2)    gSwitches.queue(eSwitch::RESULT_UPDATED_SCORE, true);
        if (pScore->maxcombo < dp.maxCombo) gSwitches.queue(eSwitch::RESULT_UPDATED_MAXCOMBO, true);
        if (pScore->bp > dp.miss)           gSwitches.queue(eSwitch::RESULT_UPDATED_BP, true);
    }

    int maxScore = gPlayContext.ruleset[PLAYER_SLOT_1P]->getMaxScore();
    if      (dp.total_acc >= 94.44) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * 1.000 - dp.score2));    // MAX-
    else if (dp.total_acc >= 88.88) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .8888));    // AAA+
    else if (dp.total_acc >= 83.33) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .8888 - dp.score2));    // AAA-
    else if (dp.total_acc >= 77.77) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .7777));    // AA+
    else if (dp.total_acc >= 72.22) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .7777 - dp.score2));    // AA-
    else if (dp.total_acc >= 66.66) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .6666));    // A+
    else if (dp.total_acc >= 61.11) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .6666 - dp.score2));    // A-
    else if (dp.total_acc >= 55.55) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .5555));    // B+
    else if (dp.total_acc >= 50.00) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .5555 - dp.score2));    // B-
    else if (dp.total_acc >= 44.44) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .4444));    // C+
    else if (dp.total_acc >= 38.88) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .4444 - dp.score2));    // C-
    else if (dp.total_acc >= 33.33) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .3333));    // D+
    else if (dp.total_acc >= 27.77) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .3333 - dp.score2));    // D-
    else if (dp.total_acc >= 22.22) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(dp.score2 - maxScore * .2222));    // E+
    else if (dp.total_acc >= 11.11) gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, int(maxScore * .2222 - dp.score2));    // E-
    else                            gNumbers.queue(eNumber::RESULT_NEXT_RANK_EX_DIFF, dp.score2);    // F+


    // TODO compare to target

    gNumbers.flush();
    gOptions.flush();
    gSwitches.flush();


    bool cleared = gSwitches.get(eSwitch::RESULT_CLEAR);

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
        //cleared = gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared() || gPlayContext.ruleset[PLAYER_SLOT_2P]->isCleared();
        break;
    }

    default:
        //cleared = gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared();
        break;
    }

    // Moved to play
    //gSwitches.set(eSwitch::RESULT_CLEAR, cleared);

    if (_playmode != ePlayMode::LOCAL_BATTLE && !gChartContext.hash.empty())
    {
        _pScoreOld = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
    }

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
        SoundMgr::playSample(eSoundSample::SOUND_CLEAR);
    else
        SoundMgr::playSample(eSoundSample::SOUND_FAIL);
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
        gTimers.set(eTimer::RESULT_RANK_START, t.norm());
        // TODO play hit sound
        _state = eResultState::STOP;
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

        // save score
        if (_playmode != ePlayMode::LOCAL_BATTLE && !gChartContext.hash.empty())
        {
            assert(gPlayContext.ruleset[PLAYER_SLOT_1P] != nullptr);
            ScoreBMS score;
            auto& format = gChartContext.chartObj;
            auto& chart = gPlayContext.chartObj[PLAYER_SLOT_1P];
            auto& ruleset = gPlayContext.ruleset[PLAYER_SLOT_1P];
            auto& data = ruleset->getData();
            score.notes = chart->getNoteCount();
            score.score = data.score;
            score.rate = data.total_acc;
            score.fast = data.fast;
            score.slow = data.slow;
            score.maxcombo = data.maxCombo;
            score.playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
            switch (format->type())
            {
            case eChartFormat::BMS:
            case eChartFormat::BMSON:
            {
                auto rBMS = std::reinterpret_pointer_cast<RulesetBMS>(ruleset);
                score.exscore = data.score2;

                score.lamp = ScoreBMS::Lamp::NOPLAY;
                if (rBMS->isCleared())
                {
                    if (rBMS->getMaxCombo() == rBMS->getData().maxCombo)
                    {
                        score.lamp = ScoreBMS::Lamp::FULLCOMBO;
                    }
                    else
                    {
                        if (gPlayContext.isCourse)
                        {
                        }
                        else
                        {
                            switch (rBMS->getGaugeType())
                            {
                            case RulesetBMS::GaugeType::GROOVE:  score.lamp = ScoreBMS::Lamp::NORMAL; break;
                            case RulesetBMS::GaugeType::EASY:    score.lamp = ScoreBMS::Lamp::EASY; break;
                            case RulesetBMS::GaugeType::ASSIST:  score.lamp = ScoreBMS::Lamp::ASSIST; break;
                            case RulesetBMS::GaugeType::HARD:    score.lamp = ScoreBMS::Lamp::HARD; break;
                            case RulesetBMS::GaugeType::EXHARD:  score.lamp = ScoreBMS::Lamp::EXHARD; break;
                            case RulesetBMS::GaugeType::DEATH:   score.lamp = ScoreBMS::Lamp::FULLCOMBO; break;
                            case RulesetBMS::GaugeType::P_ATK:   score.lamp = ScoreBMS::Lamp::EASY; break;
                            case RulesetBMS::GaugeType::G_ATK:   score.lamp = ScoreBMS::Lamp::EASY; break;
                            case RulesetBMS::GaugeType::GRADE:   score.lamp = ScoreBMS::Lamp::NOPLAY; break;
                            case RulesetBMS::GaugeType::EXGRADE: score.lamp = ScoreBMS::Lamp::NOPLAY; break;
                            default: break;
                            }
                        }
                    }
                }
                else
                {
                    score.lamp = ScoreBMS::Lamp::FAILED;
                }

                score.pgreat = rBMS->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
                score.great = rBMS->getJudgeCount(RulesetBMS::JudgeType::GREAT);
                score.good = rBMS->getJudgeCount(RulesetBMS::JudgeType::GOOD);
                score.bad = rBMS->getJudgeCount(RulesetBMS::JudgeType::BAD);
                score.bpoor = rBMS->getJudgeCount(RulesetBMS::JudgeType::BPOOR);
                score.miss = rBMS->getJudgeCount(RulesetBMS::JudgeType::MISS);
                score.bp = score.bad + score.bpoor + score.miss;
                score.combobreak = rBMS->getJudgeCount(RulesetBMS::JudgeType::COMBOBREAK);
                g_pScoreDB->updateChartScoreBMS(gChartContext.hash, score);
                break;
            }
            default:
                break;
            }
        }

        // check retry
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
void SceneResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m & INPUT_MASK_DECIDE).any())
    {
        switch (_state)
        {
        case eResultState::DRAW:
            gTimers.set(eTimer::RESULT_RANK_START, t.norm());
            // TODO play hit sound
            _state = eResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eResultState::STOP:
            gTimers.set(eTimer::RESULT_HIGHSCORE_START, t.norm());
            // TODO stop result sound
            // TODO play record sound
            _state = eResultState::RECORD;
            LOG_DEBUG << "[Result] State changed to RECORD";
            break;

        case eResultState::RECORD:
            if (_scoreSyncFinished || true) // debug
            {
                gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
                _state = eResultState::FADEOUT;
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
void SceneResult::inputGameHold(InputMask& m, const Time& t)
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
void SceneResult::inputGameRelease(InputMask& m, const Time& t)
{
    if (t - gTimers.get(eTimer::SCENE_START) < _skin->info.timeIntro) return;
}
