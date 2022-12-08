#include "scene_result.h"
#include "scene_context.h"
#include "common/types.h"
#include "game/ruleset/ruleset.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/sound/sound_mgr.h"
#include "game/sound/sound_sample.h"

#include "game/arena/arena_data.h"
#include "game/arena/arena_client.h"
#include "game/arena/arena_host.h"

#include "config/config_mgr.h"
#include <boost/algorithm/string.hpp>

SceneResult::SceneResult() : vScene(eMode::RESULT, 1000)
{
    _scene = eScene::RESULT;

    _inputAvailable = INPUT_MASK_FUNC;

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_1P;
    }
        
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        _inputAvailable |= INPUT_MASK_2P;
    }

    _state = eResultState::DRAW;

    if (!gPlayContext.isReplay)
    {
        auto& [saveScoreTmp, saveLampTmp] = getSaveScoreType();
        saveScore = saveScoreTmp;
        switch (saveLampTmp)
        {
        case Option::e_lamp_type::LAMP_NOPLAY:      saveLamp = ScoreBMS::Lamp::NOPLAY; break;
        case Option::e_lamp_type::LAMP_FAILED:      saveLamp = ScoreBMS::Lamp::FAILED; break;
        case Option::e_lamp_type::LAMP_ASSIST:      saveLamp = ScoreBMS::Lamp::ASSIST; break;
        case Option::e_lamp_type::LAMP_EASY:        saveLamp = ScoreBMS::Lamp::EASY; break;
        case Option::e_lamp_type::LAMP_NORMAL:      saveLamp = ScoreBMS::Lamp::NORMAL; break;
        case Option::e_lamp_type::LAMP_HARD:        saveLamp = ScoreBMS::Lamp::HARD; break;
        case Option::e_lamp_type::LAMP_EXHARD:      saveLamp = ScoreBMS::Lamp::EXHARD; break;
        case Option::e_lamp_type::LAMP_FULLCOMBO:   saveLamp = ScoreBMS::Lamp::FULLCOMBO; break;
        case Option::e_lamp_type::LAMP_PERFECT:     saveLamp = ScoreBMS::Lamp::PERFECT; break;
        case Option::e_lamp_type::LAMP_MAX:         saveLamp = ScoreBMS::Lamp::MAX; break;
        default: assert(false); break;
        }
    }

    std::map<std::string, int> param;

    if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER])
    {
        gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->updateGlobals();

        // set options
        auto d1p = gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData();
        param["1prank"] = Option::getRankType(d1p.total_acc);
        param["1pmaxcombo"] = d1p.maxCombo;

        if (auto pr = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]); pr)
        {
            param["1pexscore"] = pr->getExScore();
            param["1pbp"] = pr->getJudgeCountEx(RulesetBMS::JUDGE_BP);
        }

        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET])
        {
            gPlayContext.ruleset[PLAYER_SLOT_TARGET]->updateGlobals();

            auto d2p = gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData();
            param["2prank"] = Option::getRankType(d1p.total_acc);
            param["2pmaxcombo"] = d2p.maxCombo;

            if (auto pr = std::dynamic_pointer_cast<RulesetBMS>(gPlayContext.ruleset[PLAYER_SLOT_TARGET]); pr)
            {
                param["2pexscore"] = pr->getExScore();
                param["2pbp"] = pr->getJudgeCountEx(RulesetBMS::JUDGE_BP);
            }

        }
        param["1ptarget"] = param["1pexscore"] - param["2pexscore"];
        param["2ptarget"] = -param["1ptarget"];
        param["winlose"] = (param["1ptarget"] > 0) ? 1 : (param["1ptarget"] < 0) ? 2 : 0;

        // TODO set chart info (total notes, etc.)
        auto chartLength = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm() / 1000;
        param["min"] = int(chartLength / 60);
        param["sec"] = int(chartLength % 60);

        // compare to db record
        auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
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
            param["updatedmaxcombo"] = pScore->maxcombo < d1p.maxCombo;
            param["updatedbp"] = pScore->bp > param["1pbp"];
        }
        else if (saveScore)
        {
            param["dbexscorediff"] = param["1pexscore"];
            param["newexscore"] = param["1pexscore"];
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

        State::set(IndexNumber::PLAY_MIN, param["min"]);
        State::set(IndexNumber::PLAY_SEC, param["sec"]);
        State::set(IndexNumber::PLAY_REMAIN_MIN, param["min"]);
        State::set(IndexNumber::PLAY_REMAIN_SEC, param["sec"]);

        State::set(IndexNumber::RESULT_RECORD_EX_BEFORE, param["dbexscore"]);
        State::set(IndexNumber::RESULT_RECORD_EX_NOW, param["newexscore"]);
        State::set(IndexNumber::RESULT_RECORD_EX_DIFF, param["newexscorediff"]);
        State::set(IndexNumber::RESULT_RECORD_MAXCOMBO_BEFORE, param["dbmaxcombo"]);
        State::set(IndexNumber::RESULT_RECORD_MAXCOMBO_NOW, param["newmaxcombo"]);
        State::set(IndexNumber::RESULT_RECORD_MAXCOMBO_DIFF, param["newmaxcombodiff"]);
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

    LOG_INFO << "[Result] " << (State::get(IndexSwitch::RESULT_CLEAR) ? "CLEARED" : "FAILED");

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&SceneResult::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&SceneResult::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&SceneResult::inputGameRelease, this, _1, _2));

    Time t;
    State::set(IndexTimer::RESULT_GRAPH_START, t.norm());

    if (!gInCustomize)
    {
        SoundMgr::stopSysSamples();

        if (State::get(IndexSwitch::RESULT_CLEAR))
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
    if (gNextScene != eScene::RESULT) return;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
    }

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
    case eResultState::WAIT_ARENA:
        updateWaitArena();
        break;
    }
}

void SceneResult::updateDraw()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);

    if (rt.norm() >= _skin->info.timeResultRank)
    {
        State::set(IndexTimer::RESULT_RANK_START, t.norm());
        // TODO play hit sound
        _state = eResultState::STOP;
        LOG_DEBUG << "[Result] State changed to STOP";
    }
}

void SceneResult::updateStop()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);
}

void SceneResult::updateRecord()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);

    // TODO sync score in online mode?
    if (true)
    {
        _scoreSyncFinished = true;
    }
}

void SceneResult::updateFadeout()
{
    auto t = Time();
    auto rt = t - State::get(IndexTimer::SCENE_START);
    auto ft = t - State::get(IndexTimer::FADEOUT_BEGIN);

    if (ft >= _skin->info.timeOutro)
    {
        SoundMgr::stopNoteSamples();

        std::string replayFileName = (boost::format("%04d%02d%02d-%02d%02d%02d.rep")
            % State::get(IndexNumber::DATE_YEAR)
            % State::get(IndexNumber::DATE_MON)
            % State::get(IndexNumber::DATE_DAY)
            % State::get(IndexNumber::DATE_HOUR)
            % State::get(IndexNumber::DATE_MIN)
            % State::get(IndexNumber::DATE_SEC)
            ).str();
        Path replayPath = ConfigMgr::Profile()->getPath() / "replay" / "chart" / gChartContext.hash.hexdigest() / replayFileName;

        // save replay
        if (saveScore)
        {
            gPlayContext.replayNew->saveFile(replayPath);
        }

        // save score
        if (saveScore && !gChartContext.hash.empty())
        {
            assert(gPlayContext.ruleset[PLAYER_SLOT_PLAYER] != nullptr);
            ScoreBMS score;
            auto& format = gChartContext.chartObj;
            auto& chart = gPlayContext.chartObj[PLAYER_SLOT_PLAYER];
            auto& ruleset = gPlayContext.ruleset[PLAYER_SLOT_PLAYER];
            auto& data = ruleset->getData();
            score.notes = chart->getNoteTotalCount();
            score.rate = data.total_acc;
            score.maxcombo = data.maxCombo;
            score.playcount = _pScoreOld ? _pScoreOld->playcount + 1 : 1;
            auto isclear = ruleset->isCleared() ? 1 : 0;
            score.clearcount = _pScoreOld ? _pScoreOld->clearcount + isclear : isclear;
            score.replayFileName = replayFileName;

            switch (format->type())
            {
            case eChartFormat::BMS:
            case eChartFormat::BMSON:
            {
                auto rBMS = std::dynamic_pointer_cast<RulesetBMS>(ruleset);
                score.score = int(std::floor(rBMS->getScore()));
                score.exscore = rBMS->getExScore();
                score.fast = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_EARLY);
                score.slow = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_LATE);

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
                score.lamp = std::min(score.lamp, saveLamp);

                if (gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask == 0)
                {
                    score.pgreat = rBMS->getJudgeCount(RulesetBMS::JudgeType::PERFECT);
                    score.great = rBMS->getJudgeCount(RulesetBMS::JudgeType::GREAT);
                    score.good = rBMS->getJudgeCount(RulesetBMS::JudgeType::GOOD);
                    score.bad = rBMS->getJudgeCount(RulesetBMS::JudgeType::BAD);
                    score.kpoor = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_KPOOR);
                    score.miss = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_MISS);
                    score.bp = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_BP);
                    score.combobreak = rBMS->getJudgeCountEx(RulesetBMS::JUDGE_CB);
                }
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
            SoundMgr::stopSysSamples();
            clearContextPlayForRetry();
            gNextScene = eScene::PLAY;
        }
        else if (gPlayContext.isCourse)
        {
            if (saveScore)
                gPlayContext.courseStageReplayPathNew.push_back(replayPath);
            else
                gPlayContext.courseStageReplayPathNew.push_back(Path());

            if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER])
            {
                gPlayContext.initialHealth[PLAYER_SLOT_PLAYER] = gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().health;
                gPlayContext.courseStageRulesetCopy[PLAYER_SLOT_PLAYER].push_back(gPlayContext.ruleset[PLAYER_SLOT_PLAYER]);
                gPlayContext.courseRunningCombo[PLAYER_SLOT_PLAYER] = gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().comboDisplay;
                gPlayContext.courseMaxCombo[PLAYER_SLOT_PLAYER] = gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().maxComboDisplay;
            }
            if (gPlayContext.ruleset[PLAYER_SLOT_TARGET])
            {
                gPlayContext.initialHealth[PLAYER_SLOT_TARGET] = gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData().health;
                gPlayContext.courseStageRulesetCopy[PLAYER_SLOT_TARGET].push_back(gPlayContext.ruleset[PLAYER_SLOT_TARGET]);
                gPlayContext.courseRunningCombo[PLAYER_SLOT_TARGET] = gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData().comboDisplay;
                gPlayContext.courseMaxCombo[PLAYER_SLOT_TARGET] = gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData().maxComboDisplay;
            }

            gPlayContext.courseStage++;
            if (gPlayContext.courseStage < gPlayContext.courseCharts.size())
            {
                if (gPlayContext.isReplay)
                {
                    gPlayContext.replay = std::make_shared<ReplayChart>();
                    gPlayContext.replay->loadFile(gPlayContext.courseStageReplayPath[gPlayContext.courseStage]);
                }

                if (gPlayContext.courseStage + 1 == gPlayContext.courseCharts.size())
                    State::set(IndexOption::PLAY_COURSE_STAGE, Option::STAGE_FINAL);
                else
                    State::set(IndexOption::PLAY_COURSE_STAGE, Option::STAGE_1 + gPlayContext.courseStage);

                // set metadata
                auto pChart = *g_pSongDB->findChartByHash(gPlayContext.courseCharts[gPlayContext.courseStage]).begin();
                gChartContext.chartObj = pChart;

                auto& chart = *gChartContext.chartObj;
                //gChartContext.path = chart._filePath;
                gChartContext.path = chart.absolutePath;

                // only reload resources if selected chart is different
                if (gChartContext.hash != chart.fileHash)
                {
                    gChartContext.isBgaLoaded = false;
                    gChartContext.isSampleLoaded = false;
                }
                gChartContext.hash = chart.fileHash;

                //gChartContext.chartObj = std::make_shared<ChartFormatBase>(chart);
                gChartContext.title = chart.title;
                gChartContext.title2 = chart.title2;
                gChartContext.artist = chart.artist;
                gChartContext.artist2 = chart.artist2;
                gChartContext.genre = chart.genre;
                gChartContext.version = chart.version;
                gChartContext.level = chart.levelEstimated;
                gChartContext.minBPM = chart.minBPM;
                gChartContext.maxBPM = chart.maxBPM;
                gChartContext.startBPM = chart.startBPM;

                auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
                if (pScore && !pScore->replayFileName.empty())
                {
                    Path replayFilePath = ReplayChart::getReplayPath(gChartContext.hash) / pScore->replayFileName;
                    if (fs::is_regular_file(replayFilePath))
                    {
                        gPlayContext.replayMybest = std::make_shared<ReplayChart>();
                        if (gPlayContext.replayMybest->loadFile(replayFilePath))
                        {
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].randomLeft = gPlayContext.replayMybest->randomTypeLeft;
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].randomRight = gPlayContext.replayMybest->randomTypeRight;
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].gauge = gPlayContext.replayMybest->gaugeType;
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].assist_mask = gPlayContext.replayMybest->assistMask;
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].hispeedFix = gPlayContext.replayMybest->hispeedFix;
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].laneEffect = (eModLaneEffect)gPlayContext.replayMybest->laneEffectType;
                            gPlayContext.mods[PLAYER_SLOT_MYBEST].DPFlip = gPlayContext.replayMybest->DPFlip;
                        }
                        else
                        {
                            gPlayContext.replayMybest.reset();
                        }
                    }
                }
                clearContextPlayForRetry();
                gNextScene = eScene::PLAY;
            }
            else
            {
                gNextScene = eScene::COURSE_RESULT;
            }
        }
        else
        {
            clearContextPlay();
            gPlayContext.isAuto = false;
            gPlayContext.isReplay = false;
            gNextScene = gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT;
        }
    }
}

void SceneResult::updateWaitArena()
{
    assert(gArenaData.isOnline());

    Time t;
    if (!gSelectContext.isArenaReady)
    {
        State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
        _state = eResultState::FADEOUT;
        SoundMgr::setSysVolume(0.0, 2000);
        SoundMgr::setNoteVolume(0.0, 2000);
        LOG_DEBUG << "[Result] State changed to FADEOUT";
    }
}

////////////////////////////////////////////////////////////////////////////////

// CALLBACK
void SceneResult::inputGamePress(InputMask& m, const Time& t)
{
    if (t - State::get(IndexTimer::SCENE_START) < _skin->info.timeIntro) return;

    if ((_inputAvailable & m & (INPUT_MASK_DECIDE | INPUT_MASK_CANCEL)).any() || m[Input::ESC])
    {
        switch (_state)
        {
        case eResultState::DRAW:
            State::set(IndexTimer::RESULT_RANK_START, t.norm());
            // TODO play hit sound
            _state = eResultState::STOP;
            LOG_DEBUG << "[Result] State changed to STOP";
            break;

        case eResultState::STOP:
            if (saveScore)
            {
                State::set(IndexTimer::RESULT_HIGHSCORE_START, t.norm());
                // TODO stop result sound
                // TODO play record sound
                _state = eResultState::RECORD;
                LOG_DEBUG << "[Result] State changed to RECORD";
            }
            else if (gArenaData.isOnline())
            {
                if (gArenaData.isClient())
                    g_pArenaClient->setResultFinished();
                else
                    g_pArenaHost->setResultFinished();

                State::set(IndexTimer::ARENA_RESULT_WAIT, t.norm());
                _state = eResultState::WAIT_ARENA;
                LOG_DEBUG << "[Result] State changed to WAIT_ARENA";
            }
            else
            {
                State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
                _state = eResultState::FADEOUT;
                SoundMgr::setSysVolume(0.0, 2000);
                SoundMgr::setNoteVolume(0.0, 2000);
                LOG_DEBUG << "[Result] State changed to FADEOUT";
            }
            break;

        case eResultState::RECORD:
            if (_scoreSyncFinished)
            {
                if (gArenaData.isOnline())
                {
                    if (gArenaData.isClient())
                        g_pArenaClient->setResultFinished();
                    else
                        g_pArenaHost->setResultFinished();

                    State::set(IndexTimer::ARENA_RESULT_WAIT, t.norm());
                    _state = eResultState::WAIT_ARENA;
                    LOG_DEBUG << "[Result] State changed to WAIT_ARENA";
                }
                else
                {
                    State::set(IndexTimer::FADEOUT_BEGIN, t.norm());
                    _state = eResultState::FADEOUT;
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
    if (t - State::get(IndexTimer::SCENE_START) < _skin->info.timeIntro) return;

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
    if (t - State::get(IndexTimer::SCENE_START) < _skin->info.timeIntro) return;
}
