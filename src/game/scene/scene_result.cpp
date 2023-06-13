#include "common/pch.h"
#include "scene_result.h"
#include "common/types.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

#include "game/data/data_types.h"

#include "config/config_mgr.h"

#include "game/replay/replay_chart.h"
#include "db/db_score.h"
#include "db/db_song.h"

namespace lunaticvibes
{

SceneResult::SceneResult() : SceneBase(SkinType::RESULT, 1000)
{
    _type = SceneType::RESULT;

    _inputAvailable = INPUT_MASK_FUNC;

    if (PlayData.player[PLAYER_SLOT_PLAYER].chartObj != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }

    if (PlayData.player[PLAYER_SLOT_TARGET].chartObj != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    state = eResultState::DRAW;

    saveLampMax = LampType::NOPLAY;
    if (!PlayData.isReplay)
    {
        const auto [saveScoreType, saveLampMaxType] = getMaxSaveScoreType();
        saveScore = saveScoreType;
        switch (saveLampMaxType)
        {
        case LampType::NOPLAY:      saveLampMax = LampType::NOPLAY; break;
        case LampType::FAILED:      saveLampMax = LampType::FAILED; break;
        case LampType::ASSIST:      saveLampMax = LampType::ASSIST; break;
        case LampType::EASY:        saveLampMax = LampType::EASY; break;
        case LampType::NORMAL:      saveLampMax = LampType::NORMAL; break;
        case LampType::HARD:        saveLampMax = LampType::HARD; break;
        case LampType::EXHARD:      saveLampMax = LampType::EXHARD; break;
        case LampType::FULLCOMBO:   saveLampMax = LampType::FULLCOMBO; break;
        case LampType::PERFECT:     saveLampMax = LampType::PERFECT; break;
        case LampType::MAX:         saveLampMax = LampType::MAX; break;
        default: assert(false); break;
        }
    }
    switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge)
    {
    case PlayModifierGaugeType::ASSISTEASY: saveLampMax = std::min(saveLampMax, LampType::ASSIST); break;
    case PlayModifierGaugeType::EASY:       saveLampMax = std::min(saveLampMax, LampType::EASY); break;
    case PlayModifierGaugeType::NORMAL:     saveLampMax = std::min(saveLampMax, LampType::NORMAL); break;
    case PlayModifierGaugeType::HARD:       saveLampMax = std::min(saveLampMax, LampType::HARD); break;
    case PlayModifierGaugeType::EXHARD:     saveLampMax = std::min(saveLampMax, LampType::EXHARD); break;
    case PlayModifierGaugeType::DEATH:      saveLampMax = std::min(saveLampMax, LampType::FULLCOMBO); break;
    default: 
        if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getMaxCombo() != PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData().maxCombo)
            saveLampMax = LampType::NOPLAY;
        break;
    }

    LOG_INFO << "[Result] " << (ResultData.cleared ? "CLEARED" : "FAILED");

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneResult::inputGameRelease, this, _1, _2));

    Time t;
    ResultData.timers["graph_start"] = t.norm();

    //if (!LR2CustomizeData.isInCustomize)
    {
        SoundMgr::stopSysSamples();

        if (ResultData.cleared)
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_CLEAR);
        else
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_FAIL);
    }
}

////////////////////////////////////////////////////////////////////////////////

SceneResult::~SceneResult()
{
    _input.loopEnd();
    loopEnd();
}

void SceneResult::_updateAsync()
{
    if (SystemData.gNextScene != SceneType::RESULT) return;

    if (SystemData.isAppExiting)
    {
        SystemData.gNextScene = SceneType::EXIT_TRANS;
    }

    switch (state)
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
    case eResultState::WAIT_ARENA:
        updateWaitArena();
        break;
    }

    if (ArenaData.isOnline() && ArenaData.isExpired())
    {
        ArenaData.reset();
    }
}

void SceneResult::updateDraw()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];

    if (rt.norm() >= pSkin->info.timeResultRank)
    {
        ResultData.timers["graph_end"] = t.norm();
        // TODO play hit sound
        state = eResultState::STOP;
        LOG_DEBUG << "[Result] State changed to STOP";
    }
}

void SceneResult::updateStop()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];
}

void SceneResult::updateRecord()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];

    // TODO sync score in online mode?
    if (true)
    {
        _scoreSyncFinished = true;
    }
}

void SceneResult::updateFadeout()
{
    auto t = Time();
    auto rt = t - SystemData.timers["scene_start"];
    auto ft = t - SystemData.timers["fadeout_start"];

    if (ft >= pSkin->info.timeOutro)
    {
        SoundMgr::stopNoteSamples();

        std::string replayFileName = (boost::format("%04d%02d%02d-%02d%02d%02d.rep")
            % SystemData.dateYear
            % SystemData.dateMonthOfYear
            % SystemData.dateDayOfMonth
            % SystemData.timeHour
            % SystemData.timeMin
            % SystemData.timeSec
            ).str();
        Path replayPath = ConfigMgr::Profile()->getPath() / "replay" / "chart" / SelectData.selectedChart.hash.hexdigest() / replayFileName;

        // save replay
        if (saveScore)
        {
            PlayData.replayNew->saveFile(replayPath);
        }

        // save score
        if (saveScore && !SelectData.selectedChart.hash.empty())
        {
            assert(PlayData.player[PLAYER_SLOT_PLAYER].ruleset != nullptr);
            auto& format = SelectData.selectedChart.chart;
            std::shared_ptr<ScoreBase> pScore = nullptr;

            switch (format->type())
            {
            case eChartFormat::BMS:
            case eChartFormat::BMSON:
            {
                auto score = std::make_shared<ScoreBMS>();

                auto& chart = PlayData.player[PLAYER_SLOT_PLAYER].chartObj;
                auto& ruleset = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
                const auto data = ruleset->getData();
                score->notes = chart->getNoteTotalCount();
                score->rate = data.total_acc;
                score->maxcombo = data.maxCombo;
                score->playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
                auto isclear = ruleset->isCleared() ? 1 : 0;
                score->clearcount = _pScoreOld ? _pScoreOld->clearcount + isclear : isclear;
                score->replayFileName = replayFileName;

                auto rBMS = std::dynamic_pointer_cast<RulesetBMS>(ruleset);
                score->score = int(std::floor(rBMS->getScore()));
                score->exscore = rBMS->getExScore();
                score->fast = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_EARLY);
                score->slow = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_LATE);
                score->lamp = saveLampMax;

                if (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask == 0)
                {
                    score->pgreat = rBMS->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
                    score->great = rBMS->getJudgeCount(RulesetBMS::JudgeType::GREAT);
                    score->good = rBMS->getJudgeCount(RulesetBMS::JudgeType::GOOD);
                    score->bad = rBMS->getJudgeCount(RulesetBMS::JudgeType::BAD);
                    score->kpoor = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_KPOOR);
                    score->miss = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_MISS);
                    score->bp = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_BP);
                    score->combobreak = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_CB);
                }

                g_pScoreDB->updateChartScoreBMS(SelectData.selectedChart.hash, *score);
                pScore = score;

                break;
            }
            default:
                break;
            }

            // update entry list score
            SelectData.songList.updateScore(SelectData.selectedChart.hash, pScore);
        }

        // check retry
        if (_retryRequested && PlayData.canRetry)
        {
            SoundMgr::stopSysSamples();

            // update mybest
            if (PlayData.battleType != PlayModifierBattleType::LocalBattle &&
                PlayData.battleType != PlayModifierBattleType::DoubleBattle)
            {
                auto pScore = g_pScoreDB->getChartScoreBMS(SelectData.selectedChart.hash);
                if (pScore && !pScore->replayFileName.empty())
                {
                    Path replayFilePath = ReplayChart::getReplayPath(SelectData.selectedChart.hash) / pScore->replayFileName;
                    if (!replayFilePath.empty() && fs::is_regular_file(replayFilePath))
                    {
                        PlayData.replayMybest = std::make_shared<ReplayChart>();
                        if (PlayData.replayMybest->loadFile(replayFilePath))
                        {
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.randomLeft = PlayData.replayMybest->randomTypeLeft;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.randomRight = PlayData.replayMybest->randomTypeRight;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.gauge = PlayData.replayMybest->gaugeType;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.assist_mask = PlayData.replayMybest->assistMask;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.hispeedFix = PlayData.replayMybest->hispeedFix;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.laneEffect = (PlayModifierLaneEffectType)PlayData.replayMybest->laneEffectType;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.DPFlip = PlayData.replayMybest->DPFlip;
                        }
                        else
                        {
                            PlayData.replayMybest.reset();
                        }
                    }
                }
            }

            PlayData.clearContextPlayForRetry();
            SystemData.gNextScene = SceneType::PLAY;
        }
        else if (PlayData.courseStage >= 0)
        {
            if (saveScore)
                PlayData.courseStageData[PlayData.courseStage].replayPathNew = replayPath;
            else
                PlayData.courseStageData[PlayData.courseStage].replayPathNew = Path();

            if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset)
            {
                PlayData.player[PLAYER_SLOT_PLAYER].initialHealth = PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData().health;
                PlayData.courseStageData[PlayData.courseStage].rulesetCopy[PLAYER_SLOT_PLAYER] = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
                PlayData.player[PLAYER_SLOT_PLAYER].courseRunningCombo = PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData().comboEx;
                PlayData.player[PLAYER_SLOT_PLAYER].courseMaxCombo = PlayData.player[PLAYER_SLOT_PLAYER].ruleset->getData().maxComboDisplay;
            }
            if (PlayData.player[PLAYER_SLOT_TARGET].ruleset)
            {
                PlayData.player[PLAYER_SLOT_TARGET].initialHealth = PlayData.player[PLAYER_SLOT_TARGET].ruleset->getData().health;
                PlayData.courseStageData[PlayData.courseStage].rulesetCopy[PLAYER_SLOT_TARGET] = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
                PlayData.player[PLAYER_SLOT_TARGET].courseRunningCombo = PlayData.player[PLAYER_SLOT_TARGET].ruleset->getData().comboEx;
                PlayData.player[PLAYER_SLOT_TARGET].courseMaxCombo = PlayData.player[PLAYER_SLOT_TARGET].ruleset->getData().maxComboDisplay;
            }

            PlayData.courseStage++;
            if (PlayData.courseStage < PlayData.courseStageData.size())
            {
                if (PlayData.isReplay)
                {
                    PlayData.replay = std::make_shared<ReplayChart>();
                    PlayData.replay->loadFile(PlayData.courseStageData[PlayData.courseStage].replayPath);
                }

                // set metadata
                auto pChart = *g_pSongDB->findChartByHash(PlayData.courseStageData[PlayData.courseStage].hash).begin();
                SelectData.selectedChart.chart = pChart;

                auto& nextChart = *SelectData.selectedChart.chart;
                //SelectData.selectedChart.path = chart._filePath;
                SelectData.selectedChart.path = nextChart.absolutePath;

                // only reload resources if selected chart is different
                if (SelectData.selectedChart.hash != nextChart.fileHash)
                {
                    SelectData.selectedChart.isSampleLoaded = false;
                    SelectData.selectedChart.sampleLoadedHash.reset();
                    SelectData.selectedChart.isBgaLoaded = false;
                    SelectData.selectedChart.bgaLoadedHash.reset();
                }
                SelectData.selectedChart.hash = nextChart.fileHash;

                //SelectData.selectedChart.chart = std::make_shared<ChartFormatBase>(chart);
                SelectData.selectedChart.title = nextChart.title;
                SelectData.selectedChart.title2 = nextChart.title2;
                SelectData.selectedChart.artist = nextChart.artist;
                SelectData.selectedChart.artist2 = nextChart.artist2;
                SelectData.selectedChart.genre = nextChart.genre;
                SelectData.selectedChart.version = nextChart.version;
                SelectData.selectedChart.level = nextChart.levelEstimated;
                SelectData.selectedChart.minBPM = nextChart.minBPM;
                SelectData.selectedChart.maxBPM = nextChart.maxBPM;
                SelectData.selectedChart.startBPM = nextChart.startBPM;

                auto pScore = g_pScoreDB->getChartScoreBMS(SelectData.selectedChart.hash);
                if (pScore && !pScore->replayFileName.empty())
                {
                    Path replayFilePath = ReplayChart::getReplayPath(SelectData.selectedChart.hash) / pScore->replayFileName;
                    if (fs::is_regular_file(replayFilePath))
                    {
                        PlayData.replayMybest = std::make_shared<ReplayChart>();
                        if (PlayData.replayMybest->loadFile(replayFilePath))
                        {
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.randomLeft = PlayData.replayMybest->randomTypeLeft;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.randomRight = PlayData.replayMybest->randomTypeRight;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.gauge = PlayData.replayMybest->gaugeType;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.assist_mask = PlayData.replayMybest->assistMask;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.hispeedFix = PlayData.replayMybest->hispeedFix;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.laneEffect = (PlayModifierLaneEffectType)PlayData.replayMybest->laneEffectType;
                            PlayData.player[PLAYER_SLOT_MYBEST].mods.DPFlip = PlayData.replayMybest->DPFlip;
                        }
                        else
                        {
                            PlayData.replayMybest.reset();
                        }
                    }
                }
                PlayData.clearContextPlayForRetry();
                SystemData.gNextScene = SceneType::PLAY;
            }
            else
            {
                SystemData.gNextScene = SceneType::COURSE_RESULT;
            }
        }
        else
        {
            PlayData.clearContextPlay();
            PlayData.isAuto = false;
            PlayData.isReplay = false;
            SystemData.gNextScene = SystemData.quitOnFinish ? SceneType::EXIT_TRANS : SceneType::SELECT;
        }
    }
}

void SceneResult::updateWaitArena()
{
    assert(ArenaData.isOnline());

    Time t;
    if (!ArenaData.isOnline() || !ArenaData.isArenaReady)
    {
        SystemData.timers["fadeout_start"] = t.norm();
        state = eResultState::FADEOUT;
        SoundMgr::setSysVolume(0.0, 2000);
        SoundMgr::setNoteVolume(0.0, 2000);
        LOG_DEBUG << "[Result] State changed to FADEOUT";
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - SystemData.timers["scene_start"] < pSkin->info.timeIntro) return;

    if ((_inputAvailable & m & (INPUT_MASK_DECIDE | INPUT_MASK_CANCEL)).any() || m[Input::ESC])
    {
        switch (state)
        {
        case eResultState::DRAW:
            ResultData.timers["graph_end"] = t.norm();
            // TODO play hit sound
            state = eResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eResultState::STOP:
            if (saveScore)
            {
                ResultData.timers["sub_page"] = t.norm();
                // TODO stop result sound
                // TODO play record sound
                state = eResultState::RECORD;
                LOG_DEBUG << "[Result] State changed to RECORD";
            }
            else if (ArenaData.isOnline())
            {
                if (ArenaData.isClient())
                    g_pArenaClient->setResultFinished();
                else
                    g_pArenaHost->setResultFinished();

                ArenaData.timers["result_wait"] = t.norm();
                state = eResultState::WAIT_ARENA;
                LOG_DEBUG << "[Result] State changed to WAIT_ARENA";
            }
            else
            {
                SystemData.timers["fadeout_start"] = t.norm();
                state = eResultState::FADEOUT;
                SoundMgr::setSysVolume(0.0, 2000);
                SoundMgr::setNoteVolume(0.0, 2000);
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eResultState::RECORD:
            if (_scoreSyncFinished)
            {
                if (ArenaData.isOnline())
                {
                    if (ArenaData.isClient())
                        g_pArenaClient->setResultFinished();
                    else
                        g_pArenaHost->setResultFinished();

                    ArenaData.timers["result_wait"] = t.norm();
                    state = eResultState::WAIT_ARENA;
                    LOG_DEBUG << "[Result] State changed to WAIT_ARENA";
                }
                else
                {
                    SystemData.timers["fadeout_start"] = t.norm();
                    state = eResultState::FADEOUT;
                    SoundMgr::setSysVolume(0.0, 2000);
                    SoundMgr::setNoteVolume(0.0, 2000);
                    LOG_DEBUG << "[Result] State changed to FADEOUT";
                }
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
    if (t - SystemData.timers["scene_start"] < pSkin->info.timeIntro) return;

    if (state == eResultState::FADEOUT)
    {
        _retryRequested =
            (_inputAvailable & m & INPUT_MASK_DECIDE).any() &&
            (_inputAvailable & m & INPUT_MASK_CANCEL).any();
    }
}

// CALLBACK
void SceneResult::inputGameRelease(InputMask& m, const Time& t)
{
    if (t - SystemData.timers["scene_start"] < pSkin->info.timeIntro) return;
}

}
