#include "scene_course_result.h"
#include "scene_context.h"
#include "common/types.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"
#include <boost/algorithm/string.hpp>

SceneCourseResult::SceneCourseResult() : vScene(eMode::COURSE_RESULT, 1000)
{
    _scene = eScene::COURSE_RESULT;

    _inputAvailable = INPUT_MASK_FUNC;

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    _state = eCourseResultState::DRAW;

    std::map<std::string, int> param;

    if (!gPlayContext.courseStageRulesetCopy[0].empty())
    {
        d1p = { 0 };
        int max = 0;
        for (const auto& r : gPlayContext.courseStageRulesetCopy[0])
        {
            auto& d = r->getData();
            totalNotes += r->getMaxCombo();
            max += r->getMaxScore();
            d1p.score += d.score;
            d1p.score2 += d.score2;
            d1p.maxCombo += d.maxCombo;
            d1p.hit += d.hit;
            d1p.miss += d.miss;
            d1p.fast += d.fast;
            d1p.slow += d.slow;

            switch (gChartContext.chartObj->type())
            {
            case eChartFormat::BMS:
            case eChartFormat::BMSON:
            {
                auto rBMS = std::reinterpret_pointer_cast<RulesetBMS>(r);
                for (unsigned j = 0; j <= (unsigned)RulesetBMS::JudgeType::COMBOBREAK; ++j)
                    judgeCount[j] += rBMS->getJudgeCount((RulesetBMS::JudgeType)j);
                break;
            }
            }
        }
        if (max > 0)
        {
            d1p.acc = (double)d1p.score2 / max;
            d1p.total_acc = d1p.acc;
        }

        // set options
        param["1prank"] = Option::getRankType(d1p.total_acc);
        param["1ptarget"] = d1p.score2;
        param["1pexscore"] = d1p.score2;
        param["1pmaxcombo"] = d1p.maxCombo;
        param["1pbp"] = d1p.miss;
        param["1ppg"] = judgeCount[0];
        param["1pgr"] = judgeCount[1];
        param["1pgd"] = judgeCount[2];
        param["1pbd"] = judgeCount[3];
        param["1ppr"] = judgeCount[4] + judgeCount[5];
        param["1pcb"] = judgeCount[3] + judgeCount[5];
        param["1pfast"] = d1p.fast;
        param["1pslow"] = d1p.slow;

        if (totalNotes > 0)
        {
            param["1ppgrate"] = int(100 * param["1ppg"] / totalNotes);
            param["1pgrrate"] = int(100 * param["1pgr"] / totalNotes);
            param["1pgdrate"] = int(100 * param["1pgd"] / totalNotes);
            param["1pbdrate"] = int(100 * param["1pbd"] / totalNotes);
            param["1pprrate"] = int(100 * param["1ppr"] / totalNotes);
        }


        if (gPlayContext.isBattle && !gPlayContext.courseStageRulesetCopy[1].empty())
        {
            vRuleset::BasicData d2p = { 0 };
            std::map<unsigned, unsigned> judgeCount2P;
            int max = 0;
            for (const auto& r : gPlayContext.courseStageRulesetCopy[0])
            {
                auto& d = r->getData();
                max += r->getMaxScore();
                d2p.score += d.score;
                d2p.score2 += d.score2;
                d2p.maxCombo += d.maxCombo;
                d2p.hit += d.hit;
                d2p.miss += d.miss;
                d2p.fast += d.fast;
                d2p.slow += d.slow;

                switch (gChartContext.chartObj->type())
                {
                case eChartFormat::BMS:
                case eChartFormat::BMSON:
                {
                    auto rBMS = std::reinterpret_pointer_cast<RulesetBMS>(r);
                    for (unsigned j = 0; j <= (unsigned)RulesetBMS::JudgeType::COMBOBREAK; ++j)
                        judgeCount2P[j] += rBMS->getJudgeCount((RulesetBMS::JudgeType)j);
                    break;
                }
                }
            }
            if (max > 0)
            {
                d2p.acc = (double)d1p.score2 / max;
                d2p.total_acc = d1p.acc;
            }

            param["2prank"] = Option::getRankType(d1p.total_acc);
            param["2ptarget"] = d2p.score2 - d1p.score2;
            param["2pexscore"] = d2p.score2;
            param["2pmaxcombo"] = d2p.maxCombo;
            param["2pbp"] = d2p.miss;
            param["2ppg"] = judgeCount2P[0];
            param["2pgr"] = judgeCount2P[1];
            param["2pgd"] = judgeCount2P[2];
            param["2pbd"] = judgeCount2P[3];
            param["2pbpoor"] = judgeCount2P[4];
            param["2ppr"] = judgeCount2P[4] + judgeCount2P[5];
            param["2pcb"] = judgeCount2P[3] + judgeCount2P[5];
            param["2pfast"] = d2p.fast;
            param["2pslow"] = d2p.slow;

            if (totalNotes > 0)
            {
                param["2ppgrate"] = int(100 * param["2ppg"] / totalNotes);
                param["2pgrrate"] = int(100 * param["2pgr"] / totalNotes);
                param["2pgdrate"] = int(100 * param["2pgd"] / totalNotes);
                param["2pbdrate"] = int(100 * param["2pbd"] / totalNotes);
                param["2pprrate"] = int(100 * param["2ppr"] / totalNotes);
            }

            param["1ptarget"] = d1p.score2 - d2p.score2;

            if (d1p.score2 > d2p.score2)
                param["winlose"] = 1;
            else if (d1p.score2 < d2p.score2)
                param["winlose"] = 2;
        }

        // compare to db record
        auto pScore = g_pScoreDB->getCourseScoreBMS(gPlayContext.courseHash);
        if (pScore)
        {
            param["dbexscore"] = pScore->exscore;
            param["dbexscorediff"] = (int)d1p.score2 - pScore->exscore;
            param["newexscore"] = (int)d1p.score2;
            param["newexscorediff"] = param["newexscore"] - pScore->exscore;
            param["dbmaxcombo"] = (int)pScore->maxcombo;
            param["newmaxcombo"] = (int)d1p.maxCombo;
            param["newmaxcombodiff"] = param["newmaxcombo"] - pScore->maxcombo;
            param["dbbp"] = pScore->bp;
            param["newbp"] = (int)d1p.miss;
            param["newbpdiff"] = param["newbp"] - pScore->bp;
            param["dbrate"] = (int)(pScore->rate);
            param["dbrated2"] = (int)(pScore->rate * 100.0) % 100;
            param["dbrank"] = Option::getRankType(pScore->rate);

            param["updatedscore"] = pScore->exscore < d1p.score2;
            param["updatedmaxcombo"] = pScore->maxcombo < d1p.maxCombo;
            param["updatedbp"] = pScore->bp < d1p.miss;
        }
        else
        {
            param["dbexscorediff"] = (int)d1p.score2;
            param["newexscore"] = (int)d1p.score2;
            param["newexscorediff"] = param["newexscore"];
            param["newmaxcombo"] = (int)d1p.maxCombo;
            param["newmaxcombodiff"] = param["newmaxcombo"];
            param["newbp"] = (int)d1p.miss;
            param["newbpdiff"] = param["newbp"];
            param["updatedscore"] = true;
            param["updatedmaxcombo"] = true;
            param["updatedbp"] = true;
        }
    }

    // save
    {
        State::set(IndexOption::RESULT_RANK_1P, param["1prank"]);
        State::set(IndexOption::RESULT_RANK_2P, param["2prank"]);
        State::set(IndexNumber::PLAY_1P_EXSCORE_DIFF, param["1ptarget"]);
        State::set(IndexNumber::PLAY_2P_EXSCORE_DIFF, param["2ptarget"]);
        State::set(IndexOption::RESULT_BATTLE_WIN_LOSE, param["winlose"]);
        
        State::set(IndexNumber::PLAY_1P_EXSCORE, param["1pexscore"]);
        State::set(IndexNumber::PLAY_1P_PERFECT, param["1ppg"]);
        State::set(IndexNumber::PLAY_1P_GREAT, param["1pgr"]);
        State::set(IndexNumber::PLAY_1P_GOOD, param["1pgd"]);
        State::set(IndexNumber::PLAY_1P_BAD, param["1pbd"]);
        State::set(IndexNumber::PLAY_1P_POOR, param["1ppr"]);
        State::set(IndexNumber::PLAY_1P_BPOOR, param["1pbpoor"]);
        State::set(IndexNumber::PLAY_1P_COMBOBREAK, param["1pcb"]);
        State::set(IndexNumber::PLAY_1P_BP, param["1pbp"]);
        State::set(IndexNumber::PLAY_1P_FAST_COUNT, param["1pfast"]);
        State::set(IndexNumber::PLAY_1P_SLOW_COUNT, param["1pslow"]);

        State::set(IndexNumber::PLAY_2P_EXSCORE, param["2pexscore"]);
        State::set(IndexNumber::PLAY_2P_PERFECT, param["2ppg"]);
        State::set(IndexNumber::PLAY_2P_GREAT, param["2pgr"]);
        State::set(IndexNumber::PLAY_2P_GOOD, param["2pgd"]);
        State::set(IndexNumber::PLAY_2P_BAD, param["2pbd"]);
        State::set(IndexNumber::PLAY_2P_POOR, param["2ppr"]);
        State::set(IndexNumber::PLAY_2P_BPOOR, param["2pbpoor"]);
        State::set(IndexNumber::PLAY_2P_COMBOBREAK, param["2pcb"]);
        State::set(IndexNumber::PLAY_2P_BP, param["2pbp"]);
        State::set(IndexNumber::PLAY_2P_FAST_COUNT, param["2pfast"]);
        State::set(IndexNumber::PLAY_2P_SLOW_COUNT, param["2pslow"]);

        State::set(IndexNumber::PLAY_MIN, param["min"]);
        State::set(IndexNumber::PLAY_SEC, param["sec"]);
        State::set(IndexNumber::PLAY_REMAIN_MIN, param["min"]);
        State::set(IndexNumber::PLAY_REMAIN_SEC, param["sec"]);

        State::set(IndexNumber::RESULT_RECORD_EX_BEFORE, param["dbexscore"]);
        State::set(IndexNumber::RESULT_RECORD_EX_NOW, param["newexscore"]);
        State::set(IndexNumber::RESULT_RECORD_EX_DIFF, param["newexscorediff"]);
        State::set(IndexNumber::RESULT_RECORD_MAXCOMBO_BEFORE, param["dbmaxcombo"]);
        State::set(IndexNumber::RESULT_RECORD_MAXCOMBO_NOW, param["newmaxcombo"]);
        State::set(IndexNumber::RESULT_RECORD_MAXCOMBO_DIFF, param["newmaxombodiff"]);
        State::set(IndexNumber::RESULT_RECORD_BP_BEFORE, param["dbbp"]);
        State::set(IndexNumber::RESULT_RECORD_BP_NOW, param["1pbp"]);
        State::set(IndexNumber::RESULT_RECORD_BP_DIFF, param["newbpdiff"]);
        State::set(IndexNumber::RESULT_RECORD_MYBEST_RATE, param["dbrate"]);
        State::set(IndexNumber::RESULT_RECORD_MYBEST_RATE_DECIMAL2, param["dbrated2"]);

        State::set(IndexNumber::RESULT_MYBEST_EX, param["dbexscore"]);
        State::set(IndexNumber::RESULT_MYBEST_DIFF, param["dbexscorediff"]);
        State::set(IndexNumber::RESULT_MYBEST_RATE, param["dbrate"]);
        State::set(IndexNumber::RESULT_MYBEST_RATE_DECIMAL2, param["dbrated2"]);

        State::set(IndexOption::RESULT_MYBEST_RANK, param["dbrank"]);
        State::set(IndexOption::RESULT_UPDATED_RANK, param["1prank"]);

        State::set(IndexSwitch::RESULT_UPDATED_SCORE, param["updatedscore"]);
        State::set(IndexSwitch::RESULT_UPDATED_MAXCOMBO, param["updatedmaxcombo"]);
        State::set(IndexSwitch::RESULT_UPDATED_BP, param["updatedbp"]);
    }


    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneCourseResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneCourseResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneCourseResult::inputGameRelease, this, _1, _2));

    Time t;
    State::set(IndexTimer::RESULT_GRAPH_START, t.norm());

    if (!gInCustomize)
    {
        SoundMgr::stopSysSamples();

        if (State::get(IndexSwitch::RESULT_CLEAR))
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_COURSE_CLEAR);
        else
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_COURSE_FAIL);
    }
}

////////////////////////////////////////////////////////////////////////////////

SceneCourseResult::~SceneCourseResult()
{
    _input.loopEnd();
    loopEnd();
}

void SceneCourseResult::_updateAsync()
{
    if (gNextScene != eScene::COURSE_RESULT) return;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
    }

    switch (_state)
    {
    case eCourseResultState::DRAW:
        updateDraw();
        break;
    case eCourseResultState::STOP:
        updateStop();
        break;
    case eCourseResultState::RECORD:
        updateRecord();
        break;
    case eCourseResultState::FADEOUT:
        updateFadeout();
        break;
    }
}

void SceneCourseResult::updateDraw()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeResultRank)
    {
        State::set(IndexTimer::RESULT_RANK_START, t.norm());
        // TODO play hit sound
        _state = eCourseResultState::STOP;
        LOG_DEBUG << "[Result] State changed to STOP";
    }
}

void SceneCourseResult::updateStop()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);
}

void SceneCourseResult::updateRecord()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);

    // TODO sync score in online mode?
    if (true)
    {
        _scoreSyncFinished = true;
    }
}

void SceneCourseResult::updateFadeout()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);
    auto ft = t - State::get(IndexTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        // save score
        if (!gInCustomize && !gPlayContext.isBattle && !gChartContext.hash.empty())
        {
            assert(gPlayContext.ruleset[PLAYER_SLOT_PLAYER] != nullptr);
            ScoreBMS score;
            score.notes = totalNotes;
            score.score = d1p.score;
            score.rate = d1p.total_acc;
            score.fast = d1p.fast;
            score.slow = d1p.slow;
            score.maxcombo = d1p.maxCombo;
            score.playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
            auto isclear = State::get(IndexSwitch::RESULT_CLEAR) ? 1 : 0;
            score.clearcount = _pScoreOld ? _pScoreOld->clearcount + isclear : isclear;

            std::stringstream dbReplayFile;
            for (size_t i = 0; i < gPlayContext.courseStageReplayPath.size(); ++i)
            {
                if (i != 0) dbReplayFile << "|";
                dbReplayFile << gPlayContext.courseStageReplayPath[i].u8string();
            }
            score.replayFileName = dbReplayFile.str();

            switch (gChartContext.chartObj->type())
            {
            case eChartFormat::BMS:
            case eChartFormat::BMSON:
            {
                auto rBMS = std::reinterpret_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]);
                score.exscore = d1p.score2;

                score.lamp = ScoreBMS::Lamp::NOPLAY;
                if (isclear)
                {
                    if (d1p.maxCombo == totalNotes)
                    {
                        score.lamp = ScoreBMS::Lamp::FULLCOMBO;
                    }
                    else if (rBMS->getGaugeType() == RulesetBMS::GaugeType::EXGRADE)
                    {
                        score.lamp = ScoreBMS::Lamp::HARD;
                    }
                    else if (rBMS->getGaugeType() == RulesetBMS::GaugeType::GRADE)
                    {
                        score.lamp = ScoreBMS::Lamp::NORMAL;
                    }
                }
                else
                {
                    score.lamp = ScoreBMS::Lamp::FAILED;
                }

                if (gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask == 0)
                {
                    score.pgreat = judgeCount[0];
                    score.great = judgeCount[1];
                    score.good = judgeCount[2];
                    score.bad = judgeCount[3];
                    score.bpoor = judgeCount[4];
                    score.miss = judgeCount[5];
                    score.bp = d1p.miss;
                    score.combobreak = score.bad + score.miss;
                }
                g_pScoreDB->updateCourseScoreBMS(gPlayContext.courseHash, score);
                break;
            }
            default:
                break;
            }
        }
        
        clearContextPlay();
        gPlayContext.courseStageRulesetCopy[0].clear();
        gPlayContext.courseStageRulesetCopy[1].clear();
        gPlayContext.courseStageReplayPath.clear();
        gPlayContext.isAuto = false;
        gPlayContext.isReplay = false;
        gNextScene = gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneCourseResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - State::get(IndexTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m & (INPUT_MASK_DECIDE | INPUT_MASK_CANCEL)).any() || m[Input::ESC])
    {
        switch (_state)
        {
        case eCourseResultState::DRAW:
            State::set(IndexTimer::RESULT_RANK_START, t.norm());
            // TODO play hit sound
            _state = eCourseResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eCourseResultState::STOP:
            if (!gPlayContext.isBattle)
            {
                State::set(IndexTimer::RESULT_HIGHSCORE_START, t.norm());
                // TODO stop result sound
                // TODO play record sound
                _state = eCourseResultState::RECORD;
                LOG_DEBUG << "[Result] State changed to RECORD";
            }
            else
            {
                State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
                _state = eCourseResultState::FADEOUT;
                SoundMgr::setSysVolume(0.0, 2000);
                SoundMgr::setNoteVolume(0.0, 2000);
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eCourseResultState::RECORD:
            if (_scoreSyncFinished)
            {
                State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
                _state = eCourseResultState::FADEOUT;
                SoundMgr::setSysVolume(0.0, 2000);
                SoundMgr::setNoteVolume(0.0, 2000);
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eCourseResultState::FADEOUT:
            break;

        default:
            break;
        }
    }
}

// CALLBACK
void SceneCourseResult::inputGameHold(InputMask& m, const Time& t)
{
    if (t - State::get(IndexTimer::SCENE_START) < _skin->info.timeIntro) return;

    if (_state == eCourseResultState::FADEOUT)
    {
        _retryRequested =
            (_inputAvailable & m & INPUT_MASK_DECIDE).any() && 
            (_inputAvailable & m & INPUT_MASK_CANCEL).any();
    }
}

// CALLBACK
void SceneCourseResult::inputGameRelease(InputMask& m, const Time& t)
{
    if (t - State::get(IndexTimer::SCENE_START) < _skin->info.timeIntro) return;
}
