#include "scene_course_result.h"
#include "scene_context.h"
#include "common/types.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"
#include <boost/algorithm/string.hpp>

SceneCourseResult::SceneCourseResult() : SceneBase(SkinType::COURSE_RESULT, 1000)
{
    _type = SceneType::COURSE_RESULT;

    _inputAvailable = INPUT_MASK_FUNC;

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    state = eCourseResultState::DRAW;

    std::map<std::string, int> param;

    if (!gPlayContext.courseStageRulesetCopy[0].empty())
    {
        for (const auto& r : gPlayContext.courseStageRulesetCopy[0])
        {
            const auto d = r->getData();
            summary[ARG_TOTAL_NOTES] += r->getNoteCount();
            summary[ARG_MAX_SCORE] += r->getMaxScore();
            summary[ARG_MAXCOMBO] = std::max(summary[ARG_MAXCOMBO], d.maxComboDisplay);

            if (auto pr = std::dynamic_pointer_cast<RulesetBMS>(r); pr)
            {
                summary[ARG_SCORE] += unsigned(std::floor(pr->getScore()));
                summary[ARG_EXSCORE] += pr->getExScore();
                summary[ARG_FAST] += pr->getJudgeCountEx(RulesetBMS::JUDGE_EARLY);
                summary[ARG_SLOW] += pr->getJudgeCountEx(RulesetBMS::JUDGE_LATE);
                summary[ARG_BP] += pr->getJudgeCountEx(RulesetBMS::JUDGE_BP);
                summary[ARG_CB] += pr->getJudgeCountEx(RulesetBMS::JUDGE_CB);
                summary[ARG_JUDGE_0] += pr->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
                summary[ARG_JUDGE_1] += pr->getJudgeCount(RulesetBMS::JudgeType::GREAT);
                summary[ARG_JUDGE_2] += pr->getJudgeCount(RulesetBMS::JudgeType::GOOD);
                summary[ARG_JUDGE_3] += pr->getJudgeCount(RulesetBMS::JudgeType::BAD);
                summary[ARG_JUDGE_4] += pr->getJudgeCount(RulesetBMS::JudgeType::KPOOR);
                summary[ARG_JUDGE_5] += pr->getJudgeCount(RulesetBMS::JudgeType::MISS);
            }
        }
        if (summary[ARG_TOTAL_NOTES] > 0)
        {
            for (const auto& r : gPlayContext.courseStageRulesetCopy[0])
            {
                acc += r->getData().total_acc * r->getNoteCount() / summary[ARG_TOTAL_NOTES];
            }
        }

        // set options
        param["1prank"] = Option::getRankType(acc);
        param["1ptarget"] = summary[ARG_EXSCORE];
        param["1pexscore"] = summary[ARG_EXSCORE];
        param["1pmaxcombo"] = summary[ARG_MAXCOMBO];
        param["1pbp"] = summary[ARG_BP];
        param["1ppg"] = summary[ARG_JUDGE_0];
        param["1pgr"] = summary[ARG_JUDGE_1];
        param["1pgd"] = summary[ARG_JUDGE_2];
        param["1pbd"] = summary[ARG_JUDGE_3];
        param["1ppr"] = summary[ARG_JUDGE_4] + summary[ARG_JUDGE_5];
        param["1pcb"] = summary[ARG_CB];
        param["1pfast"] = summary[ARG_FAST];
        param["1pslow"] = summary[ARG_SLOW];
        param["1prate"] = (int)acc;
        param["1prated2"] = (int)(acc * 100.0) % 100;

        if (summary[ARG_TOTAL_NOTES] > 0)
        {
            param["1ppgrate"] = int(100 * param["1ppg"] / summary[ARG_TOTAL_NOTES]);
            param["1pgrrate"] = int(100 * param["1pgr"] / summary[ARG_TOTAL_NOTES]);
            param["1pgdrate"] = int(100 * param["1pgd"] / summary[ARG_TOTAL_NOTES]);
            param["1pbdrate"] = int(100 * param["1pbd"] / summary[ARG_TOTAL_NOTES]);
            param["1pprrate"] = int(100 * param["1ppr"] / summary[ARG_TOTAL_NOTES]);
        }


        if (gPlayContext.isBattle && !gPlayContext.courseStageRulesetCopy[1].empty())
        {
            decltype(summary) summary2P;
            double acc2P = 0.;
            for (const auto& r : gPlayContext.courseStageRulesetCopy[1])
            {
                const auto d = r->getData();
                summary2P[ARG_TOTAL_NOTES] += r->getNoteCount();
                summary2P[ARG_MAX_SCORE] += r->getMaxScore();
                summary2P[ARG_MAXCOMBO] = std::max(summary2P[ARG_MAXCOMBO], d.maxComboDisplay);
                acc2P += r->getNoteCount() > 0 ? (d.total_acc / r->getNoteCount()) : 0.0;

                if (auto pr = std::dynamic_pointer_cast<RulesetBMS>(r); pr)
                {
                    summary2P[ARG_SCORE] += unsigned(std::floor(pr->getScore()));
                    summary2P[ARG_EXSCORE] += pr->getExScore();
                    summary2P[ARG_FAST] += pr->getJudgeCountEx(RulesetBMS::JUDGE_EARLY);
                    summary2P[ARG_SLOW] += pr->getJudgeCountEx(RulesetBMS::JUDGE_LATE);
                    summary2P[ARG_BP] += pr->getJudgeCountEx(RulesetBMS::JUDGE_BP);
                    summary2P[ARG_CB] += pr->getJudgeCountEx(RulesetBMS::JUDGE_CB);
                    summary2P[ARG_JUDGE_0] += pr->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
                    summary2P[ARG_JUDGE_1] += pr->getJudgeCount(RulesetBMS::JudgeType::GREAT);
                    summary2P[ARG_JUDGE_2] += pr->getJudgeCount(RulesetBMS::JudgeType::GOOD);
                    summary2P[ARG_JUDGE_3] += pr->getJudgeCount(RulesetBMS::JudgeType::BAD);
                    summary2P[ARG_JUDGE_4] += pr->getJudgeCount(RulesetBMS::JudgeType::KPOOR);
                    summary2P[ARG_JUDGE_5] += pr->getJudgeCount(RulesetBMS::JudgeType::MISS);
                }
            }
            if (summary[ARG_TOTAL_NOTES] > 0)
            {
                for (const auto& r : gPlayContext.courseStageRulesetCopy[1])
                {
                    acc2P += r->getData().total_acc * r->getNoteCount() / summary2P[ARG_TOTAL_NOTES];
                }
            }

            param["2prank"] = Option::getRankType(acc2P);
            param["2ptarget"] = summary2P[ARG_EXSCORE];
            param["2pexscore"] = summary2P[ARG_EXSCORE];
            param["2pmaxcombo"] = summary2P[ARG_MAXCOMBO];
            param["2pbp"] = summary2P[ARG_BP];
            param["2ppg"] = summary2P[ARG_JUDGE_0];
            param["2pgr"] = summary2P[ARG_JUDGE_1];
            param["2pgd"] = summary2P[ARG_JUDGE_2];
            param["2pbd"] = summary2P[ARG_JUDGE_3];
            param["2ppr"] = summary2P[ARG_JUDGE_4] + summary2P[ARG_JUDGE_5];
            param["2pcb"] = summary2P[ARG_CB];
            param["2pfast"] = summary2P[ARG_FAST];
            param["2pslow"] = summary2P[ARG_SLOW];
            param["2prate"] = (int)acc2P;
            param["2prated2"] = (int)(acc2P * 100.0) % 100;

            if (summary2P[ARG_TOTAL_NOTES] > 0)
            {
                param["2ppgrate"] = int(100 * param["2ppg"] / summary2P[ARG_TOTAL_NOTES]);
                param["2pgrrate"] = int(100 * param["2pgr"] / summary2P[ARG_TOTAL_NOTES]);
                param["2pgdrate"] = int(100 * param["2pgd"] / summary2P[ARG_TOTAL_NOTES]);
                param["2pbdrate"] = int(100 * param["2pbd"] / summary2P[ARG_TOTAL_NOTES]);
                param["2pprrate"] = int(100 * param["2ppr"] / summary2P[ARG_TOTAL_NOTES]);
            }

            param["1ptarget"] = summary[ARG_EXSCORE] - summary2P[ARG_EXSCORE];
            param["winlose"] = (param["1ptarget"] > 0) ? 1 : (param["1ptarget"] < 0) ? 2 : 0;
        }

        // compare to db record
        auto pScore = g_pScoreDB->getCourseScoreBMS(gPlayContext.courseHash);
        if (pScore)
        {
            param["dbexscore"] = pScore->exscore;
            param["dbexscorediff"] = param["1pexscore"] - pScore->exscore;
            param["newexscore"] = param["1pexscore"];
            param["newexscorediff"] = param["newexscore"] - pScore->exscore;
            param["dbmaxcombo"] = (int)pScore->maxcombo;
            param["newmaxcombo"] = param["1pmaxcombo"];
            param["newmaxcombodiff"] = param["newmaxcombo"] - pScore->maxcombo;
            param["dbbp"] = pScore->bp;
            param["newbp"] = param["1pbp"];
            param["newbpdiff"] = param["newbp"] - pScore->bp;
            param["dbrate"] = (int)(pScore->rate);
            param["dbrated2"] = (int)(pScore->rate * 100.0) % 100;
            param["dbrank"] = Option::getRankType(pScore->rate);

            param["updatedscore"] = pScore->exscore < param["1pexscore"];
            param["updatedmaxcombo"] = pScore->maxcombo < param["1pmaxcombo"];
            param["updatedbp"] = pScore->bp < param["1pbp"];
        }
        else
        {
            param["dbexscorediff"] = param["1pexscore"];
            param["newexscore"] = param["1pmaxcombo"];
            param["newexscorediff"] = param["newexscore"];
            param["newmaxcombo"] = param["1pmaxcombo"];
            param["newmaxcombodiff"] = param["newmaxcombo"];
            param["newbp"] = param["1pbp"];
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
        State::set(IndexNumber::PLAY_1P_RATE, param["1prate"]);
        State::set(IndexNumber::PLAY_1P_RATEDECIMAL, param["1prated2"]);

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
        State::set(IndexNumber::PLAY_2P_RATE, param["2prate"]);
        State::set(IndexNumber::PLAY_2P_RATEDECIMAL, param["2prated2"]);

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
    if (gNextScene != SceneType::COURSE_RESULT) return;

    if (gAppIsExiting)
    {
        gNextScene = SceneType::EXIT_TRANS;
    }

    switch (state)
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

    if (rt.norm() >= pSkin->info.timeResultRank)
    {
        State::set(IndexTimer::RESULT_RANK_START, t.norm());
        // TODO play hit sound
        state = eCourseResultState::STOP;
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

    if (ft >= pSkin->info.timeOutro)
    {
        // save score
        if (!gInCustomize && !gPlayContext.isReplay && !gPlayContext.isBattle && !gChartContext.hash.empty())
        {
            assert(gPlayContext.ruleset[PLAYER_SLOT_PLAYER] != nullptr);
            ScoreBMS score;
            score.notes = summary[ARG_TOTAL_NOTES];
            score.rate = acc;
            score.maxcombo = summary[ARG_MAXCOMBO];
            score.playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
            auto isclear = State::get(IndexSwitch::RESULT_CLEAR) ? 1 : 0;
            score.clearcount = _pScoreOld ? _pScoreOld->clearcount + isclear : isclear;

            std::stringstream dbReplayFile;
            for (size_t i = 0; i < gPlayContext.courseStageReplayPathNew.size(); ++i)
            {
                if (i != 0) dbReplayFile << "|";
                dbReplayFile << gPlayContext.courseStageReplayPathNew[i].u8string();
            }
            score.replayFileName = dbReplayFile.str();

            score.score = summary[ARG_SCORE];
            score.exscore = summary[ARG_EXSCORE];
            score.fast = summary[ARG_FAST];
            score.slow = summary[ARG_SLOW];

            if (gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask == 0)
            {
                score.pgreat = summary[ARG_JUDGE_0];
                score.great = summary[ARG_JUDGE_1];
                score.good = summary[ARG_JUDGE_2];
                score.bad = summary[ARG_JUDGE_3];
                score.kpoor = summary[ARG_JUDGE_4];
                score.miss = summary[ARG_JUDGE_5];
                score.bp = summary[ARG_BP];
                score.combobreak = summary[ARG_CB];
            }

            score.lamp = ScoreBMS::Lamp::NOPLAY;
            if (isclear)
            {
                if (summary[ARG_MAXCOMBO] == summary[ARG_TOTAL_NOTES])
                {
                    score.lamp = ScoreBMS::Lamp::FULLCOMBO;
                }
                else if (gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge == PlayModifierGaugeType::GRADE_HARD)
                {
                    score.lamp = ScoreBMS::Lamp::HARD;
                }
                else if (gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge == PlayModifierGaugeType::GRADE_NORMAL)
                {
                    score.lamp = ScoreBMS::Lamp::NORMAL;
                }
            }
            else
            {
                score.lamp = ScoreBMS::Lamp::FAILED;
            }

            // 
            g_pScoreDB->updateCourseScoreBMS(gPlayContext.courseHash, score);
        }
        
        clearContextPlay();
        gPlayContext.courseStageRulesetCopy[0].clear();
        gPlayContext.courseStageRulesetCopy[1].clear();
        gPlayContext.courseStageReplayPathNew.clear();
        gPlayContext.isAuto = false;
        gPlayContext.isReplay = false;
        gNextScene = gQuitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneCourseResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - State::get(IndexTimer::SCENE_START) < pSkin->info.timeIntro) return;

    if ((_inputAvailable & m & (INPUT_MASK_DECIDE | INPUT_MASK_CANCEL)).any() || m[Input::ESC])
    {
        switch (state)
        {
        case eCourseResultState::DRAW:
            State::set(IndexTimer::RESULT_RANK_START, t.norm());
            // TODO play hit sound
            state = eCourseResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eCourseResultState::STOP:
            if (!gPlayContext.isBattle && !gPlayContext.isReplay)
            {
                State::set(IndexTimer::RESULT_HIGHSCORE_START, t.norm());
                // TODO stop result sound
                // TODO play record sound
                state = eCourseResultState::RECORD;
                LOG_DEBUG << "[Result] State changed to RECORD";
            }
            else
            {
                State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
                state = eCourseResultState::FADEOUT;
                SoundMgr::setSysVolume(0.0, 2000);
                SoundMgr::setNoteVolume(0.0, 2000);
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eCourseResultState::RECORD:
            if (_scoreSyncFinished)
            {
                State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
                state = eCourseResultState::FADEOUT;
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
    if (t - State::get(IndexTimer::SCENE_START) < pSkin->info.timeIntro) return;

    if (state == eCourseResultState::FADEOUT)
    {
        _retryRequested =
            (_inputAvailable & m & INPUT_MASK_DECIDE).any() && 
            (_inputAvailable & m & INPUT_MASK_CANCEL).any();
    }
}

// CALLBACK
void SceneCourseResult::inputGameRelease(InputMask& m, const Time& t)
{
    if (t - State::get(IndexTimer::SCENE_START) < pSkin->info.timeIntro) return;
}
