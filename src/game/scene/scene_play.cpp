#include <cassert>
#include <future>
#include <set>
#include "scene_play.h"
#include "scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"
#include "game/ruleset/ruleset_bms_replay.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/chart/chart_bms.h"
#include "game/graphics/sprite_video.h"
#include "config/config_mgr.h"
#include "common/log.h"
#include "common/sysutil.h"
#include "game/sound/sound_sample.h"
#include "game/skin/skin_lr2_button_callbacks.h"
#include "game/skin/skin_lr2_slider_callbacks.h"

bool ScenePlay::isPlaymodeDP() const
{
    switch (gPlayContext.mode)
    {
        case eMode::PLAY10:
        case eMode::PLAY14:
            return true;
    }
    return false;
}
bool ScenePlay::isPlaymodeBattle() const
{
    return gPlayContext.isBattle;
}

ScenePlay::ScenePlay(): vScene(gPlayContext.mode, 1000, true)
{
    _scene = eScene::PLAY;

    assert(!isPlaymodeDP() || !isPlaymodeBattle());

    if (!isPlaymodeDP() && !isPlaymodeBattle())
    {
        _input.setMergeInput();
    }

    _currentKeySample.assign(Input::ESC, 0);

    _lanecoverEnabled[PLAYER_SLOT_PLAYER] = ConfigMgr::get('P', cfg::P_LANECOVER_ENABLE, false);
    if (!isPlaymodeBattle())
    {
        _lanecoverEnabled[PLAYER_SLOT_TARGET] = gPlayContext.battle2PLanecover;
    }

    int lcTop1 = ConfigMgr::get('P', cfg::P_LANECOVER_TOP, 0);
    int lcBottom1 = ConfigMgr::get('P', cfg::P_LANECOVER_BOTTOM, 0);
    gNumbers.queue(eNumber::LANECOVER_TOP_1P, lcTop1);
    gNumbers.queue(eNumber::LANECOVER_BOTTOM_1P, lcBottom1);
    gNumbers.queue(eNumber::LANECOVER100_1P, lcTop1 / 10);

    int lcTop2 = gPlayContext.battle2PLanecoverTop;
    int lcBottom2 = gPlayContext.battle2PLanecoverBottom;
    gNumbers.queue(eNumber::LANECOVER_TOP_2P, lcTop2);
    gNumbers.queue(eNumber::LANECOVER_BOTTOM_2P, lcBottom2);
    gNumbers.queue(eNumber::LANECOVER100_2P, lcTop2 / 10);

    if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
    {
        gSliders.queue(eSlider::SUD_1P, lcTop1 / 1000.0);
        if (isPlaymodeDP())
        {
            gSliders.queue(eSlider::SUD_2P, lcTop1 / 1000.0);
        }
        // TODO HID+, LIFT
    }
    if (isPlaymodeBattle() && _lanecoverEnabled[PLAYER_SLOT_TARGET])
    {
        gSliders.queue(eSlider::SUD_2P, lcTop2 / 1000.0);
    }

    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P | INPUT_MASK_2P;

    _state = ePlayState::PREPARE;

    if (gPlayContext.isReplay && gPlayContext.replay)
    {
        gSwitches.set(eSwitch::SOUND_PITCH, true);
        gOptions.set(eOption::SOUND_PITCH_TYPE, gPlayContext.replay->pitchType);
        double ps = (gPlayContext.replay->pitchValue + 12) / 24.0;
        lr2skin::slider::pitch(ps);
    }

    if (gChartContext.chartObj == nullptr || !gChartContext.chartObj->isLoaded())
    {
        if (gChartContext.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            gNextScene = gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT;
            return;
        }
        if (gPlayContext.isReplay && gPlayContext.replay)
        {
            gChartContext.chartObj = ChartFormatBase::createFromFile(gChartContext.path, gPlayContext.replay->randomSeed);
        }
        else
        {
            gChartContext.chartObj = ChartFormatBase::createFromFile(gChartContext.path, gPlayContext.randomSeed);
        }
    }
    if (gChartContext.chartObj == nullptr || !gChartContext.chartObj->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart: " << gChartContext.path.u8string();
        gNextScene = gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT;
        return;
    }

    if (gPlayContext.replayMybest)
    {
        gChartContext.chartObjMybest = ChartFormatBase::createFromFile(gChartContext.path, gPlayContext.replayMybest->randomSeed);
    }

    clearGlobalDatas();
    // global info
    // 
    // basic info
    gTexts.queue(eText::PLAY_TITLE, gChartContext.title);
    gTexts.queue(eText::PLAY_SUBTITLE, gChartContext.title2);
    if (gChartContext.title2.empty())
        gTexts.queue(eText::PLAY_FULLTITLE, gChartContext.title);
    else
        gTexts.queue(eText::PLAY_FULLTITLE, gChartContext.title + " " + gChartContext.title2);
    gTexts.queue(eText::PLAY_ARTIST, gChartContext.artist);
    gTexts.queue(eText::PLAY_SUBARTIST, gChartContext.artist2);
    gTexts.queue(eText::PLAY_GENRE, gChartContext.genre);
    gNumbers.queue(eNumber::PLAY_BPM, int(std::round(gChartContext.startBPM)));
    gNumbers.queue(eNumber::INFO_BPM_MIN, int(std::round(gChartContext.minBPM)));
    gNumbers.queue(eNumber::INFO_BPM_MAX, int(std::round(gChartContext.maxBPM)));

    gOptions.queue(eOption::PLAY_RANK_ESTIMATED_1P, Option::RANK_NONE);
    gOptions.queue(eOption::PLAY_RANK_ESTIMATED_2P, Option::RANK_NONE);
    gOptions.queue(eOption::PLAY_RANK_BORDER_1P, Option::RANK_NONE);
    gOptions.queue(eOption::PLAY_RANK_BORDER_2P, Option::RANK_NONE);

    lr2skin::button::target_type(0);

    gChartContext.title = gChartContext.chartObj->title;
    gChartContext.title2 = gChartContext.chartObj->title2;
    gChartContext.artist = gChartContext.chartObj->artist;
    gChartContext.artist2 = gChartContext.chartObj->artist2;
    gChartContext.genre = gChartContext.chartObj->genre;
    gChartContext.minBPM = gChartContext.chartObj->minBPM;
    gChartContext.startBPM = gChartContext.chartObj->startBPM;
    gChartContext.maxBPM = gChartContext.chartObj->maxBPM;

    _chartLoaded = createChartObj();
    gPlayContext.remainTime = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength();

    _rulesetLoaded = createRuleset();

    if (ConfigMgr::get('P', cfg::P_LOCK_SPEED, false))
    {
        _lockspeedEnabled[PLAYER_SLOT_PLAYER] = true;

        double bpm = gChartContext.startBPM * gSelectContext.pitchSpeed;
        switch (gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix)
        {
        case eModHs::MAXBPM:   bpm = gChartContext.maxBPM * gSelectContext.pitchSpeed; break;
        case eModHs::MINBPM:   bpm = gChartContext.minBPM * gSelectContext.pitchSpeed; break;
        case eModHs::AVERAGE:  bpm = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getAverageBPM(); break;
        case eModHs::CONSTANT: bpm = 150.0; break;
        case eModHs::NONE:     
        default:               bpm = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM(); break;
        }

        int green = ConfigMgr::get('P', cfg::P_GREENNUMBER, 1200);
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
        double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;

        gPlayContext.Hispeed = (green * bpm <= 0.0) ? 200 : std::min(visible * 120.0 * 1200 / green / bpm, 10.0);
        gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
        _lockspeedValue[PLAYER_SLOT_PLAYER] = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * gPlayContext.Hispeed / visible);
        _lockspeedGreenNumber[PLAYER_SLOT_PLAYER] = green;
    }
    if (isPlaymodeBattle() && gPlayContext.battle2PLockSpeed)
    {
        _lockspeedEnabled[PLAYER_SLOT_TARGET] = true;

        double bpm = gChartContext.startBPM;
        switch (gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix)
        {
        case eModHs::MAXBPM:   bpm = gChartContext.maxBPM * gSelectContext.pitchSpeed; break;
        case eModHs::MINBPM:   bpm = gChartContext.minBPM * gSelectContext.pitchSpeed; break;
        case eModHs::AVERAGE:  bpm = gPlayContext.chartObj[PLAYER_SLOT_TARGET]->getAverageBPM(); break;
        case eModHs::CONSTANT: bpm = 150.0; break;
        case eModHs::NONE:
        default:               bpm = gPlayContext.chartObj[PLAYER_SLOT_TARGET]->getCurrentBPM(); break;
        }

        int green = gPlayContext.battle2PGreenNumber;
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
        double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;

        gPlayContext.battle2PHispeed = (green * bpm <= 0.0) ? 200 : std::min(visible * 120.0 * 1200 / green / bpm, 10.0);
        gNumbers.queue(eNumber::HS_2P, (int)std::round(gPlayContext.battle2PHispeed));
        _lockspeedValue[PLAYER_SLOT_TARGET] = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * gPlayContext.battle2PHispeed / visible);
        _lockspeedGreenNumber[PLAYER_SLOT_TARGET] = green;
    }
    gPlayContext.HispeedGradientStart = TIMER_NEVER;
    gPlayContext.HispeedGradientFrom = gPlayContext.HispeedGradientNow;
    gPlayContext.HispeedGradientNow = gPlayContext.Hispeed;
    gPlayContext.battle2PHispeedGradientStart = TIMER_NEVER;
    gPlayContext.battle2PHispeedGradientFrom = gPlayContext.battle2PHispeed;
    gPlayContext.battle2PHispeedGradientNow = gPlayContext.battle2PHispeed;

    // apply datas
    gTexts.flush();
    gNumbers.flush();
    gBargraphs.flush();
    gSliders.flush();
    gOptions.flush();

    // set gauge type
    if (gChartContext.chartObj)
    {
        switch (gChartContext.chartObj->type())
        {
        case eChartFormat::BMS:
        case eChartFormat::BMSON:
            setTempInitialHealthBMS();
            break;
        default:
            break;
        }
    }

    {
        using namespace std::string_literals;
        eGaugeOp tmp = eGaugeOp::GROOVE;
        switch (gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge)
        {
        case eModGauge::NORMAL:    tmp = eGaugeOp::GROOVE; break;
        case eModGauge::HARD:      tmp = eGaugeOp::SURVIVAL; break;
        case eModGauge::DEATH:     tmp = eGaugeOp::EX_SURVIVAL; break;
        case eModGauge::EASY:      tmp = eGaugeOp::GROOVE; break;
        // case eModGauge::PATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
        // case eModGauge::GATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
        case eModGauge::ASSISTEASY:tmp = eGaugeOp::GROOVE; break;
        case eModGauge::EXHARD:    tmp = eGaugeOp::EX_SURVIVAL; break;
        default: break;
        }
        _skin->setExtendedProperty("GAUGETYPE_1P"s, (void*)&tmp);

        if (isPlaymodeBattle())
        {
            switch (gPlayContext.mods[PLAYER_SLOT_TARGET].gauge)
            {
            case eModGauge::NORMAL:    tmp = eGaugeOp::GROOVE; break;
            case eModGauge::HARD:      tmp = eGaugeOp::SURVIVAL; break;
            case eModGauge::EASY:      tmp = eGaugeOp::GROOVE; break;
            case eModGauge::DEATH:     tmp = eGaugeOp::EX_SURVIVAL; break;
            // case eModGauge::PATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
            // case eModGauge::GATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
            case eModGauge::ASSISTEASY:tmp = eGaugeOp::GROOVE; break;
            case eModGauge::EXHARD:    tmp = eGaugeOp::EX_SURVIVAL; break;
            default: break;
            }
            _skin->setExtendedProperty("GAUGETYPE_2P"s, (void*)&tmp);
        }
    }

    _missBgaLength = ConfigMgr::get("P", cfg::P_MISSBGA_LENGTH, 500);

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&ScenePlay::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&ScenePlay::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&ScenePlay::inputGameRelease, this, _1, _2));
    _input.register_a("SCENE_AXIS", std::bind(&ScenePlay::inputGameAxis, this, _1, _2, _3));
}

void ScenePlay::clearGlobalDatas()
{
    // reset
    eNumber numbersReset[] =
    {
        eNumber::PLAY_1P_SCORE,
        eNumber::PLAY_1P_EXSCORE,
        eNumber::PLAY_1P_RATE,
        eNumber::PLAY_1P_RATEDECIMAL,
        eNumber::PLAY_1P_NOWCOMBO,
        eNumber::PLAY_1P_MAXCOMBO,
        eNumber::PLAY_1P_EXSCORE_DIFF,
        eNumber::PLAY_1P_PERFECT,
        eNumber::PLAY_1P_GREAT,
        eNumber::PLAY_1P_GOOD,
        eNumber::PLAY_1P_BAD,
        eNumber::PLAY_1P_POOR,
        eNumber::PLAY_1P_TOTAL_RATE,
        eNumber::PLAY_1P_TOTAL_RATE_DECIMAL2,
        eNumber::PLAY_2P_SCORE,
        eNumber::PLAY_2P_EXSCORE,
        eNumber::PLAY_2P_RATE,
        eNumber::PLAY_2P_RATEDECIMAL,
        eNumber::PLAY_2P_NOWCOMBO,
        eNumber::PLAY_2P_MAXCOMBO,
        eNumber::PLAY_2P_EXSCORE_DIFF,
        eNumber::PLAY_2P_PERFECT,
        eNumber::PLAY_2P_GREAT,
        eNumber::PLAY_2P_GOOD,
        eNumber::PLAY_2P_BAD,
        eNumber::PLAY_2P_POOR,
        eNumber::PLAY_2P_TOTAL_RATE,
        eNumber::PLAY_2P_TOTAL_RATE_DECIMAL2,
        eNumber::RESULT_MYBEST_EX,
        eNumber::RESULT_TARGET_EX,
        eNumber::RESULT_MYBEST_DIFF,
        eNumber::RESULT_TARGET_DIFF,
        eNumber::RESULT_NEXT_RANK_EX_DIFF,
        eNumber::RESULT_MYBEST_RATE,
        eNumber::RESULT_MYBEST_RATE_DECIMAL2,
        eNumber::RESULT_TARGET_RATE,
        eNumber::RESULT_TARGET_RATE_DECIMAL2,
        eNumber::_ANGLE_TT_1P,
        eNumber::_ANGLE_TT_2P
    };
    for (auto& e : numbersReset)
    {
        gNumbers.queue(e, 0);
    }

    eBargraph bargraphsReset[] =
    {
        eBargraph::PLAY_EXSCORE,
        eBargraph::PLAY_EXSCORE_PREDICT,
        eBargraph::PLAY_MYBEST_NOW,
        eBargraph::PLAY_MYBEST_FINAL,
        eBargraph::PLAY_RIVAL_EXSCORE,
        eBargraph::PLAY_RIVAL_EXSCORE_FINAL,
        eBargraph::PLAY_1P_SLOW_COUNT,
        eBargraph::PLAY_1P_FAST_COUNT,
        eBargraph::PLAY_2P_SLOW_COUNT,
        eBargraph::PLAY_2P_FAST_COUNT,
    };
    for (auto& e : bargraphsReset)
    {
        gBargraphs.queue(e, 0.0);
    }

    eSlider slidersReset[] =
    {
        eSlider::SONG_PROGRESS
    };
    for (auto& e : slidersReset)
    {
        gSliders.queue(e, 0.0);
    }

    gTexts.flush();
    gNumbers.flush();
    gBargraphs.flush();
    gSliders.flush();
}

bool ScenePlay::createChartObj()
{
    //load chart object from Chart object
    switch (gChartContext.chartObj->type())
    {
    case eChartFormat::BMS:
    {
        auto bms = std::reinterpret_pointer_cast<ChartFormatBMS>(gChartContext.chartObj);

        if (gPlayContext.isAuto || gPlayContext.isReplay)
        {
            gPlayContext.chartObj[PLAYER_SLOT_PLAYER] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);

            if (gPlayContext.isAuto && isPlaymodeBattle())
                gPlayContext.chartObj[PLAYER_SLOT_TARGET] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_TARGET, bms);
            else
                gPlayContext.chartObj[PLAYER_SLOT_TARGET] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);    // create for rival; loading with 1P options

            if (gPlayContext.replayMybest)
                gPlayContext.chartObj[PLAYER_SLOT_MYBEST] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_MYBEST, bms);

            if (gPlayContext.isReplay)
                itReplayCommand = gPlayContext.replay->commands.begin();
        }
        else
        {
            if (isPlaymodeBattle())
            {
                gPlayContext.chartObj[PLAYER_SLOT_PLAYER] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);
                gPlayContext.chartObj[PLAYER_SLOT_TARGET] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_TARGET, bms);
            }
            else
            {
                gPlayContext.chartObj[PLAYER_SLOT_PLAYER] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);
                gPlayContext.chartObj[PLAYER_SLOT_TARGET] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_PLAYER, bms);    // create for rival; loading with 1P options

                if (gPlayContext.replayMybest)
                    gPlayContext.chartObj[PLAYER_SLOT_MYBEST] = std::make_shared<ChartObjectBMS>(PLAYER_SLOT_MYBEST, bms);
            }
        }
        gNumbers.set(eNumber::PLAY_REMAIN_MIN, int(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm() / 1000 / 60));
        gNumbers.set(eNumber::PLAY_REMAIN_SEC, int(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm() / 1000 % 60));
        return true;
    }

    case eChartFormat::BMSON:
    default:
        LOG_WARNING << "[Play] chart format not supported.";
        gNextScene = gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT;
        return false;
    }

}

bool ScenePlay::createRuleset()
{
    // build Ruleset object
    switch (gPlayContext.rulesetType)
    {
    case eRuleset::BMS:
    {
        // set judge diff
        RulesetBMS::JudgeDifficulty judgeDiff;
        switch (gChartContext.chartObj->type())
        {
        case eChartFormat::BMS:
            switch (std::reinterpret_pointer_cast<ChartFormatBMS>(gChartContext.chartObj)->rank)
            {
            case 1: judgeDiff = RulesetBMS::JudgeDifficulty::HARD; break;
            case 2: judgeDiff = RulesetBMS::JudgeDifficulty::NORMAL; break;
            case 3: judgeDiff = RulesetBMS::JudgeDifficulty::EASY; break;
            case 4: judgeDiff = RulesetBMS::JudgeDifficulty::VERYEASY; break;
            case 6: judgeDiff = RulesetBMS::JudgeDifficulty::WHAT; break;
            case 0:
            default:
                judgeDiff = RulesetBMS::JudgeDifficulty::VERYHARD; break;
            }
            break;
        case eChartFormat::BMSON:
        default:
            LOG_WARNING << "[Play] chart format not supported.";
            break;
        }

        unsigned keys = 7;
        switch (gPlayContext.mode)
        {
        case eMode::PLAY5:
        case eMode::PLAY5_2: keys = 5; break;
        case eMode::PLAY7:
        case eMode::PLAY7_2: keys = 7; break;
        case eMode::PLAY9:
        case eMode::PLAY9_2: keys = 9; break;
        case eMode::PLAY10: keys = 10; break;
        case eMode::PLAY14: keys = 14; break;
        defualt: break;
        }

        if (gPlayContext.isAuto)
        {
            gPlayContext.ruleset[PLAYER_SLOT_PLAYER] = std::make_shared<RulesetBMSAuto>(
                gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_PLAYER],
                gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge, keys, judgeDiff,
                gPlayContext.initialHealth[PLAYER_SLOT_PLAYER], RulesetBMS::PlaySide::AUTO);

            if (isPlaymodeBattle())
            {
                if (gPlayContext.replayMybest)
                {
                    gPlayContext.ruleset[PLAYER_SLOT_TARGET] = std::make_shared<RulesetBMSReplay>(
                        gChartContext.chartObjMybest, gPlayContext.chartObj[PLAYER_SLOT_TARGET], gPlayContext.replayMybest,
                        gPlayContext.replayMybest->gaugeType, keys, judgeDiff,
                        gPlayContext.initialHealth[PLAYER_SLOT_TARGET], RulesetBMS::PlaySide::AUTO_2P);
                }
                else
                {
                    gPlayContext.ruleset[PLAYER_SLOT_TARGET] = std::make_shared<RulesetBMSAuto>(
                        gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_TARGET],
                        gPlayContext.mods[PLAYER_SLOT_TARGET].gauge, keys, judgeDiff,
                        gPlayContext.initialHealth[PLAYER_SLOT_TARGET], RulesetBMS::PlaySide::AUTO_2P);
                }
            }
        }
        else if (gPlayContext.isReplay)
        {
            gPlayContext.ruleset[PLAYER_SLOT_PLAYER] = std::make_shared<RulesetBMSReplay>(
                gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_PLAYER], gPlayContext.replay,
                gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge, keys, judgeDiff,
                gPlayContext.initialHealth[PLAYER_SLOT_PLAYER], RulesetBMS::PlaySide::AUTO);

            gPlayContext.ruleset[PLAYER_SLOT_TARGET] = std::make_shared<RulesetBMSAuto>(
                gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_TARGET],
                gPlayContext.mods[PLAYER_SLOT_TARGET].gauge, keys, judgeDiff,
                gPlayContext.initialHealth[PLAYER_SLOT_TARGET], RulesetBMS::PlaySide::RIVAL);

            if (gPlayContext.replayMybest)
            {
                gPlayContext.ruleset[PLAYER_SLOT_MYBEST] = std::make_shared<RulesetBMSReplay>(
                    gChartContext.chartObjMybest, gPlayContext.chartObj[PLAYER_SLOT_MYBEST], gPlayContext.replayMybest,
                    gPlayContext.replayMybest->gaugeType, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_MYBEST], RulesetBMS::PlaySide::MYBEST);
            }
        }
        else
        {
            if (isPlaymodeBattle())
            {
                gPlayContext.ruleset[PLAYER_SLOT_PLAYER] = std::make_shared<RulesetBMS>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_PLAYER],
                    gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_PLAYER], RulesetBMS::PlaySide::BATTLE_1P);

                gPlayContext.ruleset[PLAYER_SLOT_TARGET] = std::make_shared<RulesetBMS>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_TARGET],
                    gPlayContext.mods[PLAYER_SLOT_TARGET].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_TARGET], RulesetBMS::PlaySide::BATTLE_2P);
            }
            else
            {
                gPlayContext.ruleset[PLAYER_SLOT_PLAYER] = std::make_shared<RulesetBMS>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_PLAYER],
                    gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_PLAYER], (keys == 10 || keys == 14) ? RulesetBMS::PlaySide::DOUBLE : RulesetBMS::PlaySide::SINGLE);

                gPlayContext.ruleset[PLAYER_SLOT_TARGET] = std::make_shared<RulesetBMSAuto>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_TARGET],
                    gPlayContext.mods[PLAYER_SLOT_TARGET].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_TARGET], RulesetBMS::PlaySide::RIVAL);

                if (gPlayContext.replayMybest)
                {
                    gPlayContext.ruleset[PLAYER_SLOT_MYBEST] = std::make_shared<RulesetBMSReplay>(
                        gChartContext.chartObjMybest, gPlayContext.chartObj[PLAYER_SLOT_MYBEST], gPlayContext.replayMybest,
                        gPlayContext.replayMybest->gaugeType, keys, judgeDiff,
                        gPlayContext.initialHealth[PLAYER_SLOT_MYBEST], RulesetBMS::PlaySide::MYBEST);
                }

                // create replay
                gPlayContext.replayNew = std::make_shared<ReplayChart>();
                gPlayContext.replayNew->chartHash = gChartContext.hash;
                gPlayContext.replayNew->randomSeed = gPlayContext.randomSeed;
                gPlayContext.replayNew->gaugeType = gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge;
                gPlayContext.replayNew->randomTypeLeft = gPlayContext.mods[PLAYER_SLOT_PLAYER].randomLeft;
                gPlayContext.replayNew->randomTypeRight = gPlayContext.mods[PLAYER_SLOT_PLAYER].randomRight;
                gPlayContext.replayNew->assistMask = gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask;
                gPlayContext.replayNew->hispeedFix = gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix;
                gPlayContext.replayNew->laneEffectType = gPlayContext.mods[PLAYER_SLOT_PLAYER].visual_mask;
                if (gSwitches.get(eSwitch::SOUND_PITCH))
                {
                    gPlayContext.replayNew->pitchType = (int8_t)gOptions.get(eOption::SOUND_PITCH_TYPE);
                    gPlayContext.replayNew->pitchValue = (int8_t)std::round((gSliders.get(eSlider::PITCH) - 0.5) * 2 * 12);
                }
                gPlayContext.replayNew->DPFlip = gPlayContext.mods[PLAYER_SLOT_PLAYER].DPFlip;
            }
        }

        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr && !isPlaymodeBattle())
        {
            double targetRateReal = 0.0;
            switch (gOptions.get(eOption::PLAY_TARGET_TYPE))
            {
            case Option::TARGET_A:   targetRateReal = 8.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK A");   break;
            case Option::TARGET_AA:  targetRateReal = 7.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK AA");  break;
            case Option::TARGET_AAA: targetRateReal = 6.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK AAA"); break;
            default:
            {
                // rename target
                int targetRate = gNumbers.get(eNumber::DEFAULT_TARGET_RATE);
                switch (targetRate)
                {
                case 22:  targetRateReal = 2.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK E"); break;  // E
                case 33:  targetRateReal = 3.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK D"); break;  // D
                case 44:  targetRateReal = 4.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK C"); break;  // C
                case 55:  targetRateReal = 5.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK B"); break;  // B
                case 66:  targetRateReal = 6.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK A");  break;  // A
                case 77:  targetRateReal = 7.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK AA"); break;  // AA
                case 88:  targetRateReal = 8.0 / 9; gTexts.queue(eText::TARGET_NAME, "RANK AAA"); break;  // AAA
                case 100: targetRateReal = 1.0;     gTexts.queue(eText::TARGET_NAME, "DJ AUTO"); break;  // MAX
                default:
                    targetRateReal = targetRate / 100.0;
                    gTexts.queue(eText::TARGET_NAME, "RATE "s + std::to_string(targetRate) + "%"s);
                    break;
                }
            }
            break;
            }

            std::reinterpret_pointer_cast<RulesetBMSAuto>(gPlayContext.ruleset[PLAYER_SLOT_TARGET])->setTargetRate(targetRateReal);
            gBargraphs.queue(eBargraph::PLAY_RIVAL_EXSCORE_FINAL, targetRateReal);
        }

        // load mybest score
        auto pScore = g_pScoreDB->getChartScoreBMS(gChartContext.hash);
        if (pScore)
        {
            gBargraphs.queue(eBargraph::PLAY_MYBEST_FINAL, (double)pScore->exscore / gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxScore());
            if (!gPlayContext.replayMybest)
            {
                gBargraphs.queue(eBargraph::PLAY_MYBEST_NOW, (double)pScore->exscore / gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxScore());
                gNumbers.queue(eNumber::RESULT_MYBEST_EX, pScore->exscore);
                gNumbers.queue(eNumber::RESULT_MYBEST_RATE, (int)std::floor(pScore->rate * 100.0));
                gNumbers.queue(eNumber::RESULT_MYBEST_RATE_DECIMAL2, (int)std::floor(pScore->rate * 10000.0) % 100);
            }
        }

        return true;
    }
    break;

    default:
        break;
    }

    return false;
}

ScenePlay::~ScenePlay()
{
    gPlayContext.bgaTexture->stopUpdate();

    _input.loopEnd();
    loopEnd();
}

void ScenePlay::setTempInitialHealthBMS()
{
    if (!gPlayContext.isCourse || gPlayContext.isCourseFirstStage)
    {
        switch (gPlayContext.mods[PLAYER_SLOT_PLAYER].gauge)
        {
        case eModGauge::NORMAL:
        case eModGauge::EASY:
        case eModGauge::ASSISTEASY:
            gPlayContext.initialHealth[PLAYER_SLOT_PLAYER] = 0.2;
            gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, 20);
            break;

        case eModGauge::HARD:
        case eModGauge::DEATH:
        // case eModGauge::PATTACK:
        // case eModGauge::GATTACK:
        case eModGauge::EXHARD:
            gPlayContext.initialHealth[PLAYER_SLOT_PLAYER] = 1.0;
            gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, 100);
            break;

        default: break;
        }

        if (isPlaymodeBattle())
        {
            switch (gPlayContext.mods[PLAYER_SLOT_TARGET].gauge)
            {
            case eModGauge::NORMAL:
            case eModGauge::EASY:
            case eModGauge::ASSISTEASY:
                gPlayContext.initialHealth[PLAYER_SLOT_TARGET] = 0.2;
                gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, 20);
                break;

            case eModGauge::HARD:
            case eModGauge::DEATH:
            // case eModGauge::PATTACK:
            // case eModGauge::GATTACK:
            case eModGauge::EXHARD:
                gPlayContext.initialHealth[PLAYER_SLOT_TARGET] = 1.0;
                gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, 100);
                break;

            default: break;
            }
        }

        if (gPlayContext.replayMybest)
        {
            switch (gPlayContext.replayMybest->gaugeType)
            {
            case eModGauge::NORMAL:
            case eModGauge::EASY:
            case eModGauge::ASSISTEASY:
                gPlayContext.initialHealth[PLAYER_SLOT_MYBEST] = 0.2;
                break;

            case eModGauge::HARD:
            case eModGauge::DEATH:
                // case eModGauge::PATTACK:
                // case eModGauge::GATTACK:
            case eModGauge::EXHARD:
                gPlayContext.initialHealth[PLAYER_SLOT_MYBEST] = 1.0;
                break;

            default: break;
            }
        }
    }
    else
    {
        gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, (int)gPlayContext.initialHealth[0]);

        if (isPlaymodeBattle())
        {
            gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, (int)gPlayContext.initialHealth[1]);
        }
    }
    gNumbers.flush();
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::loadChart()
{
    // always reload unstable resources
    if (!gChartContext.chartObj->resourceStable)
    {
        gChartContext.isSampleLoaded = false;
        gChartContext.isBgaLoaded = false;
    }

    // load samples
    if (!gChartContext.isSampleLoaded && !sceneEnding)
    {
        auto dtor = std::async(std::launch::async, [&]() {
            SetDebugThreadName("Chart sound sample loading thread");
            SoundMgr::freeNoteSamples();

            auto _pChart = gChartContext.chartObj;
            auto chartDir = gChartContext.chartObj->getDirectory();
            LOG_DEBUG << "[Play] Load files from " << chartDir.c_str();
            for (const auto& it : _pChart->wavFiles)
            {
				if (sceneEnding) break;
                if (it.empty()) continue;
                ++_wavToLoad;
            }
            if (_wavToLoad == 0)
            {
                _wavLoaded = 1;
                gChartContext.isSampleLoaded = true;
                return;
            }
            for (size_t i = 0; i < _pChart->wavFiles.size(); ++i)
            {
				if (sceneEnding) break;
                const auto& wav = _pChart->wavFiles[i];
                if (wav.empty()) continue;
				Path pWav = fs::u8path(wav);
				if (pWav.is_absolute())
					SoundMgr::loadNoteSample(pWav, i);
				else
					SoundMgr::loadNoteSample((chartDir / pWav), i);
                ++_wavLoaded;
            }
            gChartContext.isSampleLoaded = true;
        });
    }
    else
    {
        gChartContext.isSampleLoaded = true;
    }

    // load bga
    if (gSwitches.get(eSwitch::SYSTEM_BGA) && !sceneEnding)
    {
        if (!gChartContext.isBgaLoaded)
        {
            auto dtor = std::async(std::launch::async, [&]() {
                SetDebugThreadName("Chart BGA loading thread");
                gPlayContext.bgaTexture->clear();

                auto _pChart = gChartContext.chartObj;
                auto chartDir = gChartContext.chartObj->getDirectory();
                for (const auto& it : _pChart->bgaFiles)
                {
                    if (sceneEnding) return;
                    if (it.empty()) continue;
                    ++_bmpToLoad;
                }
                if (_bmpToLoad == 0)
                {
                    _bmpLoaded = 1;
                    gChartContext.isBgaLoaded = true;
                    return;
                }

                std::list<std::pair<size_t, Path>> mapBgaFiles;
                auto loadBgaFiles = [&]
                {
                    for (auto& [i, pBmp] : mapBgaFiles)
                    {
                        if (pBmp.is_absolute())
                            gPlayContext.bgaTexture->addBmp(i, pBmp);
                        else
                            gPlayContext.bgaTexture->addBmp(i, chartDir / pBmp);
                        ++_bmpLoaded;
                    }
                };
                for (size_t i = 0; i < _pChart->bgaFiles.size(); ++i)
                {
                    if (sceneEnding) return;
                    const auto& bmp = _pChart->bgaFiles[i];
                    if (bmp.empty()) continue;

                    mapBgaFiles.emplace_back(i, fs::u8path(bmp));

                    if (mapBgaFiles.size() >= 8)
                    {
                        pushAndWaitMainThreadTask<void>(loadBgaFiles);
                        mapBgaFiles.clear();
                    }
                }
                loadBgaFiles();
                mapBgaFiles.clear();

                if (_bmpLoaded > 0)
                {
                    gPlayContext.bgaTexture->setLoaded();
                }
                gPlayContext.bgaTexture->setSlotFromBMS(*std::reinterpret_pointer_cast<ChartObjectBMS>(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]));
                gChartContext.isBgaLoaded = true;
                });
        }
        else
        {
            // set playback speed on each play
            gPlayContext.bgaTexture->setVideoSpeed();
        }
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER] != nullptr)
    {
        auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2);
        _input.register_p("JUDGE_PRESS_1", fp);
        auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2);
        _input.register_h("JUDGE_HOLD_1", fh);
        auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2);
        _input.register_r("JUDGE_RELEASE_1", fr);
        auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_PLAYER], _1, _2, _3);
        _input.register_a("JUDGE_AXIS_1", fa);
    }
    else
    {
        LOG_ERROR << "[Play] Ruleset of 1P not initialized!";
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr)
    {
        auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2);
        _input.register_p("JUDGE_PRESS_2", fp);
        auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2);
        _input.register_h("JUDGE_HOLD_2", fh);
        auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2);
        _input.register_r("JUDGE_RELEASE_2", fr);
        auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_TARGET], _1, _2, _3);
        _input.register_a("JUDGE_AXIS_2", fa);
    }
    else if (!gPlayContext.isAuto)
    {
        LOG_ERROR << "[Play] Ruleset of 2P not initialized!";
    }
}

void ScenePlay::removeInputJudgeCallback()
{
    for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
    {
        _input.unregister_p("JUDGE_PRESS_1");
        _input.unregister_h("JUDGE_HOLD_1");
        _input.unregister_r("JUDGE_RELEASE_1");
        _input.unregister_a("JUDGE_AXIS_1");
        _input.unregister_p("JUDGE_PRESS_2");
        _input.unregister_h("JUDGE_HOLD_2");
        _input.unregister_r("JUDGE_RELEASE_2");
        _input.unregister_a("JUDGE_AXIS_2");
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::_updateAsync()
{
    if (gNextScene != eScene::PLAY) return;

    if (gAppIsExiting)
    {
        gNextScene = eScene::EXIT_TRANS;
    }

    Time t;

    // update lanecover / hispeed change
    int lcThreshold = getRate() / 200;  // lanecover, +200 per second
    int hsThreshold = getRate() / 25;   // hispeed, +25 per second
    if (true)
    {
        // TODO SUD+ or HID+/LIFT?
        bool lcHasChanged = (_lanecoverAdd[PLAYER_SLOT_PLAYER] != 0);
        int lc = gNumbers.get(eNumber::LANECOVER_TOP_1P);
        int lcOld = lc;
        while (lc < 1000 && _lanecoverAdd[PLAYER_SLOT_PLAYER] >= lcThreshold)
        {
            _lanecoverAdd[PLAYER_SLOT_PLAYER] -= lcThreshold;
            lc += 1;
        }
        while (lc > 0 && _lanecoverAdd[PLAYER_SLOT_PLAYER] <= -lcThreshold)
        {
            _lanecoverAdd[PLAYER_SLOT_PLAYER] += lcThreshold;
            lc -= 1;
        }
        if (lc <= 0)
        {
            lc = 0;
            if (_lanecoverAdd[PLAYER_SLOT_PLAYER] < 0)
                _lanecoverAdd[PLAYER_SLOT_PLAYER] = 0;
        }
        else if (lc >= 1000)
        {
            lc = 1000;
            if (_lanecoverAdd[PLAYER_SLOT_PLAYER] > 0)
                _lanecoverAdd[PLAYER_SLOT_PLAYER] = 0;
        }

        double hs = gPlayContext.Hispeed;
        double hsOld = hs;
        while (hs < 10.0 && _hispeedAdd[PLAYER_SLOT_PLAYER] >= hsThreshold)
        {
            _hispeedAdd[PLAYER_SLOT_PLAYER] -= hsThreshold;
            hs += 0.01;
        }
        while (hs > 0.25 && _hispeedAdd[PLAYER_SLOT_PLAYER] <= -hsThreshold)
        {
            _hispeedAdd[PLAYER_SLOT_PLAYER] += hsThreshold;
            hs -= 0.01;
        }
        if (hs <= 0.01)
        {
            hs = 0.01;
            if (_hispeedAdd[PLAYER_SLOT_PLAYER] < 0)
                _hispeedAdd[PLAYER_SLOT_PLAYER] = 0;
        }
        else if (hs >= 10.0)
        {
            hs = 10.0;
            if (_hispeedAdd[PLAYER_SLOT_PLAYER] > 0)
                _hispeedAdd[PLAYER_SLOT_PLAYER] = 0;
        }

        if (lcHasChanged)
        {
            gNumbers.queue(eNumber::LANECOVER_TOP_1P, lc);
            gNumbers.queue(eNumber::LANECOVER100_1P, lc / 10);
            gSliders.queue(eSlider::SUD_1P, lc / 1000.0);

            if (_lockspeedEnabled[PLAYER_SLOT_PLAYER])
            {
                gPlayContext.HispeedGradientStart = t;
                gPlayContext.HispeedGradientFrom = gPlayContext.HispeedGradientNow;

                double bpm = gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix == eModHs::CONSTANT ?
                    150.0 : gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM();
                int lcTop = lc;
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
                double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                hs = std::min(_lockspeedValue[PLAYER_SLOT_PLAYER] / bpm * visible, 10.0);
                gPlayContext.Hispeed = hs;
                gNumbers.queue(eNumber::HS_1P, (int)std::round(hs * 100));
            }
        }
        else if (hs != hsOld)
        {
            gPlayContext.HispeedGradientStart = t;
            gPlayContext.HispeedGradientFrom = gPlayContext.HispeedGradientNow;

            gPlayContext.Hispeed = hs;
            gNumbers.queue(eNumber::HS_1P, (int)std::round(hs * 100));

            if (_lockspeedEnabled[PLAYER_SLOT_PLAYER] && _isHoldingSelect[PLAYER_SLOT_PLAYER])
            {
                double bpm = gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix == eModHs::CONSTANT ?
                    150.0 : gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM();
                int lcTop = lc;
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
                double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_PLAYER] = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / visible);

                double den = hs * bpm;
                _lockspeedGreenNumber[PLAYER_SLOT_PLAYER] = den != 0.0 ? int(std::round(visible * 120.0 * 1200 / hs / bpm)) : 0;
            }
        }

    }
    if (isPlaymodeBattle())
    {
        // TODO SUD+ or HID+/LIFT?
        bool lcHasChanged = (_lanecoverAdd[PLAYER_SLOT_TARGET] != 0);
        int lc = gNumbers.get(eNumber::LANECOVER_TOP_2P);
        int lcOld = lc;
        while (lc < 1000 && _lanecoverAdd[PLAYER_SLOT_TARGET] >= lcThreshold)
        {
            _lanecoverAdd[PLAYER_SLOT_TARGET] -= lcThreshold;
            lc += 1;
        }
        while (lc > 0 && _lanecoverAdd[PLAYER_SLOT_TARGET] <= -lcThreshold)
        {
            _lanecoverAdd[PLAYER_SLOT_TARGET] += lcThreshold;
            lc -= 1;
        }
        if (lc <= 0)
        {
            lc = 0;
            if (_lanecoverAdd[PLAYER_SLOT_TARGET] < 0)
                _lanecoverAdd[PLAYER_SLOT_TARGET] = 0;
        }
        else if (lc >= 1000)
        {
            lc = 1000;
            if (_lanecoverAdd[PLAYER_SLOT_TARGET] > 0)
                _lanecoverAdd[PLAYER_SLOT_TARGET] = 0;
        }

        double hs = gPlayContext.battle2PHispeed;
        double hsOld = hs;
        while (hs < 10.0 && _hispeedAdd[PLAYER_SLOT_TARGET] >= hsThreshold)
        {
            _hispeedAdd[PLAYER_SLOT_TARGET] -= hsThreshold;
            hs += 0.01;
        }
        while (hs > 0.25 && _hispeedAdd[PLAYER_SLOT_TARGET] <= -hsThreshold)
        {
            _hispeedAdd[PLAYER_SLOT_TARGET] += hsThreshold;
            hs -= 0.01;
        }
        if (hs <= 0.01)
        {
            hs = 0.01;
            if (_hispeedAdd[PLAYER_SLOT_TARGET] < 0)
                _hispeedAdd[PLAYER_SLOT_TARGET] = 0;
        }
        else if (hs >= 10.0)
        {
            hs = 10.0;
            if (_hispeedAdd[PLAYER_SLOT_TARGET] > 0)
                _hispeedAdd[PLAYER_SLOT_TARGET] = 0;
        }

        if (lcHasChanged)
        {
            gNumbers.queue(eNumber::LANECOVER_TOP_2P, lc);
            gNumbers.queue(eNumber::LANECOVER100_2P, lc / 10);
            gSliders.queue(eSlider::SUD_2P, lc / 1000.0);

            if (_lockspeedEnabled[PLAYER_SLOT_TARGET])
            {
                gPlayContext.battle2PHispeedGradientStart = t;
                gPlayContext.battle2PHispeedGradientFrom = gPlayContext.battle2PHispeedGradientNow;

                double bpm = gPlayContext.mods[PLAYER_SLOT_TARGET].hispeedFix == eModHs::CONSTANT ?
                    150.0 : gPlayContext.chartObj[PLAYER_SLOT_TARGET]->getCurrentBPM();
                int lcTop = lc;
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
                double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                hs = std::min(_lockspeedValue[PLAYER_SLOT_TARGET] / bpm * visible, 10.0);
                gPlayContext.battle2PHispeed = hs;
                gNumbers.queue(eNumber::HS_2P, (int)std::round(hs * 100));
            }
        }
        else if (hs != hsOld)
        {
            gPlayContext.battle2PHispeedGradientStart = t;
            gPlayContext.battle2PHispeedGradientFrom = gPlayContext.battle2PHispeedGradientNow;

            gPlayContext.battle2PHispeed = hs;
            gNumbers.queue(eNumber::HS_2P, (int)std::round(hs * 100));

            if (_lockspeedEnabled[PLAYER_SLOT_TARGET] && _isHoldingSelect[PLAYER_SLOT_PLAYER])
            {
                double bpm = gPlayContext.mods[PLAYER_SLOT_TARGET].hispeedFix == eModHs::CONSTANT ?
                    150.0 : gPlayContext.chartObj[PLAYER_SLOT_TARGET]->getCurrentBPM();
                int lcTop = lc;
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
                double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_TARGET] = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / visible);

                double den = hs * bpm;
                _lockspeedGreenNumber[PLAYER_SLOT_PLAYER] = den != 0.0 ? int(std::round(visible * 120.0 * 1200 / hs / bpm)) : 0;
            }
        }
    }
    if (!_lanecoverEnabled[PLAYER_SLOT_PLAYER])
    {
        gSliders.queue(eSlider::SUD_1P, 0.0);
    }
    if (isPlaymodeDP() || !_lanecoverEnabled[PLAYER_SLOT_TARGET])
    {
        gSliders.queue(eSlider::SUD_2P, 0.0);
    }
    gSliders.flush();
    gNumbers.flush();

    // HS gradient
    const long long HS_GRADIENT_LENGTH_MS = 200;
    if (gPlayContext.HispeedGradientStart != TIMER_NEVER)
    {
        Time hsGradient1P = t - gPlayContext.HispeedGradientStart;
        if (hsGradient1P.norm() < HS_GRADIENT_LENGTH_MS)
        {
            double prog = std::sin((hsGradient1P.norm() / (double)HS_GRADIENT_LENGTH_MS) * 1.57079632679);
            gPlayContext.HispeedGradientNow = gPlayContext.HispeedGradientFrom + prog * (gPlayContext.Hispeed - gPlayContext.HispeedGradientFrom);
        }
        else
        {
            gPlayContext.HispeedGradientNow = gPlayContext.Hispeed;
            gPlayContext.HispeedGradientStart = TIMER_NEVER;
        }
    }
    if (gPlayContext.battle2PHispeedGradientStart != TIMER_NEVER)
    {
        Time hsGradient1P = t - gPlayContext.battle2PHispeedGradientStart;
        if (hsGradient1P.norm() < HS_GRADIENT_LENGTH_MS)
        {
            double prog = std::sin((hsGradient1P.norm() / (double)HS_GRADIENT_LENGTH_MS * 1.57079632679));
            gPlayContext.battle2PHispeedGradientNow = gPlayContext.battle2PHispeedGradientFrom + prog * (gPlayContext.battle2PHispeed - gPlayContext.battle2PHispeedGradientFrom);
        }
        else
        {
            gPlayContext.battle2PHispeedGradientNow = gPlayContext.battle2PHispeed;
            gPlayContext.battle2PHispeedGradientStart = TIMER_NEVER;
        }
    }

    // update green number
    // 120BPM with 1.0x HS is 2000ms (500ms/beat, green number 1200)
    if (_skin->info.noteLaneHeight1P != 0)
    {
        double bpm, minBPM, maxBPM;
        if (gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix != eModHs::CONSTANT)
        {
            bpm = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM();
            minBPM = gChartContext.minBPM;
            maxBPM = gChartContext.maxBPM;
        }
        else
        {
            bpm = minBPM = maxBPM = 150.0;
        }
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);

        double visible = 1.0;
        if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
        visible -= std::clamp(lcTop + lcBottom, 0, 1000) / 1000.0;
        double den;
        den = gPlayContext.Hispeed * bpm;
        gNumbers.queue(eNumber::GREEN_NUMBER_1P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
        den = gPlayContext.Hispeed * maxBPM;
        gNumbers.queue(eNumber::GREEN_NUMBER_MAXBPM_1P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
        den = gPlayContext.Hispeed * minBPM;
        gNumbers.queue(eNumber::GREEN_NUMBER_MINBPM_1P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
    }
    if (_skin->info.noteLaneHeight2P != 0 && gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        double bpm, minBPM, maxBPM;
        if (gPlayContext.mods[PLAYER_SLOT_TARGET].hispeedFix != eModHs::CONSTANT)
        {
            bpm = gPlayContext.chartObj[PLAYER_SLOT_TARGET]->getCurrentBPM();
            minBPM = gChartContext.minBPM;
            maxBPM = gChartContext.maxBPM;
        }
        else
        {
            bpm = minBPM = maxBPM = 150.0;
        }
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);

        double visible = 1.0;
        if (_lanecoverEnabled[PLAYER_SLOT_TARGET])
            visible -= std::clamp(lcTop + lcBottom, 0, 1000) / 1000.0;
        double den;
        den = gPlayContext.battle2PHispeed * bpm;
        gNumbers.queue(eNumber::GREEN_NUMBER_2P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
        den = gPlayContext.battle2PHispeed * maxBPM;
        gNumbers.queue(eNumber::GREEN_NUMBER_MAXBPM_2P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
        den = gPlayContext.battle2PHispeed * minBPM;
        gNumbers.queue(eNumber::GREEN_NUMBER_MINBPM_2P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
    }

    gNumbers.queue(eNumber::SCENE_UPDATE_FPS, _looper.getRate());
    gNumbers.flush();

    // setting speed / lanecover (if display white number / green number)
    gSwitches.queue(eSwitch::P1_SETTING_SPEED, false);
    gSwitches.queue(eSwitch::P2_SETTING_SPEED, false);
    if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
    {
        if (_isHoldingStart[PLAYER_SLOT_PLAYER] || _isHoldingSelect[PLAYER_SLOT_PLAYER])
        {
            gSwitches.queue(eSwitch::P1_SETTING_SPEED, true);
        }
        if (_isHoldingStart[PLAYER_SLOT_TARGET] || _isHoldingSelect[PLAYER_SLOT_TARGET])
        {
            gSwitches.queue(!isPlaymodeDP() ? eSwitch::P1_SETTING_SPEED : eSwitch::P2_SETTING_SPEED, true);
        }
    }
    if (_lanecoverEnabled[PLAYER_SLOT_TARGET])
    {
        if (_isHoldingStart[PLAYER_SLOT_TARGET] || _isHoldingSelect[PLAYER_SLOT_TARGET])
        {
            gSwitches.queue(eSwitch::P2_SETTING_SPEED, true);
        }
    }
    gSwitches.flush();

    // show greennumber on top-left for unsupported skins
    if (!_skin->isSupportGreenNumber)
    {
        std::stringstream ss;
        if (_isHoldingStart[PLAYER_SLOT_PLAYER] || _isHoldingSelect[PLAYER_SLOT_PLAYER] || _isHoldingStart[PLAYER_SLOT_TARGET] || _isHoldingSelect[PLAYER_SLOT_TARGET])
        {
            ss << "G(1P): " << (_lockspeedEnabled[PLAYER_SLOT_PLAYER] ? "FIX " : "") << gNumbers.get(eNumber::GREEN_NUMBER_1P) <<
                " (" << gNumbers.get(eNumber::GREEN_NUMBER_MINBPM_1P) << " - " << gNumbers.get(eNumber::GREEN_NUMBER_MAXBPM_1P) << ")";

            if (isPlaymodeBattle())
            {
                ss << " | G(2P): " << (_lockspeedEnabled[PLAYER_SLOT_TARGET] ? "FIX " : "") << gNumbers.get(eNumber::GREEN_NUMBER_2P) <<
                    " (" << gNumbers.get(eNumber::GREEN_NUMBER_MINBPM_2P) << " - " << gNumbers.get(eNumber::GREEN_NUMBER_MAXBPM_2P) << ")";
            }
        }
        gTexts.set(eText::_OVERLAY_TOPLEFT, ss.str());
    }


    auto Scratch = [&](const Time& t, Input::Pad up, Input::Pad dn, double& val, int slot)
    {
        double scratchThreshold = 0.001;
        double scratchRewind = 0.0001;

        std::array<size_t, 4> keySampleIdxBufScratch;
        size_t sampleCount = 0;
        bool playSample = false;

        if (val > scratchThreshold)
        {
            // scratch down
            val -= scratchThreshold;

            if (_scratchDir[slot] != AxisDir::AXIS_DOWN)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    gTimers.set(eTimer::S1_DOWN, t.norm());
                    gTimers.set(eTimer::S1_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S1_DOWN, true);
                    playSample = true;
                }
                else
                {
                    gTimers.set(eTimer::S2_DOWN, t.norm());
                    gTimers.set(eTimer::S2_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S2_DOWN, true);
                    playSample = true;
                }
            }

            _scratchLastUpdate[slot] = t;
            _scratchDir[slot] = AxisDir::AXIS_DOWN;

            // push replay command
            if (gChartContext.started && gPlayContext.replayNew)
            {
                long long ms = t.norm() - gTimers.get(eTimer::PLAY_START);
                ReplayChart::Commands cmd;
                cmd.ms = ms;
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    cmd.type = ReplayChart::Commands::Type::S1A_PLUS;
                    replayKeyPressing[Input::Pad::S1A] = true;
                }
                else
                {
                    cmd.type = ReplayChart::Commands::Type::S2A_PLUS;
                    replayKeyPressing[Input::Pad::S2A] = true;
                }
                gPlayContext.replayNew->commands.push_back(cmd);
            }
        }
        else if (val < -scratchThreshold)
        {
            // scratch up
            val += scratchThreshold;

            if (_scratchDir[slot] != AxisDir::AXIS_UP)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    gTimers.set(eTimer::S1_DOWN, t.norm());
                    gTimers.set(eTimer::S1_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S1_DOWN, true);
                    playSample = true;
                }
                else
                {
                    gTimers.set(eTimer::S2_DOWN, t.norm());
                    gTimers.set(eTimer::S2_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S2_DOWN, true);
                    playSample = true;
                }
            }
            
            _scratchLastUpdate[slot] = t;
            _scratchDir[slot] = AxisDir::AXIS_UP;

            // push replay command
            if (gChartContext.started && gPlayContext.replayNew)
            {
                long long ms = t.norm() - gTimers.get(eTimer::PLAY_START);
                ReplayChart::Commands cmd;
                cmd.ms = ms;
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    cmd.type = ReplayChart::Commands::Type::S1A_MINUS;
                    replayKeyPressing[Input::Pad::S1A] = true;
                }
                else
                {
                    cmd.type = ReplayChart::Commands::Type::S2A_MINUS;
                    replayKeyPressing[Input::Pad::S2A] = true;
                }
                gPlayContext.replayNew->commands.push_back(cmd);
            }
        }
        else
        {
            if (slot == PLAYER_SLOT_PLAYER)
            {
                if (replayKeyPressing[Input::Pad::S1A])
                {
                    long long ms = t.norm() - gTimers.get(eTimer::PLAY_START);
                    ReplayChart::Commands cmd;
                    cmd.ms = ms;
                    cmd.type = ReplayChart::Commands::Type::S1A_STOP;
                    replayKeyPressing[Input::Pad::S1A] = false;
                    gPlayContext.replayNew->commands.push_back(cmd);
                }
            }
            else
            {
                if (replayKeyPressing[Input::Pad::S2A])
                {
                    long long ms = t.norm() - gTimers.get(eTimer::PLAY_START);
                    ReplayChart::Commands cmd;
                    cmd.ms = ms;
                    cmd.type = ReplayChart::Commands::Type::S2A_STOP;
                    replayKeyPressing[Input::Pad::S2A] = false;
                    gPlayContext.replayNew->commands.push_back(cmd);
                }
            }
        }

        if (playSample)
        {
            if (slot == PLAYER_SLOT_PLAYER && !gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFailed())
            {
                if (_scratchDir[slot] == AxisDir::AXIS_UP && _currentKeySample[Input::S1L])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[Input::S1L];
                if (_scratchDir[slot] == AxisDir::AXIS_DOWN && _currentKeySample[Input::S1R])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[Input::S1R];

                SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, keySampleIdxBufScratch.data());
            }
            if (slot != PLAYER_SLOT_PLAYER && (isPlaymodeDP() || (isPlaymodeBattle() && !gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed())))
            {
                if (_scratchDir[slot] == AxisDir::AXIS_UP && _currentKeySample[Input::S2L])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[Input::S2L];
                if (_scratchDir[slot] == AxisDir::AXIS_DOWN && _currentKeySample[Input::S2R])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[Input::S2R];

                SoundMgr::playNoteSample((!isPlaymodeBattle() ? SoundChannelType::KEY_LEFT : SoundChannelType::KEY_RIGHT), sampleCount, keySampleIdxBufScratch.data());
            }
        }

        if (val > scratchRewind)
            val -= scratchRewind;
        else if (val < -scratchRewind)
            val += scratchRewind;
        else
            val = 0.;

        if ((t - _scratchLastUpdate[slot]).norm() > 133)
        {
            // release
            if (_scratchDir[slot] != AxisDir::AXIS_NONE)
            {
                if (slot == PLAYER_SLOT_PLAYER)
                {
                    gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
                    gTimers.set(eTimer::S1_UP, t.norm());
                    gSwitches.set(eSwitch::S1_DOWN, false);
                }
                else
                {
                    gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
                    gTimers.set(eTimer::S2_UP, t.norm());
                    gSwitches.set(eSwitch::S2_DOWN, false);
                }
            }

            _scratchDir[slot] = AxisDir::AXIS_NONE;
            _scratchLastUpdate[slot] = TIMER_NEVER;
        }
    };
    Scratch(t, Input::S1L, Input::S1R, _scratchAccumulator[PLAYER_SLOT_PLAYER], PLAYER_SLOT_PLAYER);
    Scratch(t, Input::S2L, Input::S2R, _scratchAccumulator[PLAYER_SLOT_TARGET], PLAYER_SLOT_TARGET);

    switch (_state)
    {
    case ePlayState::PREPARE:
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_PREPARE);
        updatePrepare();
        break;
    case ePlayState::LOADING:
        updateLoading();
        break;
    case ePlayState::LOAD_END:
        updateLoadEnd();
        break;
    case ePlayState::PLAYING:
        updatePlaying();
        break;
    case ePlayState::FADEOUT:
        updateFadeout();
        break;
    case ePlayState::FAILED:
        updateFailed();
        break;
    }
}

void ScenePlay::updatePrepare()
{
	auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    if (rt.norm() > _skin->info.timeIntro)
    {
        gTimers.set(eTimer::_LOAD_START, t.norm());
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_LOADING);
		_loadChartFuture = std::async(std::launch::async, std::bind(&ScenePlay::loadChart, this));
        _state = ePlayState::LOADING;
        LOG_DEBUG << "[Play] State changed to LOADING";
    }
}

void ScenePlay::updateLoading()
{
	auto t = Time();
    auto rt = t - gTimers.get(eTimer::_LOAD_START);

    gNumbers.set(eNumber::PLAY_LOAD_PROGRESS_SYS, int(_chartLoaded * 50 + _rulesetLoaded * 50) / 100);
    gNumbers.set(eNumber::PLAY_LOAD_PROGRESS_WAV, int(getWavLoadProgress() * 100) / 100);
    gNumbers.set(eNumber::PLAY_LOAD_PROGRESS_BGA, int(getBgaLoadProgress() * 100) / 100);
    gNumbers.set(eNumber::PLAY_LOAD_PROGRESS_PERCENT, int
        (int(_chartLoaded) * 50 + int(_rulesetLoaded) * 50 + 
        getWavLoadProgress() * 100 + getBgaLoadProgress() * 100) / 300);

    gBargraphs.set(eBargraph::MUSIC_LOAD_PROGRESS_SYS, int(_chartLoaded) * 0.5 + int(_rulesetLoaded) * 0.5);
    gBargraphs.set(eBargraph::MUSIC_LOAD_PROGRESS_WAV, getWavLoadProgress());
    gBargraphs.set(eBargraph::MUSIC_LOAD_PROGRESS_BGA, getBgaLoadProgress());
    gBargraphs.set(eBargraph::MUSIC_LOAD_PROGRESS, int
        (int(_chartLoaded) * 0.5 + int(_rulesetLoaded) * 0.5 +
        getWavLoadProgress() + getBgaLoadProgress()) / 3.0);

    if (_chartLoaded && 
        _rulesetLoaded &&
        gChartContext.isSampleLoaded && 
        (!gSwitches.get(eSwitch::SYSTEM_BGA) || gChartContext.isBgaLoaded) &&
        (t - _readyTime) > 1000 &&
		rt > _skin->info.timeMinimumLoad)
    {
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_READY);
		if (gPlayContext.bgaTexture) gPlayContext.bgaTexture->reset();
        gTimers.set(eTimer::PLAY_READY, t.norm());
        _state = ePlayState::LOAD_END;
        LOG_DEBUG << "[Play] State changed to READY";
    }
}

void ScenePlay::updateLoadEnd()
{
	auto t = Time();
    auto rt = t - gTimers.get(eTimer::PLAY_READY);
    spinTurntable(false);
    if (rt > _skin->info.timeGetReady)
    {
        changeKeySampleMapping(0);
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_PLAYING);
        gTimers.set(eTimer::PLAY_START, t.norm());
        setInputJudgeCallback();
		gChartContext.started = true;
        _state = ePlayState::PLAYING;
        LOG_DEBUG << "[Play] State changed to PLAY_START";
    }
}

void ScenePlay::updatePlaying()
{
	auto t = Time();
	auto rt = t - gTimers.get(eTimer::PLAY_START);
    gTimers.queue(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);

    gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
    gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->update(t);

    if (gPlayContext.isReplay)
    {
        InputMask prev = replayKeyPressing;
        while (itReplayCommand != gPlayContext.replay->commands.end() && rt.norm() >= itReplayCommand->ms)
        {
            switch (itReplayCommand->type)
            {
            case ReplayChart::Commands::Type::S1L_DOWN: replayKeyPressing[Input::Pad::S1L] = true; break;
            case ReplayChart::Commands::Type::S1R_DOWN: replayKeyPressing[Input::Pad::S1R] = true; break;
            case ReplayChart::Commands::Type::K11_DOWN: replayKeyPressing[Input::Pad::K11] = true; break;
            case ReplayChart::Commands::Type::K12_DOWN: replayKeyPressing[Input::Pad::K12] = true; break;
            case ReplayChart::Commands::Type::K13_DOWN: replayKeyPressing[Input::Pad::K13] = true; break;
            case ReplayChart::Commands::Type::K14_DOWN: replayKeyPressing[Input::Pad::K14] = true; break;
            case ReplayChart::Commands::Type::K15_DOWN: replayKeyPressing[Input::Pad::K15] = true; break;
            case ReplayChart::Commands::Type::K16_DOWN: replayKeyPressing[Input::Pad::K16] = true; break;
            case ReplayChart::Commands::Type::K17_DOWN: replayKeyPressing[Input::Pad::K17] = true; break;
            case ReplayChart::Commands::Type::K18_DOWN: replayKeyPressing[Input::Pad::K18] = true; break;
            case ReplayChart::Commands::Type::K19_DOWN: replayKeyPressing[Input::Pad::K19] = true; break;
            case ReplayChart::Commands::Type::K1START_DOWN: replayKeyPressing[Input::Pad::K1START] = true; break;
            case ReplayChart::Commands::Type::K1SELECT_DOWN: replayKeyPressing[Input::Pad::K1SELECT] = true; break;
            case ReplayChart::Commands::Type::S2L_DOWN: replayKeyPressing[Input::Pad::S2L] = true; break;
            case ReplayChart::Commands::Type::S2R_DOWN: replayKeyPressing[Input::Pad::S2R] = true; break;
            case ReplayChart::Commands::Type::K21_DOWN: replayKeyPressing[Input::Pad::K21] = true; break;
            case ReplayChart::Commands::Type::K22_DOWN: replayKeyPressing[Input::Pad::K22] = true; break;
            case ReplayChart::Commands::Type::K23_DOWN: replayKeyPressing[Input::Pad::K23] = true; break;
            case ReplayChart::Commands::Type::K24_DOWN: replayKeyPressing[Input::Pad::K24] = true; break;
            case ReplayChart::Commands::Type::K25_DOWN: replayKeyPressing[Input::Pad::K25] = true; break;
            case ReplayChart::Commands::Type::K26_DOWN: replayKeyPressing[Input::Pad::K26] = true; break;
            case ReplayChart::Commands::Type::K27_DOWN: replayKeyPressing[Input::Pad::K27] = true; break;
            case ReplayChart::Commands::Type::K28_DOWN: replayKeyPressing[Input::Pad::K28] = true; break;
            case ReplayChart::Commands::Type::K29_DOWN: replayKeyPressing[Input::Pad::K29] = true; break;
            case ReplayChart::Commands::Type::K2START_DOWN: replayKeyPressing[Input::Pad::K2START] = true; break;
            case ReplayChart::Commands::Type::K2SELECT_DOWN: replayKeyPressing[Input::Pad::K2SELECT] = true; break;
            case ReplayChart::Commands::Type::S1L_UP: replayKeyPressing[Input::Pad::S1L] = false; break;
            case ReplayChart::Commands::Type::S1R_UP: replayKeyPressing[Input::Pad::S1R] = false; break;
            case ReplayChart::Commands::Type::K11_UP: replayKeyPressing[Input::Pad::K11] = false; break;
            case ReplayChart::Commands::Type::K12_UP: replayKeyPressing[Input::Pad::K12] = false; break;
            case ReplayChart::Commands::Type::K13_UP: replayKeyPressing[Input::Pad::K13] = false; break;
            case ReplayChart::Commands::Type::K14_UP: replayKeyPressing[Input::Pad::K14] = false; break;
            case ReplayChart::Commands::Type::K15_UP: replayKeyPressing[Input::Pad::K15] = false; break;
            case ReplayChart::Commands::Type::K16_UP: replayKeyPressing[Input::Pad::K16] = false; break;
            case ReplayChart::Commands::Type::K17_UP: replayKeyPressing[Input::Pad::K17] = false; break;
            case ReplayChart::Commands::Type::K18_UP: replayKeyPressing[Input::Pad::K18] = false; break;
            case ReplayChart::Commands::Type::K19_UP: replayKeyPressing[Input::Pad::K19] = false; break;
            case ReplayChart::Commands::Type::K1START_UP: replayKeyPressing[Input::Pad::K1START] = false; break;
            case ReplayChart::Commands::Type::K1SELECT_UP: replayKeyPressing[Input::Pad::K1SELECT] = false; break;
            case ReplayChart::Commands::Type::S2L_UP: replayKeyPressing[Input::Pad::S2L] = false; break;
            case ReplayChart::Commands::Type::S2R_UP: replayKeyPressing[Input::Pad::S2R] = false; break;
            case ReplayChart::Commands::Type::K21_UP: replayKeyPressing[Input::Pad::K21] = false; break;
            case ReplayChart::Commands::Type::K22_UP: replayKeyPressing[Input::Pad::K22] = false; break;
            case ReplayChart::Commands::Type::K23_UP: replayKeyPressing[Input::Pad::K23] = false; break;
            case ReplayChart::Commands::Type::K24_UP: replayKeyPressing[Input::Pad::K24] = false; break;
            case ReplayChart::Commands::Type::K25_UP: replayKeyPressing[Input::Pad::K25] = false; break;
            case ReplayChart::Commands::Type::K26_UP: replayKeyPressing[Input::Pad::K26] = false; break;
            case ReplayChart::Commands::Type::K27_UP: replayKeyPressing[Input::Pad::K27] = false; break;
            case ReplayChart::Commands::Type::K28_UP: replayKeyPressing[Input::Pad::K28] = false; break;
            case ReplayChart::Commands::Type::K29_UP: replayKeyPressing[Input::Pad::K29] = false; break;
            case ReplayChart::Commands::Type::K2START_UP: replayKeyPressing[Input::Pad::K2START] = false; break;
            case ReplayChart::Commands::Type::K2SELECT_UP: replayKeyPressing[Input::Pad::K2SELECT] = false; break;
            case ReplayChart::Commands::Type::S1A_PLUS:  _scratchAccumulator[PLAYER_SLOT_PLAYER] = 0.0015; break;
            case ReplayChart::Commands::Type::S1A_MINUS: _scratchAccumulator[PLAYER_SLOT_PLAYER] = -0.0015; break;
            case ReplayChart::Commands::Type::S1A_STOP:  _scratchAccumulator[PLAYER_SLOT_PLAYER] = 0; break;
            case ReplayChart::Commands::Type::S2A_PLUS:  _scratchAccumulator[PLAYER_SLOT_TARGET] = 0.0015; break;
            case ReplayChart::Commands::Type::S2A_MINUS: _scratchAccumulator[PLAYER_SLOT_TARGET] = -0.0015; break;
            case ReplayChart::Commands::Type::S2A_STOP:  _scratchAccumulator[PLAYER_SLOT_TARGET] = 0; break;

            // TODO
            case ReplayChart::Commands::Type::HISPEED: break;
            case ReplayChart::Commands::Type::LANECOVER_TOP: break;
            case ReplayChart::Commands::Type::LANECOVER_BOTTOM: break;
            }
            itReplayCommand++;
        }
        InputMask pressed = replayKeyPressing & ~prev;
        InputMask released = ~replayKeyPressing & prev;
        if (pressed.any())
            inputGamePressTimer(pressed, t);
        if (released.any())
            inputGameReleaseTimer(released, t);

        if (replayKeyPressing[Input::Pad::K1START] || !isPlaymodeBattle() && replayKeyPressing[Input::Pad::K2START]) _isHoldingStart[PLAYER_SLOT_PLAYER] = true;
        if (replayKeyPressing[Input::Pad::K1SELECT] || !isPlaymodeBattle() && replayKeyPressing[Input::Pad::K2SELECT]) _isHoldingSelect[PLAYER_SLOT_PLAYER] = true;

    }

    auto dp1 = gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData();
    int miss1 = dp1.miss;
    if (_missPlayer[PLAYER_SLOT_PLAYER] != miss1)
    {
        _missPlayer[PLAYER_SLOT_PLAYER] = miss1;
        _missLastTime = t;
    }
    gNumbers.queue(eNumber::PLAY_1P_EXSCORE, dp1.score2);

    if (gPlayContext.ruleset[PLAYER_SLOT_MYBEST] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_MYBEST]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->update(t);

        auto dpb = gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->getData();

        gNumbers.set(eNumber::RESULT_MYBEST_EX, dpb.score2);
        gNumbers.queue(eNumber::RESULT_MYBEST_RATE, (int)std::floor(dpb.acc * 100.0));
        gNumbers.queue(eNumber::RESULT_MYBEST_RATE_DECIMAL2, (int)std::floor(dpb.acc * 10000.0) % 100);
    }
    if (!isPlaymodeBattle())
    {
        gNumbers.queue(eNumber::RESULT_MYBEST_DIFF, dp1.score2 - gNumbers.get(eNumber::RESULT_MYBEST_EX));
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_TARGET]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_TARGET]->update(t);

        auto targetType = gOptions.get(eOption::PLAY_TARGET_TYPE);
        
        if (targetType == Option::TARGET_MYBEST && gPlayContext.replayMybest)
        {
            assert(gPlayContext.ruleset[PLAYER_SLOT_MYBEST] != nullptr);
            auto dp2 = gPlayContext.ruleset[PLAYER_SLOT_MYBEST]->getData();

            gNumbers.queue(eNumber::PLAY_2P_EXSCORE, dp2.score2);
            gNumbers.queue(eNumber::PLAY_1P_EXSCORE_DIFF, dp1.score2 - dp2.score2);
            gNumbers.queue(eNumber::PLAY_2P_EXSCORE_DIFF, dp2.score2 - dp1.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_EX, dp2.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_DIFF, dp2.score2 - dp1.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_RATE, (int)std::floor(dp2.acc * 100.0));
            gNumbers.queue(eNumber::RESULT_TARGET_RATE_DECIMAL2, (int)std::floor(dp2.acc * 10000.0) % 100);
        }
        else if (targetType == Option::TARGET_0)
        {
            gNumbers.queue(eNumber::PLAY_2P_EXSCORE, 0);
            gNumbers.queue(eNumber::PLAY_1P_EXSCORE_DIFF, dp1.score2);
            gNumbers.queue(eNumber::PLAY_2P_EXSCORE_DIFF, -dp1.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_EX, 0);
            gNumbers.queue(eNumber::RESULT_TARGET_DIFF, -dp1.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_RATE, 0);
            gNumbers.queue(eNumber::RESULT_TARGET_RATE_DECIMAL2, 0);
        }
        else
        {
            auto dp2 = gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData();

            gNumbers.queue(eNumber::PLAY_2P_EXSCORE, dp2.score2);
            gNumbers.queue(eNumber::PLAY_1P_EXSCORE_DIFF, dp1.score2 - dp2.score2);
            gNumbers.queue(eNumber::PLAY_2P_EXSCORE_DIFF, dp2.score2 - dp1.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_EX, dp2.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_DIFF, dp2.score2 - dp1.score2);
            gNumbers.queue(eNumber::RESULT_TARGET_RATE, (int)std::floor(dp2.acc * 100.0));
            gNumbers.queue(eNumber::RESULT_TARGET_RATE_DECIMAL2, (int)std::floor(dp2.acc * 10000.0) % 100);

            int miss2 = dp2.miss;
            if (_missPlayer[PLAYER_SLOT_TARGET] != miss2)
            {
                _missPlayer[PLAYER_SLOT_TARGET] = miss2;
                _missLastTime = t;
            }
        }
    }

    gPlayContext.bgaTexture->update(rt, t.norm() - _missLastTime.norm() < _missBgaLength);

    gNumbers.queue(eNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM())));
    // play time / remain time
    {
        auto startTime = rt - gTimers.get(eTimer::PLAY_START);
        auto totalTime = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().norm();
        auto playtime_s = rt.norm() / 1000;
        auto remaintime_s = totalTime / 1000 - playtime_s;
        gNumbers.queue(eNumber::PLAY_MIN, int(playtime_s / 60));
        gNumbers.queue(eNumber::PLAY_SEC, int(playtime_s % 60));
        gNumbers.queue(eNumber::PLAY_REMAIN_MIN, int(remaintime_s / 60));
        gNumbers.queue(eNumber::PLAY_REMAIN_SEC, int(remaintime_s % 60));
        gSliders.queue(eSlider::SONG_PROGRESS, (double)rt.norm() / totalTime);
    }

    procCommonNotes();
    changeKeySampleMapping(rt);
	//updateBga();

    // charts
    if (rt.norm() / 500 >= gPlayContext.graphGauge[PLAYER_SLOT_PLAYER].size())
    {
        auto& g = gPlayContext.graphGauge[PLAYER_SLOT_PLAYER];
        auto& r = gPlayContext.ruleset[PLAYER_SLOT_PLAYER];
        int h = r->getClearHealth() * 100;
        if (!g.empty())
        {
            int ch = r->getData().health * 100;
            if ((g.back() < h && ch > h) || (g.back() > h && ch < h))
            {
                // just insert an interim point, as for a game we don't need to be too precise
                g.push_back(h);
            }
        }

        pushGraphPoints();
    }

    // health check (-> to failed)
    if (!_isExitingFromPlay)
    {
        if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFailed() &&
            (!gPlayContext.isBattle || gPlayContext.ruleset[PLAYER_SLOT_TARGET] == nullptr || gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed()))
        {
            pushGraphPoints();
            gTimers.queue(eTimer::FAIL_BEGIN, t.norm());
            gOptions.queue(eOption::PLAY_SCENE_STAT, Option::SPLAY_FAILED);
            _isExitingFromPlay = true;
            _state = ePlayState::FAILED;
            SoundMgr::stopSysSamples();
            SoundMgr::stopNoteSamples();
            SoundMgr::playSysSample(SoundChannelType::BGM_SYS, eSoundSample::SOUND_PLAYSTOP);
            for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
            {
                _input.unregister_p("SCENE_PRESS");
            }
            LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
        }

        if (!_isPlayerFinished[PLAYER_SLOT_PLAYER])
        {
            if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFinished() ||
                gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().combo == gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxCombo())
            {
                gTimers.queue(eTimer::PLAY_P1_FINISHED, t.norm());
                if (isPlaymodeDP())
                {
                    gTimers.queue(eTimer::PLAY_P2_FINISHED, t.norm());
                }

                if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().combo == gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxCombo())
                {
                    gTimers.queue(eTimer::PLAY_FULLCOMBO_1P, t.norm());
                    if (isPlaymodeDP())
                        gTimers.queue(eTimer::PLAY_FULLCOMBO_2P, t.norm());
                }

                _isPlayerFinished[PLAYER_SLOT_PLAYER] = true;
            }
        }
        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET] != nullptr && !_isPlayerFinished[PLAYER_SLOT_TARGET])
        {
            if (gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFinished() ||
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getData().combo == gPlayContext.ruleset[PLAYER_SLOT_TARGET]->getMaxCombo())
            {
                gTimers.queue(eTimer::PLAY_P2_FINISHED, t.norm());

                if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getData().combo == gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->getMaxCombo())
                {
                    gTimers.queue(eTimer::PLAY_FULLCOMBO_2P, t.norm());
                }

                _isPlayerFinished[PLAYER_SLOT_TARGET] = true;
            }
        }
    }

    spinTurntable(true);

    //last note check
    if (rt.hres() - gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getTotalLength().hres() >= 0)
    {
        gTimers.queue(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.queue(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _isExitingFromPlay = true;
        _state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
     
    gTimers.flush();
    gNumbers.flush();
    gOptions.flush();
    gSliders.flush();
}

void ScenePlay::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::PLAY_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_TARGET]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_MYBEST] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_MYBEST]->update(rt);
    }

    if (gChartContext.started)
        gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);
    spinTurntable(gChartContext.started);

	gPlayContext.bgaTexture->update(rt, false);

    if (ft >= _skin->info.timeOutro)
    {
        sceneEnding = true;
        if (_loadChartFuture.valid())
            _loadChartFuture.wait();

        if (_isExitingFromPlay)
        {
            removeInputJudgeCallback();

            bool cleared = false;
            if (isPlaymodeBattle())
            {
                if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isCleared() ||
                    gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isCleared())
                    cleared = true;
            }
            else
            {
                if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isCleared())
                    cleared = true;
            }

            gSwitches.set(eSwitch::RESULT_CLEAR, cleared);
        }

        // save lanecover settings
        ConfigMgr::set('P', cfg::P_LANECOVER_ENABLE, _lanecoverEnabled[PLAYER_SLOT_PLAYER]);
        if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
        {
            ConfigMgr::set('P', cfg::P_LANECOVER_TOP, gNumbers.get(eNumber::LANECOVER_TOP_1P));
            ConfigMgr::set('P', cfg::P_LANECOVER_BOTTOM, gNumbers.get(eNumber::LANECOVER_BOTTOM_1P));
        }
        ConfigMgr::set('P', cfg::P_LOCK_SPEED, _lockspeedEnabled[PLAYER_SLOT_PLAYER]);
        if (_lockspeedEnabled[PLAYER_SLOT_PLAYER])
        {
            ConfigMgr::set('P', cfg::P_GREENNUMBER, _lockspeedGreenNumber[PLAYER_SLOT_PLAYER]);
        }

        if (isPlaymodeBattle())
        {
            gPlayContext.battle2PLanecover = _lanecoverEnabled[PLAYER_SLOT_TARGET];
            if (_lanecoverEnabled[PLAYER_SLOT_TARGET])
            {
                gPlayContext.battle2PLanecoverTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
                gPlayContext.battle2PLanecoverBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
            }
            gPlayContext.battle2PLockSpeed = _lockspeedEnabled[PLAYER_SLOT_TARGET];
            if (_lockspeedEnabled[PLAYER_SLOT_TARGET])
            {
                gPlayContext.battle2PGreenNumber = _lockspeedGreenNumber[PLAYER_SLOT_TARGET];
            }
        }

        // check quick retry (start+select / white+black)
        bool wantRetry = false;
        auto h = _input.Holding();
        using namespace Input;
        if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
        {
            if ((h.test(K1START) && h.test(K1SELECT)) ||
                (h.test(K11) || h.test(K13) || h.test(K15) || h.test(K17) || h.test(K19)) && (h.test(K12) || h.test(K14) || h.test(K16) || h.test(K18)))
                wantRetry = true;
        }
        if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
        {
            if ((h.test(K2START) && h.test(K2SELECT)) ||
                (h.test(K21) || h.test(K23) || h.test(K25) || h.test(K27) || h.test(K29)) && (h.test(K22) || h.test(K24) || h.test(K26) || h.test(K28)))
                wantRetry = true;
        }

        gPlayContext.bgaTexture->reset();

        if (gPlayContext.isAuto)
        {
            gNextScene = (gPlayContext.isCourse && gChartContext.started) ? eScene::COURSE_TRANS : (gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT);
        }
        else if (wantRetry && gPlayContext.canRetry)
        {
            gNextScene = eScene::RETRY_TRANS;
        }
        else
        {
            gNextScene = (gPlayContext.isCourse || gChartContext.started) ? eScene::RESULT : (gQuitOnFinish ? eScene::EXIT_TRANS : eScene::SELECT);
        }
    }
}

void ScenePlay::updateFailed()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::PLAY_START);
    auto ft = t - gTimers.get(eTimer::FAIL_BEGIN);

    if (gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_TARGET]->update(rt);
    }
    if (gPlayContext.chartObj[PLAYER_SLOT_MYBEST] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_MYBEST]->update(rt);
    }

    if (gChartContext.started)
        gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentMetre() * 4.0)) % 1000);
    spinTurntable(gChartContext.started);

    //failed play finished, move to next scene. No fadeout
    if (ft.norm() >= _skin->info.timeFailed)
    {
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
}


void ScenePlay::procCommonNotes()
{
    assert(gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr);
    auto it = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteBgmExpired.begin();
    size_t max = std::min(_bgmSampleIdxBuf.size(), gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteBgmExpired.size());
    size_t i = 0;
    for (; i < max && it != gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteBgmExpired.end(); ++i, ++it)
    {
        //if ((it->index & 0xF0) == 0xE0)
        {
            // BGA
            /*
            switch (it->index)
            {
            case 0xE0: bgaBaseIdx =  (unsigned)std::get<long long>(it->value); break;
            case 0xE1: bgaLayerIdx = (unsigned)std::get<long long>(it->value); break;
            case 0xE2: bgaPoorIdx =  (unsigned)std::get<long long>(it->value); break;
            default: break;
            }
            */
        }
        //else
        {
            // BGM
            _bgmSampleIdxBuf[i] = (unsigned)it->dvalue;
        }
    }
    SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_bgmSampleIdxBuf.data());

    // also play keysound in auto
    if (gPlayContext.isAuto)
    {
        i = 0;
        auto it = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.end())
        {
            if ((it->flags & ~(Note::SCRATCH | Note::KEY_6_7)) == 0)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_keySampleIdxBuf.data());
    }

    // play auto-scratch keysound
    if (gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
    {
        i = 0;
        auto it = gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->noteExpired.end())
        {
            if ((it->flags & (Note::SCRATCH | Note::LN_TAIL)) == Note::SCRATCH)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_keySampleIdxBuf.data());
    }
    if (isPlaymodeBattle() && gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
    {
        i = 0;
        auto it = gPlayContext.chartObj[PLAYER_SLOT_TARGET]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_TARGET]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_TARGET]->noteExpired.end())
        {
            if ((it->flags & (Note::SCRATCH | Note::LN_TAIL)) == Note::SCRATCH)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_RIGHT, i, (size_t*)_keySampleIdxBuf.data());
    }
}

void ScenePlay::changeKeySampleMapping(const Time& t)
{
    static const Time MIN_REMAP_INTERVAL{ 1000 };

    auto changeKeySample = [&](Input::Pad k, int slot)
    {
        chart::NoteLaneIndex idx[3] = { chart::NoteLaneIndex::_, chart::NoteLaneIndex::_, chart::NoteLaneIndex::_ };
        HitableNote* pNote[3] = { nullptr, nullptr, nullptr };
        long long time[3] = { TIMER_NEVER, TIMER_NEVER, TIMER_NEVER };

        idx[0] = gPlayContext.chartObj[slot]->getLaneFromKey(chart::NoteLaneCategory::Note, k);
        if (idx[0] != chart::NoteLaneIndex::_)
        {
            pNote[0] = &*gPlayContext.chartObj[slot]->incomingNote(chart::NoteLaneCategory::Note, idx[0]);
            time[0] = pNote[0]->time.hres();
        }

        idx[1] = gPlayContext.chartObj[slot]->getLaneFromKey(chart::NoteLaneCategory::LN, k);
        if (idx[1] != chart::NoteLaneIndex::_)
        {
            auto itLNNote = gPlayContext.chartObj[slot]->incomingNote(chart::NoteLaneCategory::LN, idx[1]);
            while (!gPlayContext.chartObj[slot]->isLastNote(chart::NoteLaneCategory::LN, idx[1], itLNNote))
            {
                if (!(itLNNote->flags & Note::Flags::LN_TAIL))
                {
                    pNote[1] = &*itLNNote;
                    time[1] = pNote[1]->time.hres();
                    break;
                }
                itLNNote++;
            }
        }

        idx[2] = gPlayContext.chartObj[slot]->getLaneFromKey(chart::NoteLaneCategory::Invs, k);
        if (idx[2] != chart::NoteLaneIndex::_)
        {
            pNote[2] = &*gPlayContext.chartObj[slot]->incomingNote(chart::NoteLaneCategory::Invs, idx[2]);
            time[2] = pNote[2]->time.hres();
        }

        HitableNote* pNoteKey = nullptr;
        std::vector<std::pair<long long, size_t>> sortTmp;
        for (size_t i = 0; i < 3; ++i)
        {
            sortTmp.push_back(std::make_pair(time[i], i));
        }
        std::sort(sortTmp.begin(), sortTmp.end());
        for (size_t i = 0; i < 3; ++i)
        {
            size_t idxNoteKey = sortTmp[i].second;
            if (pNote[idxNoteKey])
            {
                pNoteKey = pNote[idxNoteKey];
                break;
            }
        }

        if (pNoteKey && pNoteKey->time - t <= MIN_REMAP_INTERVAL)
        {
            _currentKeySample[(size_t)k] = (size_t)pNoteKey->dvalue;

            if (k == Input::S1L) _currentKeySample[Input::S1R] = (size_t)pNoteKey->dvalue;
            if (k == Input::S2L) _currentKeySample[Input::S2R] = (size_t)pNoteKey->dvalue;
        }
    };

    assert(gPlayContext.chartObj[PLAYER_SLOT_PLAYER] != nullptr);
    for (size_t i = Input::K11; i <= Input::K19; ++i)
    {
        if (_inputAvailable[i])
            changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
    }
    if (!(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
    {
        for (size_t i = Input::S1L; i <= Input::S1R; ++i)
        {
            if (_inputAvailable[i])
                changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
        }
    }
    if (isPlaymodeDP())
    {
        for (size_t i = Input::K21; i <= Input::K29; ++i)
        {
            if (_inputAvailable[i])
                changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
        }
        if (!(gPlayContext.mods[PLAYER_SLOT_PLAYER].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
        {
            for (size_t i = Input::S2L; i <= Input::S2R; ++i)
            {
                if (_inputAvailable[i])
                    changeKeySample((Input::Pad)i, PLAYER_SLOT_PLAYER);
            }
        }
    }
	if (isPlaymodeBattle())
	{
		assert(gPlayContext.chartObj[PLAYER_SLOT_TARGET] != nullptr);
		for (size_t i = Input::K21; i <= Input::K29; ++i)
		{
			if (_inputAvailable[i])
				changeKeySample((Input::Pad)i, PLAYER_SLOT_TARGET);
		}
        if (!(gPlayContext.mods[PLAYER_SLOT_TARGET].assist_mask & PLAY_MOD_ASSIST_AUTOSCR))
        {
            for (size_t i = Input::S2L; i <= Input::S2R; ++i)
            {
                if (_inputAvailable[i])
                    changeKeySample((Input::Pad)i, PLAYER_SLOT_TARGET);
            }
        }
	}
}

void ScenePlay::spinTurntable(bool startedPlaying)
{
    if (startedPlaying)
    {
        auto t = Time();
        auto rt = t - gTimers.get(eTimer::PLAY_START);
        for (auto& aa : _ttAngleTime)
            aa = int(rt.norm() * 360 / 2000);
    }
    gNumbers.set(eNumber::_ANGLE_TT_1P, (_ttAngleTime[0] + (int)_ttAngleDiff[0]) % 360);
    gNumbers.set(eNumber::_ANGLE_TT_2P, (_ttAngleTime[1] + (int)_ttAngleDiff[1]) % 360);
}

////////////////////////////////////////////////////////////////////////////////
// CALLBACK
void ScenePlay::inputGamePress(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // individual keys
    if (!gPlayContext.isAuto && !gPlayContext.isReplay)
    {
        inputGamePressTimer(input, t);
    }
    if (gChartContext.started && gPlayContext.replayNew)
    {
        long long ms = t.norm() - gTimers.get(eTimer::PLAY_START);
        ReplayChart::Commands cmd;
        cmd.ms = ms;
        if (input[S1L])      { cmd.type = ReplayChart::Commands::Type::S1L_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[S1R])      { cmd.type = ReplayChart::Commands::Type::S1R_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K11])      { cmd.type = ReplayChart::Commands::Type::K11_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K12])      { cmd.type = ReplayChart::Commands::Type::K12_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K13])      { cmd.type = ReplayChart::Commands::Type::K13_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K14])      { cmd.type = ReplayChart::Commands::Type::K14_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K15])      { cmd.type = ReplayChart::Commands::Type::K15_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K16])      { cmd.type = ReplayChart::Commands::Type::K16_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K17])      { cmd.type = ReplayChart::Commands::Type::K17_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K18])      { cmd.type = ReplayChart::Commands::Type::K18_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K19])      { cmd.type = ReplayChart::Commands::Type::K19_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K1START])  { cmd.type = ReplayChart::Commands::Type::K1START_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K1SELECT]) { cmd.type = ReplayChart::Commands::Type::K1SELECT_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[S2L])      { cmd.type = ReplayChart::Commands::Type::S2L_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[S2R])      { cmd.type = ReplayChart::Commands::Type::S2R_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K21])      { cmd.type = ReplayChart::Commands::Type::K21_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K22])      { cmd.type = ReplayChart::Commands::Type::K22_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K23])      { cmd.type = ReplayChart::Commands::Type::K23_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K24])      { cmd.type = ReplayChart::Commands::Type::K24_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K25])      { cmd.type = ReplayChart::Commands::Type::K25_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K26])      { cmd.type = ReplayChart::Commands::Type::K26_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K27])      { cmd.type = ReplayChart::Commands::Type::K27_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K28])      { cmd.type = ReplayChart::Commands::Type::K28_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K29])      { cmd.type = ReplayChart::Commands::Type::K29_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K2START])  { cmd.type = ReplayChart::Commands::Type::K2START_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K2SELECT]) { cmd.type = ReplayChart::Commands::Type::K2SELECT_DOWN; gPlayContext.replayNew->commands.push_back(cmd); }
    }

    // double click START: toggle top lanecover
    if (input[K1START] || isPlaymodeDP() && input[K2START])
    {
        if (t > _startPressedTime[PLAYER_SLOT_PLAYER] && (t - _startPressedTime[PLAYER_SLOT_PLAYER]).norm() < 200)
        {
            _lanecoverEnabled[PLAYER_SLOT_PLAYER] = !_lanecoverEnabled[PLAYER_SLOT_PLAYER];
            _startPressedTime[PLAYER_SLOT_PLAYER] = TIMER_NEVER;

            if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
            {
                gSliders.set(eSlider::SUD_1P, gNumbers.get(eNumber::LANECOVER_TOP_1P) / 1000.0);
                if (isPlaymodeDP())
                    gSliders.set(eSlider::SUD_2P, gNumbers.get(eNumber::LANECOVER_TOP_1P) / 1000.0);
            }
        }
        else
        {
            _startPressedTime[PLAYER_SLOT_PLAYER] = t;
        }
    }
    if (isPlaymodeBattle() && input[K2START])
    {
        if (t > _startPressedTime[PLAYER_SLOT_TARGET] && (t - _startPressedTime[PLAYER_SLOT_TARGET]).norm() < 200)
        {
            _lanecoverEnabled[PLAYER_SLOT_TARGET] = !_lanecoverEnabled[PLAYER_SLOT_TARGET];
            _startPressedTime[PLAYER_SLOT_TARGET] = TIMER_NEVER;

            if (_lanecoverEnabled[PLAYER_SLOT_TARGET])
                gSliders.set(eSlider::SUD_2P, gNumbers.get(eNumber::LANECOVER_TOP_2P) / 1000.0);
        }
        else
        {
            _startPressedTime[PLAYER_SLOT_TARGET] = t;
        }
    }

    // double click SELECT when lanecover enabled: lock green number
    if (input[K1SELECT] || isPlaymodeDP() && input[K2SELECT])
    {
        if (t > _selectPressedTime[PLAYER_SLOT_PLAYER] && (t - _selectPressedTime[PLAYER_SLOT_PLAYER]).norm() < 200)
        {
            _lockspeedEnabled[PLAYER_SLOT_PLAYER] = !_lockspeedEnabled[PLAYER_SLOT_PLAYER];
            _selectPressedTime[PLAYER_SLOT_PLAYER] = TIMER_NEVER;

            gSwitches.set(eSwitch::P1_LOCK_SPEED, _lockspeedEnabled[PLAYER_SLOT_PLAYER]);
            if (_lockspeedEnabled[PLAYER_SLOT_PLAYER])
            {
                double bpm = gPlayContext.mods[PLAYER_SLOT_PLAYER].hispeedFix == eModHs::CONSTANT ?
                    150.0 : gPlayContext.chartObj[PLAYER_SLOT_PLAYER]->getCurrentBPM();
                double hs = gPlayContext.Hispeed;
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
                double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_PLAYER] = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / visible);

                double den = hs * bpm;
                _lockspeedGreenNumber[PLAYER_SLOT_PLAYER] = den != 0.0 ? int(std::round(visible * 120.0 * 1200 / hs / bpm)) : 0;
            }
        }
        else
        {
            _selectPressedTime[PLAYER_SLOT_PLAYER] = t;
        }
    }
    if (isPlaymodeBattle() && input[K2SELECT])
    {
        if (t > _selectPressedTime[PLAYER_SLOT_TARGET] && (t - _selectPressedTime[PLAYER_SLOT_TARGET]).norm() < 200)
        {
            _lockspeedEnabled[PLAYER_SLOT_TARGET] = !_lockspeedEnabled[PLAYER_SLOT_TARGET];
            _selectPressedTime[PLAYER_SLOT_TARGET] = TIMER_NEVER;

            gSwitches.set(eSwitch::P2_LOCK_SPEED, _lockspeedEnabled[PLAYER_SLOT_PLAYER]);
            if (_lockspeedEnabled[PLAYER_SLOT_TARGET])
            {
                double bpm = gPlayContext.mods[PLAYER_SLOT_TARGET].hispeedFix == eModHs::CONSTANT ?
                    150.0 : gPlayContext.chartObj[PLAYER_SLOT_TARGET]->getCurrentBPM();
                double hs = gPlayContext.battle2PHispeed;
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
                double visible = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_TARGET] = (visible == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / visible);

                double den = hs * bpm;
                _lockspeedGreenNumber[PLAYER_SLOT_PLAYER] = den != 0.0 ? int(std::round(visible * 120.0 * 1200 / hs / bpm)) : 0;
            }
        }
        else
        {
            _selectPressedTime[PLAYER_SLOT_TARGET] = t;
        }
    }


    if (true)
    {
        if (input[K1START] || isPlaymodeDP() && input[K2START]) _isHoldingStart[PLAYER_SLOT_PLAYER] = true;
        if (input[K1SELECT] || isPlaymodeDP() && input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_PLAYER] = true;

        bool white = (input[K11] || input[K13] || input[K15] || input[K17] || input[K19]) ||
            isPlaymodeDP() && (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]);
        bool black = (input[K12] || input[K14] || input[K16] || input[K18]) ||
            isPlaymodeDP() && (input[K22] || input[K24] || input[K26] || input[K28]);

        if (input[K1SPDUP] || isPlaymodeDP() && input[K2SPDUP] || 
            (_isHoldingStart[PLAYER_SLOT_PLAYER] || _isHoldingSelect[PLAYER_SLOT_PLAYER]) && black)
        {
            if (gPlayContext.Hispeed < 10.0)
            {
                gPlayContext.HispeedGradientStart = t;
                gPlayContext.HispeedGradientFrom = gPlayContext.HispeedGradientNow;

                gPlayContext.Hispeed = std::min(gPlayContext.Hispeed + 0.25, 10.0);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
            }
        }

        if (input[K1SPDDN] || isPlaymodeDP() && input[K2SPDDN] || 
            (_isHoldingStart[PLAYER_SLOT_PLAYER] || _isHoldingSelect[PLAYER_SLOT_PLAYER]) && white)
        {
            if (gPlayContext.Hispeed > 0.25)
            {
                gPlayContext.HispeedGradientStart = t;
                gPlayContext.HispeedGradientFrom = gPlayContext.HispeedGradientNow;

                gPlayContext.Hispeed = std::max(gPlayContext.Hispeed - 0.25, 0.25);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
            }
        }

    }
    if (isPlaymodeBattle())
    {
        if (input[K2START]) _isHoldingStart[PLAYER_SLOT_TARGET] = true;
        if (input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_TARGET] = true;

        bool white = (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]);
        bool black = (input[K22] || input[K24] || input[K26] || input[K28]);

        if (input[K2SPDUP] || 
            (_isHoldingStart[PLAYER_SLOT_TARGET] || _isHoldingSelect[PLAYER_SLOT_TARGET]) && black)
        {
            if (gPlayContext.battle2PHispeed < 10.0)
            {
                gPlayContext.battle2PHispeedGradientStart = t;
                gPlayContext.battle2PHispeedGradientFrom = gPlayContext.battle2PHispeedGradientNow;

                gPlayContext.battle2PHispeed = std::min(gPlayContext.battle2PHispeed + 0.25, 10.0);
                gNumbers.queue(eNumber::HS_2P, (int)std::round(gPlayContext.battle2PHispeed * 100));
            }
        }
        if (input[K2SPDDN] || 
            (_isHoldingStart[PLAYER_SLOT_TARGET] || _isHoldingSelect[PLAYER_SLOT_TARGET]) && white)
        {
            if (gPlayContext.battle2PHispeed > 0.25)
            {
                gPlayContext.battle2PHispeedGradientStart = t;
                gPlayContext.battle2PHispeedGradientFrom = gPlayContext.battle2PHispeedGradientNow;

                gPlayContext.battle2PHispeed = std::max(gPlayContext.battle2PHispeed - 0.25, 0.25);
                gNumbers.queue(eNumber::HS_2P, (int)std::round(gPlayContext.battle2PHispeed * 100));
            }
        }

    }
    gTimers.flush();
    gSwitches.flush();
    gNumbers.flush();


    if (_state != ePlayState::FADEOUT &&
        (input[Input::ESC] || (input[Input::K1START] && input[Input::K1SELECT]) || (input[Input::K2START] && input[Input::K2SELECT])))
    {
        if (gChartContext.started)
        {
            _isExitingFromPlay = true;

            if (!_isPlayerFinished[PLAYER_SLOT_PLAYER])
            {
                gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->fail();
            }
            if (!_isPlayerFinished[PLAYER_SLOT_TARGET] && isPlaymodeBattle() && gPlayContext.ruleset[PLAYER_SLOT_TARGET])
            {
                gPlayContext.ruleset[PLAYER_SLOT_TARGET]->fail();
            }

            pushGraphPoints();

        }

        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _state = ePlayState::FADEOUT;
        LOG_DEBUG << "[Play] State changed to FADEOUT";
    }
}

void ScenePlay::inputGamePressTimer(InputMask& input, const Time& t)
{
    using namespace Input;

    if (true)
    {
        if (input[S1L] || input[S1R])
        {
            _scratchDir[PLAYER_SLOT_PLAYER] = input[S1L] ? AxisDir::AXIS_UP : AxisDir::AXIS_DOWN;
            gTimers.queue(eTimer::S1_DOWN, t.norm());
            gTimers.queue(eTimer::S1_UP, TIMER_NEVER);
            gSwitches.queue(eSwitch::S1_DOWN, true);
        }
    }
    if (isPlaymodeBattle() || isPlaymodeDP())
    {
        if (input[S2L] || input[S2R])
        {
            _scratchDir[PLAYER_SLOT_TARGET] = input[S2L] ? AxisDir::AXIS_UP : AxisDir::AXIS_DOWN;
            gTimers.queue(eTimer::S2_DOWN, t.norm());
            gTimers.queue(eTimer::S2_UP, TIMER_NEVER);
            gSwitches.queue(eSwitch::S2_DOWN, true);
        }
    }

    InputMask inputSample = input;
    if (isPlaymodeBattle())
    {
        if (gPlayContext.ruleset[PLAYER_SLOT_PLAYER]->isFailed()) input &= ~INPUT_MASK_1P;
        if (gPlayContext.ruleset[PLAYER_SLOT_TARGET]->isFailed()) input &= ~INPUT_MASK_2P;
    }
    size_t sampleCount = 0;
    for (size_t i = S1L; i < LANE_COUNT; ++i)
    {
        if (inputSample[i])
        {
            if (_currentKeySample[i])
            {
                _keySampleIdxBuf[sampleCount++] = _currentKeySample[i];
            }
        }
        if (input[i])
        {
            gTimers.queue(InputGamePressMap[i].tm, t.norm());
            gTimers.queue(InputGameReleaseMap[i].tm, TIMER_NEVER);
            gSwitches.queue(InputGamePressMap[i].sw, true);
        }
    }
    SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, (size_t*)&_keySampleIdxBuf[0]);
}

// CALLBACK
void ScenePlay::inputGameHold(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // delay start
    if (!gChartContext.started && (input[K1START] || input[K1SELECT] || input[K2START] || input[K2SELECT]))
    {
        _readyTime = t;
    }

    // turntable spin
    if (input[S1L]) _ttAngleDiff[PLAYER_SLOT_PLAYER] -= 0.5;
    if (input[S1R]) _ttAngleDiff[PLAYER_SLOT_PLAYER] += 0.5;
    if (input[S2L]) _ttAngleDiff[PLAYER_SLOT_TARGET] -= 0.5;
    if (input[S2R]) _ttAngleDiff[PLAYER_SLOT_TARGET] += 0.5;

    // lanecover
    if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
    {
        if (_isHoldingStart[PLAYER_SLOT_PLAYER])
        {
            if (input[S1L])
                _lanecoverAdd[PLAYER_SLOT_PLAYER]--;  // -1 per ms
            if (input[S1R])
                _lanecoverAdd[PLAYER_SLOT_PLAYER]++;  // +1 per ms
            if (isPlaymodeDP())
            {
                if (input[S2L])
                    _lanecoverAdd[PLAYER_SLOT_PLAYER]--;  // -1 per ms
                if (input[S2R])
                    _lanecoverAdd[PLAYER_SLOT_PLAYER]++;  // +1 per ms
            }
        }
    }
    if (isPlaymodeBattle() && _lanecoverEnabled[PLAYER_SLOT_TARGET])
    {
        if (_isHoldingStart[PLAYER_SLOT_TARGET])
        {
            if (input[S2L])
                _lanecoverAdd[PLAYER_SLOT_TARGET]--;  // -1 per ms
            if (input[S2R])
                _lanecoverAdd[PLAYER_SLOT_TARGET]++;  // +1 per ms
        }
    }

    // hispeed
    if (true)
    {
        if (_isHoldingSelect[PLAYER_SLOT_PLAYER])
        {
            if (input[S1L]) 
                _hispeedAdd[PLAYER_SLOT_PLAYER]--;  // -1 per ms
            if (input[S1R])
                _hispeedAdd[PLAYER_SLOT_PLAYER]++;  // +1 per ms
            if (isPlaymodeDP())
            {
                if (input[S2L])
                    _hispeedAdd[PLAYER_SLOT_PLAYER]--;  // -1 per ms
                if (input[S2R])
                    _hispeedAdd[PLAYER_SLOT_PLAYER]++;  // +1 per ms
            }
        }
    }
    if (isPlaymodeBattle())
    {
        if (_isHoldingSelect[PLAYER_SLOT_TARGET])
        {
            if (input[S2L])
                _hispeedAdd[PLAYER_SLOT_TARGET]--;  // -1 per ms
            if (input[S2R])
                _hispeedAdd[PLAYER_SLOT_TARGET]++;  // +1 per ms
        }
    }
}

// CALLBACK
void ScenePlay::inputGameRelease(InputMask& m, const Time& t)
{
    using namespace Input;
    auto input = _inputAvailable & m;

    if (!gPlayContext.isAuto && !gPlayContext.isReplay)
    {
        inputGameReleaseTimer(input, t);
    }
    
    if (gChartContext.started && gPlayContext.replayNew)
    {
        long long ms = t.norm() - gTimers.get(eTimer::PLAY_START);
        ReplayChart::Commands cmd;
        cmd.ms = ms;
        if (input[S1L])      { cmd.type = ReplayChart::Commands::Type::S1L_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[S1R])      { cmd.type = ReplayChart::Commands::Type::S1R_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K11])      { cmd.type = ReplayChart::Commands::Type::K11_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K12])      { cmd.type = ReplayChart::Commands::Type::K12_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K13])      { cmd.type = ReplayChart::Commands::Type::K13_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K14])      { cmd.type = ReplayChart::Commands::Type::K14_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K15])      { cmd.type = ReplayChart::Commands::Type::K15_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K16])      { cmd.type = ReplayChart::Commands::Type::K16_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K17])      { cmd.type = ReplayChart::Commands::Type::K17_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K18])      { cmd.type = ReplayChart::Commands::Type::K18_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K19])      { cmd.type = ReplayChart::Commands::Type::K19_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K1START])  { cmd.type = ReplayChart::Commands::Type::K1START_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K1SELECT]) { cmd.type = ReplayChart::Commands::Type::K1SELECT_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[S2L])      { cmd.type = ReplayChart::Commands::Type::S2L_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[S2R])      { cmd.type = ReplayChart::Commands::Type::S2R_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K21])      { cmd.type = ReplayChart::Commands::Type::K21_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K22])      { cmd.type = ReplayChart::Commands::Type::K22_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K23])      { cmd.type = ReplayChart::Commands::Type::K23_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K24])      { cmd.type = ReplayChart::Commands::Type::K24_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K25])      { cmd.type = ReplayChart::Commands::Type::K25_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K26])      { cmd.type = ReplayChart::Commands::Type::K26_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K27])      { cmd.type = ReplayChart::Commands::Type::K27_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K28])      { cmd.type = ReplayChart::Commands::Type::K28_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K29])      { cmd.type = ReplayChart::Commands::Type::K29_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K2START])  { cmd.type = ReplayChart::Commands::Type::K2START_UP; gPlayContext.replayNew->commands.push_back(cmd); }
        if (input[K2SELECT]) { cmd.type = ReplayChart::Commands::Type::K2SELECT_UP; gPlayContext.replayNew->commands.push_back(cmd); }
    }

    if (input[K1START] || isPlaymodeDP() && input[K2START]) _isHoldingStart[PLAYER_SLOT_PLAYER] = false;
    if (input[K1SELECT] || isPlaymodeDP() && input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_PLAYER] = false;
    if (isPlaymodeBattle())
    {
        if (input[K2START]) _isHoldingStart[PLAYER_SLOT_TARGET] = false;
        if (input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_TARGET] = false;
    }
}

void ScenePlay::inputGameReleaseTimer(InputMask& input, const Time& t)
{
    using namespace Input;

    size_t count = 0;
    for (size_t i = Input::S1L; i < Input::LANE_COUNT; ++i)
        if (input[i])
        {
            gTimers.set(InputGamePressMap[i].tm, TIMER_NEVER);
            gTimers.set(InputGameReleaseMap[i].tm, t.norm());
            gSwitches.set(InputGameReleaseMap[i].sw, false);

            // TODO stop sample playing while release in LN notes
        }

    if (true)
    {
        if (input[S1L] || input[S1R])
        {
            if ((input[S1L] && _scratchDir[PLAYER_SLOT_PLAYER] == AxisDir::AXIS_UP) ||
                (input[S1R] && _scratchDir[PLAYER_SLOT_PLAYER] == AxisDir::AXIS_DOWN))
            {
                gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
                gTimers.set(eTimer::S1_UP, t.norm());
                gSwitches.set(eSwitch::S1_DOWN, false);
                _scratchDir[PLAYER_SLOT_PLAYER] = AxisDir::AXIS_NONE;
            }
        }
    }
    if (isPlaymodeBattle() || isPlaymodeDP())
    {
        if (input[S2L] || input[S2R])
        {
            if ((input[S2L] && _scratchDir[PLAYER_SLOT_TARGET] == AxisDir::AXIS_UP) ||
                (input[S2R] && _scratchDir[PLAYER_SLOT_TARGET] == AxisDir::AXIS_DOWN))
            {
                gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
                gTimers.set(eTimer::S2_UP, t.norm());
                gSwitches.set(eSwitch::S2_DOWN, false);
                _scratchDir[PLAYER_SLOT_TARGET] = AxisDir::AXIS_NONE;
            }
        }
    }
}

// CALLBACK
void ScenePlay::inputGameAxis(double S1, double S2, const Time& t)
{
    using namespace Input;

    // turntable spin
    _ttAngleDiff[PLAYER_SLOT_PLAYER] += S1 * 2.0 * 360;
    _ttAngleDiff[PLAYER_SLOT_TARGET] += S2 * 2.0 * 360;

    if (!gPlayContext.isAuto && (!gPlayContext.isReplay || !gChartContext.started))
    {
        _scratchAccumulator[PLAYER_SLOT_PLAYER] += S1;
        _scratchAccumulator[PLAYER_SLOT_TARGET] += S2;

        double lanecoverThreshold = 0.0002;

        // lanecover
        if (_lanecoverEnabled[PLAYER_SLOT_PLAYER])
        {
            if (_isHoldingStart[PLAYER_SLOT_PLAYER])
            {
                _lanecoverAdd[PLAYER_SLOT_PLAYER] += (int)std::round(S1 / lanecoverThreshold);
                if (isPlaymodeDP())
                    _lanecoverAdd[PLAYER_SLOT_PLAYER] += (int)std::round(S2 / lanecoverThreshold);
            }
        }
        if (isPlaymodeBattle() && _lanecoverEnabled[PLAYER_SLOT_TARGET])
        {
            if (_isHoldingStart[PLAYER_SLOT_TARGET])
            {
                _lanecoverAdd[PLAYER_SLOT_TARGET] += (int)std::round(S2 / lanecoverThreshold);
            }
        }

        // hispeed
        if (true)
        {
            if (_isHoldingSelect[PLAYER_SLOT_PLAYER])
            {
                _hispeedAdd[PLAYER_SLOT_PLAYER] += (int)std::round(S1 / lanecoverThreshold);
                if (isPlaymodeDP())
                    _hispeedAdd[PLAYER_SLOT_PLAYER] += (int)std::round(S2 / lanecoverThreshold);
            }
        }
        if (isPlaymodeBattle())
        {
            if (_isHoldingSelect[PLAYER_SLOT_TARGET])
            {
                _hispeedAdd[PLAYER_SLOT_TARGET] += (int)std::round(S2 / lanecoverThreshold);
            }
        }
    }
}