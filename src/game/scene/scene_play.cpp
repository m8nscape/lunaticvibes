#include <cassert>
#include <future>
#include <set>
#include "scene_play.h"
#include "scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_bms.h"
#include "game/ruleset/ruleset_bms_auto.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/chart/chart_bms.h"
#include "game/graphics/sprite_video.h"
#include "config/config_mgr.h"
#include "common/log.h"
#include "common/sysutil.h"
#include "game/sound/sound_sample.h"

bool ScenePlay::isPlaymodeSinglePlay() const
{
    switch (gPlayContext.mode)
    {
        case eMode::PLAY5:
        case eMode::PLAY7:
        case eMode::PLAY9:
            return true;

        case eMode::PLAY10:
        case eMode::PLAY14:
            return false;
    }
    return true;
}
bool ScenePlay::isPlaymodeBattle() const
{
    return (_playmode == ePlayMode::LOCAL_BATTLE || _playmode == ePlayMode::AUTO_BATTLE);
}
bool ScenePlay::isPlaymodeAuto() const
{
    return (_playmode == ePlayMode::AUTO || _playmode == ePlayMode::AUTO_BATTLE);
}

ScenePlay::ScenePlay(): vScene(gPlayContext.mode, 1000, true)
{
    _scene = eScene::PLAY;

    switch (gPlayContext.mode)
    {
    case eMode::PLAY5:
    case eMode::PLAY7:
    case eMode::PLAY9:
    case eMode::PLAY10:
    case eMode::PLAY14:
        _playmode = gPlayContext.isAuto ? ePlayMode::AUTO : ePlayMode::SINGLE;
        break;

    case eMode::PLAY5_2:
    case eMode::PLAY7_2:
    case eMode::PLAY9_2:
        _playmode = gPlayContext.isAuto ? ePlayMode::AUTO_BATTLE : ePlayMode::LOCAL_BATTLE;
        break;

    default:
        break;
    }

    _currentKeySample.assign(Input::ESC, 0);

    _scratchSpeed[PLAYER_SLOT_1P] = ConfigMgr::get('P', cfg::P_INPUT_SPEED_S1A, 0.2);
    _scratchSpeed[PLAYER_SLOT_2P] = ConfigMgr::get('P', cfg::P_INPUT_SPEED_S2A, 0.2);

    _lanecoverEnabled[PLAYER_SLOT_1P] = ConfigMgr::get('P', cfg::P_LANECOVER_ENABLE, false);
    if (_lanecoverEnabled[PLAYER_SLOT_1P])
        gSliders.set(eSlider::SUD_1P, gNumbers.get(eNumber::LANECOVER_TOP_1P) / 1000.0);
    if (!isPlaymodeSinglePlay())
    {
        _lanecoverEnabled[PLAYER_SLOT_2P] = gPlayContext.battle2PLanecover;
        if (_lanecoverEnabled[PLAYER_SLOT_2P])
            gSliders.set(eSlider::SUD_2P, gNumbers.get(eNumber::LANECOVER_TOP_2P) / 1000.0);
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

    if (_lanecoverEnabled[PLAYER_SLOT_1P])
    {
        gSliders.queue(eSlider::SUD_1P, lcTop1 / 1000.0);
    }
    if (!isPlaymodeSinglePlay() && _lanecoverEnabled[PLAYER_SLOT_2P])
    {
        gSliders.queue(eSlider::SUD_2P, lcTop2 / 1000.0);
    }

    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P | INPUT_MASK_2P;

    _state = ePlayState::PREPARE;

    if (gChartContext.chartObj == nullptr || !gChartContext.chartObj->isLoaded())
    {
        if (gChartContext.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            return;
        }

        gChartContext.chartObj = vChartFormat::createFromFile(gChartContext.path);
    }

    if (gChartContext.chartObj == nullptr || !gChartContext.chartObj->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart: " << gChartContext.path.u8string();

        gNextScene = eScene::SELECT;

        return;
    }
    gChartContext.title = gChartContext.chartObj->title;
    gChartContext.title2 = gChartContext.chartObj->title2;
    gChartContext.artist = gChartContext.chartObj->artist;
    gChartContext.artist2 = gChartContext.chartObj->artist2;
    gChartContext.genre = gChartContext.chartObj->genre;
    gChartContext.minBPM = gChartContext.chartObj->minBPM;
    gChartContext.startBPM = gChartContext.chartObj->startBPM;
    gChartContext.maxBPM = gChartContext.chartObj->maxBPM;

    //load chart object from Chart object
    switch (gChartContext.chartObj->type())
    {
    case eChartFormat::BMS:
    {
        auto bms = std::reinterpret_pointer_cast<BMS>(gChartContext.chartObj);
        // TODO mods

        if (isPlaymodeAuto())
        {
            gPlayContext.chartObj[PLAYER_SLOT_1P] = std::make_shared<chartBMS>(PLAYER_SLOT_1P, bms);
        }
        else
        {
            if (isPlaymodeBattle())
            {
                gPlayContext.chartObj[PLAYER_SLOT_1P] = std::make_shared<chartBMS>(PLAYER_SLOT_1P, bms);
                gPlayContext.chartObj[PLAYER_SLOT_2P] = std::make_shared<chartBMS>(PLAYER_SLOT_2P, bms);
            }
            else
            {
                gPlayContext.chartObj[PLAYER_SLOT_1P] = std::make_shared<chartBMS>(PLAYER_SLOT_1P, bms);
                gPlayContext.chartObj[PLAYER_SLOT_2P] = std::make_shared<chartBMS>(PLAYER_SLOT_1P, bms);    // create for rival; loading with 1P options
            }
        }
        _chartLoaded = true;
        gNumbers.set(eNumber::PLAY_REMAIN_MIN, int(gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() / 1000 / 60));
        gNumbers.set(eNumber::PLAY_REMAIN_SEC, int(gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() / 1000 % 60));
        break;
    }

    case eChartFormat::BMSON:
    default:
        LOG_WARNING << "[Play] chart format not supported.";

        gNextScene = eScene::SELECT;
        return;
    }

    gPlayContext.remainTime = gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength();

    if (ConfigMgr::get('P', cfg::P_LOCK_SPEED, false))
    {
        _lockspeedEnabled[PLAYER_SLOT_1P] = true;

        int green = ConfigMgr::get('P', cfg::P_GREENNUMBER, 1200);
        double bpm = gChartContext.HSFixBPMFactor1P * gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM();
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
        double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;

        gPlayContext.Hispeed = (green * bpm <= 0.0) ? 200 : std::min(lc * 120.0 * 1200 / green / bpm, 10.0);
        gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
        _lockspeedValue[PLAYER_SLOT_1P] = (lc == 0.0) ? std::numeric_limits<double>::max() : (bpm * gPlayContext.Hispeed / lc);
    }
    if (!isPlaymodeSinglePlay() && gPlayContext.battle2PLockSpeed)
    {
        _lockspeedEnabled[PLAYER_SLOT_2P] = true;

        int green = gPlayContext.battle2PGreenNumber;
        double bpm = gChartContext.HSFixBPMFactor2P * gPlayContext.chartObj[PLAYER_SLOT_2P]->getCurrentBPM();
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
        double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;

        gPlayContext.battle2PHispeed = (green * bpm <= 0.0) ? 200 : std::min(lc * 120.0 * 1200 / green / bpm, 10.0);
        gNumbers.queue(eNumber::HS_2P, (int)std::round(gPlayContext.battle2PHispeed));
        _lockspeedValue[PLAYER_SLOT_2P] = (lc == 0.0) ? std::numeric_limits<double>::max() : (bpm * gPlayContext.battle2PHispeed / lc);
    }

	// global info

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
        eBargraph::PLAY_MYBEST,
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
        switch (gPlayContext.mods[PLAYER_SLOT_1P].gauge)
        {
        case eModGauge::NORMAL:    tmp = eGaugeOp::GROOVE; break;
        case eModGauge::HARD:      tmp = eGaugeOp::SURVIVAL; break;
        case eModGauge::DEATH:     tmp = eGaugeOp::EX_SURVIVAL; break;
        case eModGauge::EASY:      tmp = eGaugeOp::GROOVE; break;
        case eModGauge::PATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
        case eModGauge::GATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
        case eModGauge::ASSISTEASY:tmp = eGaugeOp::GROOVE; break;
        case eModGauge::EXHARD:    tmp = eGaugeOp::EX_SURVIVAL; break;
        default: break;
        }
        _skin->setExtendedProperty("GAUGETYPE_1P"s, (void*)&tmp);

        if (isPlaymodeBattle())
        {
            switch (gPlayContext.mods[PLAYER_SLOT_2P].gauge)
            {
            case eModGauge::NORMAL:    tmp = eGaugeOp::GROOVE; break;
            case eModGauge::HARD:      tmp = eGaugeOp::SURVIVAL; break;
            case eModGauge::EASY:      tmp = eGaugeOp::GROOVE; break;
            case eModGauge::DEATH:     tmp = eGaugeOp::EX_SURVIVAL; break;
            case eModGauge::PATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
            case eModGauge::GATTACK:   tmp = eGaugeOp::EX_SURVIVAL; break;
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

void ScenePlay::setTempInitialHealthBMS()
{
    if (!gPlayContext.isCourse || gPlayContext.isCourseFirstStage)
    {
        switch (gPlayContext.mods[PLAYER_SLOT_1P].gauge)
        {
        case eModGauge::NORMAL:
        case eModGauge::EASY:
        case eModGauge::ASSISTEASY:
            gPlayContext.initialHealth[PLAYER_SLOT_1P] = 0.2;
            gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, 20);
            break;

        case eModGauge::HARD:
        case eModGauge::DEATH:
        case eModGauge::PATTACK:
        case eModGauge::GATTACK:
        case eModGauge::EXHARD:
            gPlayContext.initialHealth[PLAYER_SLOT_1P] = 1.0;
            gNumbers.queue(eNumber::PLAY_1P_GROOVEGAUGE, 100);
            break;

        default: break;
        }

        if (isPlaymodeBattle())
        {
            switch (gPlayContext.mods[PLAYER_SLOT_2P].gauge)
            {
            case eModGauge::NORMAL:
            case eModGauge::EASY:
            case eModGauge::ASSISTEASY:
                gPlayContext.initialHealth[PLAYER_SLOT_2P] = 0.2;
                gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, 20);
                break;

            case eModGauge::HARD:
            case eModGauge::DEATH:
            case eModGauge::PATTACK:
            case eModGauge::GATTACK:
            case eModGauge::EXHARD:
                gPlayContext.initialHealth[PLAYER_SLOT_2P] = 1.0;
                gNumbers.queue(eNumber::PLAY_2P_GROOVEGAUGE, 100);
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
            switch (std::reinterpret_pointer_cast<BMS>(gChartContext.chartObj)->rank)
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

        if (isPlaymodeAuto())
        {
            gPlayContext.ruleset[PLAYER_SLOT_1P] = std::make_shared<RulesetBMSAuto>(
                gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_1P],
                gPlayContext.mods[PLAYER_SLOT_1P].gauge, keys, judgeDiff,
                gPlayContext.initialHealth[PLAYER_SLOT_1P], RulesetBMS::PlaySide::AUTO);
        }
        else
        {
            if (isPlaymodeBattle())
            {
                gPlayContext.ruleset[PLAYER_SLOT_1P] = std::make_shared<RulesetBMS>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_1P],
                    gPlayContext.mods[PLAYER_SLOT_1P].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_1P], RulesetBMS::PlaySide::BATTLE_1P);

                gPlayContext.ruleset[PLAYER_SLOT_2P] = std::make_shared<RulesetBMS>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_2P],
                    gPlayContext.mods[PLAYER_SLOT_2P].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_2P], RulesetBMS::PlaySide::BATTLE_2P);
            }
            else
            {
                gPlayContext.ruleset[PLAYER_SLOT_1P] = std::make_shared<RulesetBMS>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_1P],
                    gPlayContext.mods[PLAYER_SLOT_1P].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_1P], (keys == 10 || keys == 14) ? RulesetBMS::PlaySide::DOUBLE : RulesetBMS::PlaySide::SINGLE);

                gPlayContext.ruleset[PLAYER_SLOT_2P] = std::make_shared<RulesetBMSAuto>(
                    gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_2P],
                    gPlayContext.mods[PLAYER_SLOT_2P].gauge, keys, judgeDiff,
                    gPlayContext.initialHealth[PLAYER_SLOT_2P], RulesetBMS::PlaySide::RIVAL);

                int targetRate = gNumbers.get(eNumber::DEFAULT_TARGET_RATE);
                double targetRateReal = 0.0;
                switch (targetRate)
                {
                    // TODO set target name
                case 22:  targetRateReal = 2.0 / 9; break;  // E
                case 33:  targetRateReal = 3.0 / 9; break;  // D
                case 44:  targetRateReal = 4.0 / 9; break;  // C
                case 55:  targetRateReal = 5.0 / 9; break;  // B
                case 66:  targetRateReal = 6.0 / 9; break;  // A
                case 77:  targetRateReal = 7.0 / 9; break;  // AA
                case 88:  targetRateReal = 8.0 / 9; break;  // AAA
                case 100: targetRateReal = 1.0;     break;  // MAX
                default:  targetRateReal = targetRate / 100.0; break;
                }
                std::reinterpret_pointer_cast<RulesetBMSAuto>(gPlayContext.ruleset[PLAYER_SLOT_2P])->setTargetRate(targetRateReal);
                gBargraphs.set(eBargraph::PLAY_RIVAL_EXSCORE_FINAL, targetRateReal);
            }
        }

        _rulesetLoaded = true;
    }
    break;

    default:
        break;
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
    if (gSwitches.get(eSwitch::SYSTEM_BGA) && !gChartContext.isBgaLoaded && !sceneEnding)
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
			gPlayContext.bgaTexture->setSlotFromBMS(*std::reinterpret_pointer_cast<chartBMS>(gPlayContext.chartObj[PLAYER_SLOT_1P]));
            gChartContext.isBgaLoaded = true;
        });
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (gPlayContext.ruleset[PLAYER_SLOT_1P] != nullptr)
    {
        auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
        _input.register_p("JUDGE_PRESS_1", fp);
        auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
        _input.register_h("JUDGE_HOLD_1", fh);
        auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
        _input.register_r("JUDGE_RELEASE_1", fr);
        auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2, _3);
        _input.register_a("JUDGE_AXIS_1", fa);
    }
    else
    {
        LOG_ERROR << "[Play] Ruleset of 1P not initialized!";
    }

    if (gPlayContext.ruleset[PLAYER_SLOT_2P] != nullptr)
    {
        auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
        _input.register_p("JUDGE_PRESS_2", fp);
        auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
        _input.register_h("JUDGE_HOLD_2", fh);
        auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
        _input.register_r("JUDGE_RELEASE_2", fr);
        auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2, _3);
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
        gPlayContext.bgaTexture->stopUpdate();
        gNextScene = eScene::EXIT_TRANS;
    }

    // 120BPM with 1.0x HS is 2000ms (500ms/beat, green number 1200)
    if (_skin->info.noteLaneHeight1P != 0)
    {
        double bpm = gChartContext.HSFixBPMFactor1P * gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM();
        double minBPM = gChartContext.HSFixBPMFactor1P * gChartContext.minBPM;
        double maxBPM = gChartContext.HSFixBPMFactor1P * gChartContext.maxBPM;
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);

        double visible = 1.0;
        if (_lanecoverEnabled[PLAYER_SLOT_1P])
            visible -= std::clamp(lcTop + lcBottom, 0, 1000) / 1000.0;
        double den;
        den = gPlayContext.Hispeed * bpm;
        gNumbers.queue(eNumber::GREEN_NUMBER_1P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
        den = gPlayContext.Hispeed * maxBPM;
        gNumbers.queue(eNumber::GREEN_NUMBER_MAXBPM_1P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
        den = gPlayContext.Hispeed * minBPM;
        gNumbers.queue(eNumber::GREEN_NUMBER_MINBPM_1P, den != 0.0 ? int(std::round(visible * 120.0 * 1200 / den)) : 0);
    }
    if (_skin->info.noteLaneHeight2P != 0 && gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        double bpm = gChartContext.HSFixBPMFactor2P * gPlayContext.chartObj[PLAYER_SLOT_2P]->getCurrentBPM();
        double minBPM = gChartContext.HSFixBPMFactor2P * gChartContext.minBPM;
        double maxBPM = gChartContext.HSFixBPMFactor2P * gChartContext.maxBPM;
        int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
        int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);

        double visible = 1.0;
        if (_lanecoverEnabled[PLAYER_SLOT_2P])
            visible -= std::clamp(lcTop + lcBottom , 0, 1000) / 1000.0;
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
    if (_lanecoverEnabled[PLAYER_SLOT_1P])
    {
        if (_isHoldingStart[PLAYER_SLOT_1P] || _isHoldingSelect[PLAYER_SLOT_1P])
        {
            gSwitches.queue(eSwitch::P1_SETTING_SPEED, true);
        }
        if (isPlaymodeSinglePlay() &&
            (_isHoldingStart[PLAYER_SLOT_2P] || _isHoldingSelect[PLAYER_SLOT_2P]))
        {
            gSwitches.queue(isPlaymodeSinglePlay() ? eSwitch::P1_SETTING_SPEED : eSwitch::P2_SETTING_SPEED, true);
        }
    }
    if (_lanecoverEnabled[PLAYER_SLOT_2P])
    {
        if (_isHoldingStart[PLAYER_SLOT_2P] || _isHoldingSelect[PLAYER_SLOT_2P])
        {
            gSwitches.queue(eSwitch::P2_SETTING_SPEED, true);
        }
    }
    gSwitches.flush();

    Time t;
    auto updateScratchTimer = [&](int slot)
    {
        if ((t - _scratchLastUpdate[slot]).norm() > 133)
        {
            // release
            if (_scratchDir[slot] != AxisDir::AXIS_NONE)
            {
                if (slot == PLAYER_SLOT_1P)
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
    updateScratchTimer(PLAYER_SLOT_1P);
    updateScratchTimer(PLAYER_SLOT_2P);

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
    gTimers.queue(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentMetre() * 4.0)) % 1000);

    gPlayContext.chartObj[PLAYER_SLOT_1P]->update(rt);
    gPlayContext.ruleset[PLAYER_SLOT_1P]->update(t);

    auto dp1 = gPlayContext.ruleset[PLAYER_SLOT_1P]->getData();
    int miss1 = dp1.miss;
    if (_missPlayer[PLAYER_SLOT_1P] != miss1)
    {
        _missPlayer[PLAYER_SLOT_1P] = miss1;
        _missLastTime = t;
    }

    if (gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr)
    {
        gPlayContext.chartObj[PLAYER_SLOT_2P]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_2P]->update(t);

        auto dp2 = gPlayContext.ruleset[PLAYER_SLOT_2P]->getData();

        gNumbers.queue(eNumber::PLAY_1P_EXSCORE_DIFF, dp1.score2 - dp2.score2);
        gNumbers.queue(eNumber::PLAY_2P_EXSCORE_DIFF, dp2.score2 - dp1.score2);
        gNumbers.queue(eNumber::RESULT_TARGET_EX, dp2.score2);
        gNumbers.queue(eNumber::RESULT_TARGET_DIFF, dp2.score2 - dp1.score2);
        gNumbers.queue(eNumber::RESULT_TARGET_RATE, (int)std::floor(dp2.acc * 100.0));
        gNumbers.queue(eNumber::RESULT_TARGET_RATE_DECIMAL2, (int)std::floor(dp2.acc * 10000.0) % 100);

        int miss2 = dp2.miss;
        if (_missPlayer[PLAYER_SLOT_2P] != miss2)
        {
            _missPlayer[PLAYER_SLOT_2P] = miss2;
            _missLastTime = t;
        }
    }

    gPlayContext.bgaTexture->update(rt, t.norm() - _missLastTime.norm() < _missBgaLength);

    gNumbers.queue(eNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM())));
    // play time / remain time
    {
        auto startTime = rt - gTimers.get(eTimer::PLAY_START);
        auto totalTime = gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm();
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
    if (rt.norm() / 500 >= gPlayContext.graphGauge[PLAYER_SLOT_1P].size())
    {
        auto& g = gPlayContext.graphGauge[PLAYER_SLOT_1P];
        auto& r = gPlayContext.ruleset[PLAYER_SLOT_1P];
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
        if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isFailed() &&
            (gPlayContext.ruleset[PLAYER_SLOT_2P] == nullptr || gPlayContext.ruleset[PLAYER_SLOT_2P]->isFailed()))
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

        if (_isPlayerFinished[PLAYER_SLOT_1P] ^ gPlayContext.ruleset[PLAYER_SLOT_1P]->isFinished())
        {
            _isPlayerFinished[PLAYER_SLOT_1P] = true;
            gTimers.queue(eTimer::PLAY_P1_FINISHED, t.norm());
            if (gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().combo == gPlayContext.chartObj[PLAYER_SLOT_1P]->getNoteCount())
                gTimers.queue(eTimer::PLAY_FULLCOMBO_1P, t.norm());
        }
        if (gPlayContext.ruleset[PLAYER_SLOT_2P] != nullptr && _isPlayerFinished[PLAYER_SLOT_2P] ^ gPlayContext.ruleset[PLAYER_SLOT_2P]->isFinished())
        {
            _isPlayerFinished[PLAYER_SLOT_2P] = true;
            gTimers.queue(eTimer::PLAY_P2_FINISHED, t.norm());
            if (gPlayContext.ruleset[PLAYER_SLOT_2P]->getData().combo == gPlayContext.chartObj[PLAYER_SLOT_2P]->getNoteCount())
                gTimers.queue(eTimer::PLAY_FULLCOMBO_2P, t.norm());
        }
    }

    spinTurntable(true);

    //last note check
    if (rt.hres() - gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().hres() >= 0)
    {
        gTimers.queue(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.queue(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _isExitingFromPlay = true;
        _state = ePlayState::FADEOUT;
    }
     
    gTimers.flush();
    gNumbers.flush();
    gOptions.flush();
    gSliders.flush();
}

void ScenePlay::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);
    if (gChartContext.started)
        gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentMetre() * 4.0)) % 1000);
    spinTurntable(gChartContext.started);
	gPlayContext.bgaTexture->update(rt, false);

    if (ft >= _skin->info.timeOutro)
    {
        if (_isExitingFromPlay)
        {
            removeInputJudgeCallback();

            bool cleared = false;
            if (isPlaymodeBattle())
            {
                if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared() &&
                    gPlayContext.ruleset[PLAYER_SLOT_2P]->isCleared())
                    cleared = true;
            }
            else
            {
                if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared())
                    cleared = true;
            }

            gSwitches.set(eSwitch::RESULT_CLEAR, cleared);
        }

        // save lanecover settings
        ConfigMgr::set('P', cfg::P_LANECOVER_ENABLE, _lanecoverEnabled[PLAYER_SLOT_1P]);
        if (_lanecoverEnabled[PLAYER_SLOT_1P])
        {
            ConfigMgr::set('P', cfg::P_LANECOVER_TOP, gNumbers.get(eNumber::LANECOVER_TOP_1P));
            ConfigMgr::set('P', cfg::P_LANECOVER_BOTTOM, gNumbers.get(eNumber::LANECOVER_BOTTOM_1P));
        }
        ConfigMgr::set('P', cfg::P_LOCK_SPEED, _lockspeedEnabled[PLAYER_SLOT_1P]);
        if (_lockspeedEnabled[PLAYER_SLOT_1P])
        {
            ConfigMgr::set('P', cfg::P_GREENNUMBER, gNumbers.get(eNumber::GREEN_NUMBER_1P));
        }

        if (!isPlaymodeSinglePlay())
        {
            gPlayContext.battle2PLanecover = _lanecoverEnabled[PLAYER_SLOT_2P];
            if (_lanecoverEnabled[PLAYER_SLOT_2P])
            {
                gPlayContext.battle2PLanecoverTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
                gPlayContext.battle2PLanecoverBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
            }
            gPlayContext.battle2PLockSpeed = _lockspeedEnabled[PLAYER_SLOT_2P];
            if (_lockspeedEnabled[PLAYER_SLOT_1P])
            {
                gPlayContext.battle2PGreenNumber = gNumbers.get(eNumber::GREEN_NUMBER_2P);
            }
        }

        // check quick retry (start+select / white+black)
        bool wantRetry = false;
        auto h = _input.Holding();
        using namespace Input;
        if (gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr)
        {
            if ((h.test(K1START) && h.test(K1SELECT)) ||
                (h.test(K11) || h.test(K13) || h.test(K15) || h.test(K17) || h.test(K19)) && (h.test(K12) || h.test(K14) || h.test(K16) || h.test(K18)))
                wantRetry = true;
        }
        if (gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr)
        {
            if ((h.test(K2START) && h.test(K2SELECT)) ||
                (h.test(K21) || h.test(K23) || h.test(K25) || h.test(K27) || h.test(K29)) && (h.test(K22) || h.test(K24) || h.test(K26) || h.test(K28)))
                wantRetry = true;
        }

        SoundMgr::stopNoteSamples();
        gPlayContext.bgaTexture->reset();

        if (isPlaymodeAuto())
        {
            gNextScene = (gPlayContext.isCourse && gChartContext.started) ? eScene::COURSE_TRANS : eScene::SELECT;
        }
        else if (wantRetry && gPlayContext.canRetry)
        {
            gNextScene = eScene::RETRY_TRANS;
        }
        else
        {
            gNextScene = (gPlayContext.isCourse || gChartContext.started) ? eScene::RESULT : eScene::SELECT;
        }
    }
}

void ScenePlay::updateFailed()
{
    auto t = Time();
    auto ft = t - gTimers.get(eTimer::FAIL_BEGIN);
    if (gChartContext.started)
        gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentMetre() * 4.0)) % 1000);
    spinTurntable(gChartContext.started);

    //failed play finished, move to next scene. No fadeout
    if (ft.norm() >= _skin->info.timeFailed)
    {
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _state = ePlayState::FADEOUT;
    }
}


void ScenePlay::procCommonNotes()
{
    assert(gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr);
    auto it = gPlayContext.chartObj[PLAYER_SLOT_1P]->noteBgmExpired.begin();
    size_t max = std::min(_bgmSampleIdxBuf.size(), gPlayContext.chartObj[PLAYER_SLOT_1P]->noteBgmExpired.size());
    size_t i = 0;
    for (; i < max && it != gPlayContext.chartObj[PLAYER_SLOT_1P]->noteBgmExpired.end(); ++i, ++it)
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
        auto it = gPlayContext.chartObj[PLAYER_SLOT_1P]->noteExpired.begin();
        size_t max2 = std::min(_keySampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_1P]->noteExpired.size());
        while (i < max2 && it != gPlayContext.chartObj[PLAYER_SLOT_1P]->noteExpired.end())
        {
            if (it->flags == 0)
            {
                _keySampleIdxBuf[i] = (unsigned)it->dvalue;
                ++i;
            }
            ++it;
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, i, (size_t*)_keySampleIdxBuf.data());
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

	if (isPlaymodeBattle())
	{
		assert(gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr);
		assert(gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr);

		for (size_t i = 0; i < Input::S2L; ++i)
		{
			if (_inputAvailable[i])
				changeKeySample((Input::Pad)i, PLAYER_SLOT_1P);
		}
		for (size_t i = Input::S2L; i < Input::ESC; ++i)
		{
			if (_inputAvailable[i])
				changeKeySample((Input::Pad)i, PLAYER_SLOT_2P);
		}
	}
	else
	{
		assert(gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr);

		for (auto i = 0; i < Input::ESC; ++i)
		{
			if (_inputAvailable[i])
				changeKeySample((Input::Pad)i, PLAYER_SLOT_1P);
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
            aa = int(rt.norm() * 360 / 1000);
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
    if (!gPlayContext.isAuto)
    {
        size_t sampleCount = 0;
        for (size_t i = 0; i < ESC; ++i)
        {
            if (input[i])
            {
                if (_currentKeySample[i])
                    _keySampleIdxBuf[sampleCount++] = _currentKeySample[i];
                gTimers.queue(InputGamePressMapSingle[i].tm, t.norm());
                gTimers.queue(InputGameReleaseMapSingle[i].tm, TIMER_NEVER);
                gSwitches.queue(InputGamePressMapSingle[i].sw, true);
            }
        }
        SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, (size_t*)&_keySampleIdxBuf[0]);

        if (input[S1L] || input[S1R] || isPlaymodeSinglePlay() && (input[S2L] || input[S2R]))
        {
            gTimers.queue(eTimer::S1_DOWN, t.norm());
            gTimers.queue(eTimer::S1_UP, TIMER_NEVER);
            gSwitches.queue(eSwitch::S1_DOWN, true);
        }

        if (!isPlaymodeSinglePlay())
        {
            if (input[S2L] || input[S2R])
            {
                gTimers.queue(eTimer::S2_DOWN, t.norm());
                gTimers.queue(eTimer::S2_UP, TIMER_NEVER);
                gSwitches.queue(eSwitch::S2_DOWN, true);
            }
        }
    }

    // double click START: toggle top lanecover
    if (input[K1START] || isPlaymodeSinglePlay() && input[K2START])
    {
        if (t > _startPressedTime[PLAYER_SLOT_1P] && (t - _startPressedTime[PLAYER_SLOT_1P]).norm() < 200)
        {
            _lanecoverEnabled[PLAYER_SLOT_1P] = !_lanecoverEnabled[PLAYER_SLOT_1P];
            _startPressedTime[PLAYER_SLOT_1P] = TIMER_NEVER;

            if (_lanecoverEnabled[PLAYER_SLOT_1P])
                gSliders.set(eSlider::SUD_1P, gNumbers.get(eNumber::LANECOVER_TOP_1P) / 1000.0);
        }
        else
        {
            _startPressedTime[PLAYER_SLOT_1P] = t;
        }
    }
    if (!isPlaymodeSinglePlay() && input[K2START])
    {
        if (t > _startPressedTime[PLAYER_SLOT_2P] && (t - _startPressedTime[PLAYER_SLOT_2P]).norm() < 200)
        {
            _lanecoverEnabled[PLAYER_SLOT_2P] = !_lanecoverEnabled[PLAYER_SLOT_2P];
            _startPressedTime[PLAYER_SLOT_2P] = TIMER_NEVER;

            if (_lanecoverEnabled[PLAYER_SLOT_2P])
                gSliders.set(eSlider::SUD_2P, gNumbers.get(eNumber::LANECOVER_TOP_2P) / 1000.0);
        }
        else
        {
            _startPressedTime[PLAYER_SLOT_2P] = t;
        }
    }

    // double click SELECT when lanecover enabled: lock green number
    if (_lanecoverEnabled[PLAYER_SLOT_1P] && (input[K1SELECT] || isPlaymodeSinglePlay() && input[K2SELECT]))
    {
        if (t > _selectPressedTime[PLAYER_SLOT_1P] && (t - _selectPressedTime[PLAYER_SLOT_1P]).norm() < 200)
        {
            _lockspeedEnabled[PLAYER_SLOT_1P] = !_lockspeedEnabled[PLAYER_SLOT_1P];
            _selectPressedTime[PLAYER_SLOT_1P] = TIMER_NEVER;

            gSwitches.set(eSwitch::P1_LOCK_SPEED, _lockspeedEnabled[PLAYER_SLOT_1P]);
            if (_lockspeedEnabled[PLAYER_SLOT_1P])
            {
                double bpm = gChartContext.HSFixBPMFactor1P * gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM();
                double hs = gPlayContext.Hispeed;
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
                double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_1P] = (lc == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / lc);
            }
        }
        else
        {
            _selectPressedTime[PLAYER_SLOT_1P] = t;
        }
    }
    if (_lanecoverEnabled[PLAYER_SLOT_2P] && (!isPlaymodeSinglePlay() && input[K2SELECT]))
    {
        if (t > _selectPressedTime[PLAYER_SLOT_2P] && (t - _selectPressedTime[PLAYER_SLOT_2P]).norm() < 200)
        {
            _lockspeedEnabled[PLAYER_SLOT_2P] = !_lockspeedEnabled[PLAYER_SLOT_2P];
            _selectPressedTime[PLAYER_SLOT_2P] = TIMER_NEVER;

            gSwitches.set(eSwitch::P2_LOCK_SPEED, _lockspeedEnabled[PLAYER_SLOT_1P]);
            if (_lockspeedEnabled[PLAYER_SLOT_2P])
            {
                double bpm = gChartContext.HSFixBPMFactor2P * gPlayContext.chartObj[PLAYER_SLOT_2P]->getCurrentBPM();
                double hs = gPlayContext.battle2PHispeed;
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
                double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_2P] = (lc == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / lc);
            }
        }
        else
        {
            _selectPressedTime[PLAYER_SLOT_2P] = t;
        }
    }


    if (true)
    {
        if (input[K1START] || isPlaymodeSinglePlay() && input[K2START]) _isHoldingStart[PLAYER_SLOT_1P] = true;
        if (input[K1SELECT] || isPlaymodeSinglePlay() && input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_1P] = true;

        bool white = (input[K11] || input[K13] || input[K15] || input[K17] || input[K19]) ||
            isPlaymodeSinglePlay() && (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]);
        bool black = (input[K12] || input[K14] || input[K16] || input[K18]) ||
            isPlaymodeSinglePlay() && (input[K22] || input[K24] || input[K26] || input[K28]);

        if (input[K1SPDUP] || isPlaymodeSinglePlay() && input[K2SPDUP] || _isHoldingStart[PLAYER_SLOT_1P] && black)
        {
            if (gPlayContext.Hispeed < 10.0)
            {
                gPlayContext.Hispeed = std::min(gPlayContext.Hispeed + 0.25, 10.0);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
            }
        }

        if (input[K1SPDDN] || isPlaymodeSinglePlay() && input[K2SPDDN] || _isHoldingStart[PLAYER_SLOT_1P] && white)
        {
            if (gPlayContext.Hispeed > 0.25)
            {
                gPlayContext.Hispeed = std::max(gPlayContext.Hispeed - 0.25, 0.25);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
            }
        }

    }
    if (!isPlaymodeSinglePlay())
    {
        if (input[K2START]) _isHoldingStart[PLAYER_SLOT_2P] = true;
        if (input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_2P] = true;

        bool white = (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]);
        bool black = (input[K22] || input[K24] || input[K26] || input[K28]);

        if (input[K2SPDUP] || black)
        {
            if (gPlayContext.battle2PHispeed < 10.0)
            {
                gPlayContext.battle2PHispeed = std::min(gPlayContext.battle2PHispeed + 0.25, 10.0);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
            }
        }
        if (input[K2SPDDN] || white)
        {
            if (gPlayContext.battle2PHispeed > 0.25)
            {
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

            if (isPlaymodeBattle())
            {
                if (!_isPlayerFinished[PLAYER_SLOT_1P])
                {
                    gPlayContext.ruleset[PLAYER_SLOT_1P]->fail();
                }
                if (!_isPlayerFinished[PLAYER_SLOT_2P])
                {
                    gPlayContext.ruleset[PLAYER_SLOT_2P]->fail();
                }
            }
            else
            {
                if (!_isPlayerFinished[PLAYER_SLOT_1P])
                {
                    gPlayContext.ruleset[PLAYER_SLOT_1P]->fail();
                    if (gPlayContext.ruleset[PLAYER_SLOT_2P])
                        gPlayContext.ruleset[PLAYER_SLOT_2P]->fail();
                }
            }

            pushGraphPoints();

        }

        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _state = ePlayState::FADEOUT;
    }
}

// CALLBACK
void ScenePlay::inputGameHold(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // turntable spin
    if (input[S1L]) _ttAngleDiff[PLAYER_SLOT_1P] -= 0.5;
    if (input[S1R]) _ttAngleDiff[PLAYER_SLOT_1P] += 0.5;
    if (isPlaymodeSinglePlay())
    {
        if (input[S2L]) _ttAngleDiff[PLAYER_SLOT_1P] -= 0.5;
        if (input[S2R]) _ttAngleDiff[PLAYER_SLOT_1P] += 0.5;
    }
    else
    {
        if (input[S2L]) _ttAngleDiff[PLAYER_SLOT_2P] -= 0.5;
        if (input[S2R]) _ttAngleDiff[PLAYER_SLOT_2P] += 0.5;
    }

    // lanecover, +200 per second
    int lcThreshold = getRate() / 200;
    if (_lanecoverEnabled[PLAYER_SLOT_1P])
    {
        if (_isHoldingStart[PLAYER_SLOT_1P] || isPlaymodeSinglePlay() && _isHoldingStart[PLAYER_SLOT_2P])
        {
            if (input[S1L])
                _lanecoverAdd[PLAYER_SLOT_1P]--;  // -1 per ms
            if (isPlaymodeSinglePlay() && input[S2L])
                _lanecoverAdd[PLAYER_SLOT_1P]--;  // -1 per ms
            if (input[S1R])
                _lanecoverAdd[PLAYER_SLOT_1P]++;  // +1 per ms
            if (isPlaymodeSinglePlay() && input[S2R])
                _lanecoverAdd[PLAYER_SLOT_1P]++;  // +1 per ms

            // TODO SUD+ or HID+/LIFT?
            int lc = gNumbers.get(eNumber::LANECOVER_TOP_1P);
            while (lc < 1000 && _lanecoverAdd[PLAYER_SLOT_1P] >= lcThreshold)
            {
                _lanecoverAdd[PLAYER_SLOT_1P] -= lcThreshold;
                lc += 1;
            }
            while (lc > 0 && _lanecoverAdd[PLAYER_SLOT_1P] <= -lcThreshold)
            {
                _lanecoverAdd[PLAYER_SLOT_1P] += lcThreshold;
                lc -= 1;
            }
            if (lc <= 0)
            {
                lc = 0;
                if (_lanecoverAdd[PLAYER_SLOT_1P] < 0)
                    _lanecoverAdd[PLAYER_SLOT_1P] = 0;
            }
            else if (lc >= 1000)
            {
                lc = 1000;
                if (_lanecoverAdd[PLAYER_SLOT_1P] > 0)
                    _lanecoverAdd[PLAYER_SLOT_1P] = 0;
            }
            gNumbers.queue(eNumber::LANECOVER_TOP_1P, lc);
            gNumbers.queue(eNumber::LANECOVER100_1P, lc / 10);
            gSliders.queue(eSlider::SUD_1P, lc / 1000.0);

            if (_lockspeedEnabled[PLAYER_SLOT_1P] && (input[S1L] || input[S1R] || (isPlaymodeSinglePlay() && (input[S2L] || input[S2R]))))
            {
                double bpm = gChartContext.HSFixBPMFactor1P * gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM();
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
                double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                gPlayContext.Hispeed = std::min(_lockspeedValue[PLAYER_SLOT_1P] / bpm * lc, 10.0);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.Hispeed * 100));
            }
        }
    }
    else
    {
        gSliders.queue(eSlider::SUD_1P, 0);
    }
    if (!isPlaymodeSinglePlay() && _lanecoverEnabled[PLAYER_SLOT_2P])
    {
        if (_isHoldingStart[PLAYER_SLOT_2P])
        {
            if (input[S2L])
                _lanecoverAdd[PLAYER_SLOT_2P]--;  // -1 per ms
            if (input[S2R])
                _lanecoverAdd[PLAYER_SLOT_2P]++;  // +1 per ms

            int lc = gNumbers.get(eNumber::LANECOVER_TOP_2P);
            while (lc < 1000 && _lanecoverAdd[PLAYER_SLOT_2P] >= 4)
            {
                _lanecoverAdd[PLAYER_SLOT_2P] -= 4;
                lc += 1;
            }
            while (lc > 0 && _lanecoverAdd[PLAYER_SLOT_2P] <= -4)
            {
                _lanecoverAdd[PLAYER_SLOT_2P] += 4;
                lc -= 1;
            }
            if (lc <= 0)
            {
                lc = 0;
                if (_lanecoverAdd[PLAYER_SLOT_2P] < 0)
                    _lanecoverAdd[PLAYER_SLOT_2P] = 0;
            }
            else if (lc >= 1000)
            {
                lc = 1000;
                if (_lanecoverAdd[PLAYER_SLOT_2P] > 0)
                    _lanecoverAdd[PLAYER_SLOT_2P] = 0;
            }
            gNumbers.queue(eNumber::LANECOVER_TOP_2P, lc);
            gNumbers.queue(eNumber::LANECOVER100_2P, lc / 10);
            gSliders.queue(eSlider::SUD_2P, lc / 1000.0);

            if (_lockspeedEnabled[PLAYER_SLOT_2P] && (input[S2L] || input[S2R]))
            {
                double bpm = gChartContext.HSFixBPMFactor2P * gPlayContext.chartObj[PLAYER_SLOT_2P]->getCurrentBPM();
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
                double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                gPlayContext.battle2PHispeed = std::min(_lockspeedValue[PLAYER_SLOT_2P] / bpm * lc, 10.0);
                gNumbers.queue(eNumber::HS_1P, (int)std::round(gPlayContext.battle2PHispeed * 100));
            }
        }
    }
    else
    {
        gSliders.queue(eSlider::SUD_2P, 0);
    }

    // hispeed, +25 per second
    int hsThreshold = getRate() / 25;
    if (true)
    {
        if (_isHoldingSelect[PLAYER_SLOT_1P] || isPlaymodeSinglePlay() && _isHoldingSelect[PLAYER_SLOT_2P])
        {
            if (input[S1L]) 
                _hispeedAdd[PLAYER_SLOT_1P]--;  // -1 per ms
            if (isPlaymodeSinglePlay() && input[S2L]) 
                _hispeedAdd[PLAYER_SLOT_1P]--;  // -1 per ms
            if (input[S1R])
                _hispeedAdd[PLAYER_SLOT_1P]++;  // +1 per ms
            if (isPlaymodeSinglePlay() && input[S2R])
                _hispeedAdd[PLAYER_SLOT_1P]++;  // +1 per ms

            double& hs = gPlayContext.Hispeed;
            while (hs < 10.0 && _hispeedAdd[PLAYER_SLOT_1P] >= hsThreshold)
            {
                _hispeedAdd[PLAYER_SLOT_1P] -= hsThreshold;
                hs += 0.01;
            }
            while (hs > 0.25 && _hispeedAdd[PLAYER_SLOT_1P] <= -hsThreshold)
            {
                _hispeedAdd[PLAYER_SLOT_1P] += hsThreshold;
                hs -= 0.01;
            }
            if (hs <= 0.25)
            {
                hs = 0.25;
                if (_hispeedAdd[PLAYER_SLOT_1P] < 0)
                    _hispeedAdd[PLAYER_SLOT_1P] = 0;
            }
            else if (hs >= 10.0)
            {
                hs = 10.0;
                if (_hispeedAdd[PLAYER_SLOT_1P] > 0)
                    _hispeedAdd[PLAYER_SLOT_1P] = 0;
            }
            gNumbers.queue(eNumber::HS_1P, (int)std::round(hs * 100));

            if (_lockspeedEnabled[PLAYER_SLOT_1P] && (input[S1L] || input[S1R] || (isPlaymodeSinglePlay() && (input[S2L] || input[S2R]))))
            {
                double bpm = gChartContext.HSFixBPMFactor1P * gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM();
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_1P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_1P);
                double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_1P] = (lc == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / lc);
            }
        }
    }
    if (!isPlaymodeSinglePlay())
    {
        if (_isHoldingSelect[PLAYER_SLOT_2P])
        {
            if (input[S2L])
                _hispeedAdd[PLAYER_SLOT_2P]--;  // -1 per ms
            if (input[S2R])
                _hispeedAdd[PLAYER_SLOT_2P]++;  // +1 per ms

            double& hs = gPlayContext.battle2PHispeed;
            while (hs < 10.0 && _hispeedAdd[PLAYER_SLOT_2P] >= hsThreshold)
            {
                _hispeedAdd[PLAYER_SLOT_2P] -= hsThreshold;
                hs += 0.01;
            }
            while (hs > 0.25 && _hispeedAdd[PLAYER_SLOT_2P] <= -hsThreshold)
            {
                _hispeedAdd[PLAYER_SLOT_2P] += hsThreshold;
                hs -= 0.01;
            }
            if (hs <= 0.25)
            {
                hs = 0.25;
                if (_hispeedAdd[PLAYER_SLOT_2P] < 0)
                    _hispeedAdd[PLAYER_SLOT_2P] = 0;
            }
            else if (hs >= 10.0)
            {
                hs = 10.0;
                if (_hispeedAdd[PLAYER_SLOT_2P] > 0)
                    _hispeedAdd[PLAYER_SLOT_2P] = 0;
            }
            gNumbers.queue(eNumber::HS_2P, (int)std::round(hs * 100));

            if (_lockspeedEnabled[PLAYER_SLOT_2P] && (input[S2L] || input[S2R]))
            {
                double bpm = gChartContext.HSFixBPMFactor2P * gPlayContext.chartObj[PLAYER_SLOT_2P]->getCurrentBPM();
                int lcTop = gNumbers.get(eNumber::LANECOVER_TOP_2P);
                int lcBottom = gNumbers.get(eNumber::LANECOVER_BOTTOM_2P);
                double lc = std::max(0, (1000 - lcTop - lcBottom)) / 1000.0;
                _lockspeedValue[PLAYER_SLOT_2P] = (lc == 0.0) ? std::numeric_limits<double>::max() : (bpm * hs / lc);
            }
        }
    }

    gNumbers.flush();
    gSliders.flush();
    gSwitches.flush();
}

// CALLBACK
void ScenePlay::inputGameRelease(InputMask& m, const Time& t)
{
    using namespace Input;
    auto input = _inputAvailable & m;

    if (!gPlayContext.isAuto)
    {
        size_t count = 0;
        for (size_t i = 0; i < Input::ESC; ++i)
            if (input[i])
            {
                gTimers.set(InputGamePressMapSingle[i].tm, TIMER_NEVER);
                gTimers.set(InputGameReleaseMapSingle[i].tm, t.norm());
                gSwitches.set(InputGameReleaseMapSingle[i].sw, false);

                // TODO stop sample playing while release in LN notes
            }

        if (true)
        {
            if (input[S1L] || input[S1R] || isPlaymodeSinglePlay() && (input[S2L] || input[S2R]))
            {
                gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
                gTimers.set(eTimer::S1_UP, t.norm());
                gSwitches.set(eSwitch::S1_DOWN, false);
            }
        }
        if (!isPlaymodeSinglePlay())
        {
            if (input[S2L] || input[S2R])
            {
                gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
                gTimers.set(eTimer::S2_UP, t.norm());
                gSwitches.set(eSwitch::S2_DOWN, false);
            }
        }
    }

    if (input[K1START] || isPlaymodeSinglePlay() && input[K2START]) _isHoldingStart[PLAYER_SLOT_1P] = false;
    if (input[K1SELECT] || isPlaymodeSinglePlay() && input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_1P] = false;
    if (!isPlaymodeSinglePlay())
    {
        if (input[K2START]) _isHoldingStart[PLAYER_SLOT_2P] = false;
        if (input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_2P] = false;
    }
}

// CALLBACK
void ScenePlay::inputGameAxis(double S1, double S2, const Time& t)
{
    using namespace Input;
    std::array<size_t, 4> keySampleIdxBufScratch;
    size_t sampleCount = 0;

    if (true)
    {
        _ttAngleDiff[PLAYER_SLOT_1P] += S1 * 360;

        if (_isHoldingStart[PLAYER_SLOT_1P] && !_isHoldingSelect[PLAYER_SLOT_1P] ||
            isPlaymodeSinglePlay() && _isHoldingStart[PLAYER_SLOT_2P] && !_isHoldingSelect[PLAYER_SLOT_2P])
        {
            // lanecover 1P
            int lanecoverPrev = gNumbers.get(eNumber::LANECOVER_TOP_1P);
            gNumbers.set(eNumber::LANECOVER_TOP_1P, lanecoverPrev + S1);
            gNumbers.set(eNumber::LANECOVER100_1P, int(lanecoverPrev + S1) / 10);

            // ars 1P
        }
    }

    if (!isPlaymodeSinglePlay())
    {
        _ttAngleDiff[PLAYER_SLOT_2P] += S2 * 360;
        if (_isHoldingStart[PLAYER_SLOT_2P] && !_isHoldingSelect[PLAYER_SLOT_2P])
        {
            // lanecover 2P
            int lanecoverPrev = gNumbers.get(eNumber::LANECOVER_TOP_2P);
            gNumbers.set(eNumber::LANECOVER_TOP_2P, lanecoverPrev + S2);
            gNumbers.set(eNumber::LANECOVER100_2P, int(lanecoverPrev + S2) / 10);

            // ars 2P
        }
    }

    auto Scratch = [&](const Time& t, double val, Input::Pad up, Input::Pad dn, int slot)
    {
        if (val * _scratchSpeed[slot] > 0.002)
        {
            // scratch down
            if (_scratchDir[slot] != AxisDir::AXIS_DOWN)
            {
                if (slot == PLAYER_SLOT_1P)
                {
                    gTimers.set(eTimer::S1_DOWN, t.norm());
                    gTimers.set(eTimer::S1_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S1_DOWN, true);
                }
                else
                {
                    gTimers.set(eTimer::S2_DOWN, t.norm());
                    gTimers.set(eTimer::S2_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S2_DOWN, true);
                }
            }

            if (slot == PLAYER_SLOT_1P)
            {
                if (_scratchDir[slot] == AxisDir::AXIS_UP && _currentKeySample[S1L])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S1L];
                if (_scratchDir[slot] == AxisDir::AXIS_DOWN && _currentKeySample[S1R])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S1R];
            }
            else
            {
                if (_scratchDir[slot] == AxisDir::AXIS_UP && _currentKeySample[S2L])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S2L];
                if (_scratchDir[slot] == AxisDir::AXIS_DOWN && _currentKeySample[S2R])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S2R];
            }

            _scratchLastUpdate[slot] = t;
            _scratchDir[slot] = AxisDir::AXIS_DOWN;
        }
        else if (val * _scratchSpeed[slot] < -0.002)
        {
            // scratch up
            if (_scratchDir[slot] != AxisDir::AXIS_UP)
            {
                if (slot == PLAYER_SLOT_1P)
                {
                    gTimers.set(eTimer::S1_DOWN, t.norm());
                    gTimers.set(eTimer::S1_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S1_DOWN, true);
                }
                else
                {
                    gTimers.set(eTimer::S2_DOWN, t.norm());
                    gTimers.set(eTimer::S2_UP, TIMER_NEVER);
                    gSwitches.set(eSwitch::S2_DOWN, true);
                }
            }

            if (slot == PLAYER_SLOT_1P)
            {
                if (_scratchDir[slot] == AxisDir::AXIS_UP && _currentKeySample[S1L])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S1L];
                if (_scratchDir[slot] == AxisDir::AXIS_DOWN && _currentKeySample[S1R])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S1R];
            }
            else
            {
                if (_scratchDir[slot] == AxisDir::AXIS_UP && _currentKeySample[S2L])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S2L];
                if (_scratchDir[slot] == AxisDir::AXIS_DOWN && _currentKeySample[S2R])
                    keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S2R];
            }

            _scratchLastUpdate[slot] = t;
            _scratchDir[slot] = AxisDir::AXIS_UP;
        }
    };
    Scratch(t, S1, S1L, S1R, PLAYER_SLOT_1P);
    Scratch(t, S2, S2L, S2R, PLAYER_SLOT_2P);

    SoundMgr::playNoteSample(SoundChannelType::KEY_LEFT, sampleCount, keySampleIdxBufScratch.data());
}