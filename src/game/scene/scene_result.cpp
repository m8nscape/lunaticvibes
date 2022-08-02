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
    auto d1p = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();
    gOptions.queue(eOption::RESULT_RANK_1P, Option::getRankType(d1p.total_acc));
    gPlayContext.ruleset[PLAYER_SLOT_1P]->updateGlobals();

    auto d2p = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();
    gOptions.queue(eOption::RESULT_RANK_2P, Option::getRankType(d2p.total_acc));
    gPlayContext.ruleset[PLAYER_SLOT_2P]->updateGlobals();

    gNumbers.queue(eNumber::PLAY_1P_EXSCORE_DIFF, d1p.score2 - d2p.score2);
    gNumbers.queue(eNumber::PLAY_2P_EXSCORE_DIFF, d2p.score2 - d1p.score2);

    // TODO set chart info (total notes, etc.)
    auto chartLength = gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() / 1000;
    gNumbers.queue(eNumber::PLAY_MIN, int(chartLength / 60));
    gNumbers.queue(eNumber::PLAY_SEC, int(chartLength % 60));
    gNumbers.queue(eNumber::PLAY_REMAIN_MIN, int(chartLength / 60));
    gNumbers.queue(eNumber::PLAY_REMAIN_SEC, int(chartLength % 60));

    // compare to db record
    auto dp = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();
    Option::e_rank_type nowRank = Option::getRankType(dp.total_acc);
    auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
    if (pScore)
    {
        gNumbers.queue(eNumber::RESULT_RECORD_EX_NOW, (int)dp.score2);
        gNumbers.queue(eNumber::RESULT_RECORD_EX_DIFF, (int)dp.score2 - pScore->exscore);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_NOW, (int)dp.maxCombo);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_DIFF, (int)dp.maxCombo - pScore->exscore);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_NOW, (int)dp.miss);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_DIFF, (int)dp.miss - pScore->bp);

        gNumbers.queue(eNumber::RESULT_MYBEST_EX, pScore->exscore);
        gNumbers.queue(eNumber::RESULT_MYBEST_DIFF, dp.score2 - pScore->exscore);
        gNumbers.queue(eNumber::RESULT_MYBEST_RATE, (int)std::floor(pScore->rate));
        gNumbers.queue(eNumber::RESULT_MYBEST_RATE_DECIMAL2, (int)std::floor(pScore->rate * 100.0) % 100);

        gNumbers.queue(eNumber::RESULT_RECORD_EX_BEFORE, pScore->exscore);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_BEFORE, pScore->maxcombo);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_BEFORE, pScore->bp);
        gNumbers.queue(eNumber::RESULT_RECORD_MYBEST_RATE, (int)std::floor(pScore->rate));
        gNumbers.queue(eNumber::RESULT_RECORD_MYBEST_RATE_DECIMAL2, (int)std::floor(pScore->rate * 100.0) % 100);

        Option::e_rank_type recordRank = Option::getRankType(pScore->rate);
        gOptions.queue(eOption::RESULT_MYBEST_RANK, recordRank);
        //gOptions.queue(eOption::RESULT_UPDATED_RANK, (pScore->exscore > (int)dp.score2) ? recordRank : nowRank);
        gOptions.queue(eOption::RESULT_UPDATED_RANK, nowRank);

        if (pScore->exscore < dp.score2)    gSwitches.queue(eSwitch::RESULT_UPDATED_SCORE, true);
        if (pScore->maxcombo < dp.maxCombo) gSwitches.queue(eSwitch::RESULT_UPDATED_MAXCOMBO, true);
        if (pScore->bp > dp.miss)           gSwitches.queue(eSwitch::RESULT_UPDATED_BP, true);
    }
    else
    {
        gNumbers.queue(eNumber::RESULT_RECORD_EX_NOW, (int)dp.score2);
        gNumbers.queue(eNumber::RESULT_RECORD_EX_DIFF, (int)dp.score2);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_NOW, (int)dp.maxCombo);
        gNumbers.queue(eNumber::RESULT_RECORD_MAXCOMBO_DIFF, (int)dp.maxCombo);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_NOW, (int)dp.miss);
        gNumbers.queue(eNumber::RESULT_RECORD_BP_DIFF, (int)dp.miss);

        gOptions.queue(eOption::RESULT_MYBEST_RANK, Option::RANK_NONE);
        gOptions.queue(eOption::RESULT_UPDATED_RANK, nowRank);

        gSwitches.queue(eSwitch::RESULT_UPDATED_SCORE, true);
        gSwitches.queue(eSwitch::RESULT_UPDATED_MAXCOMBO, true);
        gSwitches.queue(eSwitch::RESULT_UPDATED_BP, true);
    }

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

    _looper->loopStart();
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
    if (gNextScene != eScene::RESULT) return;

    if (gAppIsExiting)
    {
        _input.loopEnd();
        _skin->stopSpriteVideoUpdate();
        gNextScene = eScene::EXIT_TRANS;
    }

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
        _input.loopEnd();
        SoundMgr::stopKeySamples();

        // save score
        if (_playmode != ePlayMode::LOCAL_BATTLE && !gChartContext.hash.empty() && gSelectContext.pitchSpeed >= 1.0)
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
            _skin->stopSpriteVideoUpdate();
            gNextScene = eScene::PLAY;
        }
        else
        {
            clearContextPlay();
            _skin->stopSpriteVideoUpdate();
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
