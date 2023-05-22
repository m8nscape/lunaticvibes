#include "common/pch.h"
#include "scene_course_result.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/data/data_types.h"
#include "db/db_score.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "config/config_mgr.h"

namespace lunaticvibes
{

SceneCourseResult::SceneCourseResult() : SceneBase(SkinType::COURSE_RESULT, 1000)
{
    _type = SceneType::COURSE_RESULT;

    _inputAvailable = INPUT_MASK_FUNC;

    if (PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }

    if (PlayData.player[PLAYER_SLOT_TARGET].chartObj != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    state = eCourseResultState::DRAW;

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneCourseResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneCourseResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneCourseResult::inputGameRelease, this, _1, _2));

    Time t;
    ResultData.timers["graph_start"] = t.norm();

    // if (!LR2CustomizeData.isInCustomize)
    {
        SoundMgr::stopSysSamples();

        if (ResultData.cleared)
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
    if (SystemData.gNextScene != SceneType::COURSE_RESULT) return;

    if (SystemData.isAppExiting)
    {
        SystemData.gNextScene = SceneType::EXIT_TRANS;
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
    auto rt = t - SystemData.timers["scene_start"];

    if (rt.norm() >= pSkin->info.timeResultRank)
    {
        ResultData.timers["graph_end"] = t.norm();
        // TODO play hit sound
        state = eCourseResultState::STOP;
        LOG_DEBUG << "[Result] State changed to STOP";
    }
}

void SceneCourseResult::updateStop()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];
}

void SceneCourseResult::updateRecord()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];

    // TODO sync score in online mode?
    if (true)
    {
        _scoreSyncFinished = true;
    }
}

void SceneCourseResult::updateFadeout()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];
    auto ft = t - SystemData.timers["fadeout"];

    if (ft >= pSkin->info.timeOutro)
    {
        // save score
        if (!PlayData.isReplay && 
            PlayData.battleType != PlayModifierBattleType::LocalBattle &&
            PlayData.battleType != PlayModifierBattleType::DoubleBattle &&
            PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr &&
            !PlayData.player[PLAYER_SLOT_PLAYER].chartObj->getFileHash().empty() &&
            PlayData.player[PLAYER_SLOT_PLAYER].ruleset != nullptr)
        {
            ScoreBMS score;
            score.notes = summary[ARG_TOTAL_NOTES];
            score.rate = acc;
            score.maxcombo = summary[ARG_MAXCOMBO];
            score.playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
            auto isclear = ResultData.cleared;
            score.clearcount = _pScoreOld ? _pScoreOld->clearcount + isclear : isclear;

            std::stringstream dbReplayFile;
            for (size_t i = 0; i < PlayData.courseStageData.size(); ++i)
            {
                if (i != 0) dbReplayFile << "|";
                dbReplayFile << PlayData.courseStageData[i].replayPathNew.u8string();
            }
            score.replayFileName = dbReplayFile.str();

            score.score = summary[ARG_SCORE];
            score.exscore = summary[ARG_EXSCORE];
            score.fast = summary[ARG_FAST];
            score.slow = summary[ARG_SLOW];

            if (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask == 0)
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

            score.lamp = LampType::NOPLAY;
            if (isclear)
            {
                if (summary[ARG_MAXCOMBO] == summary[ARG_TOTAL_NOTES])
                {
                    score.lamp = LampType::FULLCOMBO;
                }
                else if (PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge == PlayModifierGaugeType::CLASS_HARD)
                {
                    score.lamp = LampType::HARD;
                }
                else if (PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge == PlayModifierGaugeType::CLASS_NORMAL)
                {
                    score.lamp = LampType::NORMAL;
                }
            }
            else
            {
                score.lamp = LampType::FAILED;
            }

            // 
            g_pScoreDB->updateCourseScoreBMS(PlayData.courseHash, score);
        }

        PlayData.clearContextPlay();
        PlayData.isAuto = false;
        PlayData.isReplay = false;
        SystemData.gNextScene = SystemData.quitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneCourseResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - SystemData.timers["scene_start"] < pSkin->info.timeIntro) return;

    if ((_inputAvailable & m & (INPUT_MASK_DECIDE | INPUT_MASK_CANCEL)).any() || m[Input::ESC])
    {
        switch (state)
        {
        case eCourseResultState::DRAW:
            ResultData.timers["sub_page"] = t.norm();
            // TODO play hit sound
            state = eCourseResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eCourseResultState::STOP:
            if (PlayData.battleType != PlayModifierBattleType::LocalBattle && 
                PlayData.battleType != PlayModifierBattleType::DoubleBattle &&
                !PlayData.isReplay)
            {
                ResultData.timers["sub_page"] = t.norm();
                // TODO stop result sound
                // TODO play record sound
                state = eCourseResultState::RECORD;
                LOG_DEBUG << "[Result] State changed to RECORD";
            }
            else
            {
                SystemData.timers["fadeout"] = t.norm();
                state = eCourseResultState::FADEOUT;
                SoundMgr::setSysVolume(0.0, 2000);
                SoundMgr::setNoteVolume(0.0, 2000);
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eCourseResultState::RECORD:
            if (_scoreSyncFinished)
            {
                SystemData.timers["fadeout"] = t.norm();
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
    if (t - SystemData.timers["scene_start"] < pSkin->info.timeIntro) return;

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
    if (t - SystemData.timers["scene_start"] < pSkin->info.timeIntro) return;
}

}
