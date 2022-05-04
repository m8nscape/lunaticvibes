#include <cassert>
#include <future>
#include <set>
#include "scene_play.h"
#include "scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_bms.h"
#include "common/chartformat/chartformat_bms.h"
#include "game/chart/chart_bms.h"
#include "game/graphics/sprite_video.h"
#include "config/config_mgr.h"
#include "common/log.h"
#include "common/sysutil.h"
#include "game/sound/sound_sample.h"

bool ScenePlay::isPlaymodeSingle() const
{
    return !(_playmode == ePlayMode::LOCAL_BATTLE || _playmode == ePlayMode::AUTO_BATTLE);
}
bool ScenePlay::isPlaymodeBattle() const
{
    return !isPlaymodeSingle();
}
bool ScenePlay::isPlaymodeAuto() const
{
    return (_playmode == ePlayMode::AUTO || _playmode == ePlayMode::AUTO_BATTLE);
}

ScenePlay::ScenePlay(): vScene(gPlayContext.mode, 1000, true)
{
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

    _inputAvailable = INPUT_MASK_FUNC;
    _inputAvailable |= INPUT_MASK_1P | INPUT_MASK_2P;

    // file loading may delayed

    _state = ePlayState::PREPARE;

	if (gChartContext.chartObj == nullptr)
	{

	}

	// global info

    // basic info
    gTexts.set(eText::PLAY_TITLE, gChartContext.title);
    gTexts.set(eText::PLAY_SUBTITLE, gChartContext.title2);
    gTexts.set(eText::PLAY_ARTIST, gChartContext.artist);
    gTexts.set(eText::PLAY_SUBARTIST, gChartContext.artist2);
    gTexts.set(eText::PLAY_GENRE, gChartContext.genre);
    gNumbers.set(eNumber::PLAY_BPM, int(std::round(gChartContext.startBPM)));
    gNumbers.set(eNumber::BPM_MIN, int(std::round(gChartContext.minBPM)));
    gNumbers.set(eNumber::BPM_MAX, int(std::round(gChartContext.maxBPM)));

    // player datas

    // set gauge type
    if (gChartContext.chartObj)
        switch (gChartContext.chartObj->type())
        {
        case eChartFormat::BMS:
        case eChartFormat::BMSON:
            setTempInitialHealthBMS();
            break;
        default:
            break;
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

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&ScenePlay::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&ScenePlay::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&ScenePlay::inputGameRelease, this, _1, _2));
    _input.register_a("SCENE_AXIS", std::bind(&ScenePlay::inputGameAxis, this, _1, _2));

    loopStart();
    _input.loopStart();
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
            gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 20);
            break;

        case eModGauge::HARD:
        case eModGauge::DEATH:
        case eModGauge::PATTACK:
        case eModGauge::GATTACK:
        case eModGauge::EXHARD:
            gPlayContext.initialHealth[PLAYER_SLOT_1P] = 1.0;
            gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 100);
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
                gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 20);
                break;

            case eModGauge::HARD:
            case eModGauge::DEATH:
            case eModGauge::PATTACK:
            case eModGauge::GATTACK:
            case eModGauge::EXHARD:
                gPlayContext.initialHealth[PLAYER_SLOT_2P] = 1.0;
                gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 100);
                break;

            default: break;
            }
        }
    }
    else
    {
        gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, (int)gPlayContext.initialHealth[0]);

        if (isPlaymodeBattle())
        {
            gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, (int)gPlayContext.initialHealth[1]);
        }
    }

}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::loadChart()
{
    if (gChartContext.chartObj == nullptr || !gChartContext.chartObj->isLoaded())
    {
        if (gChartContext.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            return;
        }

        gChartContext.chartObj = vChartFormat::getFromFile(gChartContext.path);
    }

    if (gChartContext.chartObj == nullptr || !gChartContext.chartObj->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart: " << gChartContext.path.string();
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

        if (isPlaymodeBattle())
        {
            gPlayContext.chartObj[PLAYER_SLOT_1P] = std::make_shared<chartBMS>(PLAYER_SLOT_1P, bms);
            gPlayContext.chartObj[PLAYER_SLOT_2P] = std::make_shared<chartBMS>(PLAYER_SLOT_2P, bms);
        }
        else
        {
            gPlayContext.chartObj[PLAYER_SLOT_1P] = std::make_shared<chartBMS>(PLAYER_SLOT_1P, bms);
        }
        _chartLoaded = true;
        gNumbers.set(eNumber::PLAY_REMAIN_MIN, int(gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() / 1000 / 60));
        gNumbers.set(eNumber::PLAY_REMAIN_SEC, int(gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() / 1000 % 60));
        break;
    }

    case eChartFormat::BMSON:
    default:
		LOG_WARNING << "[Play] chart format not supported.";
        return;
    }

    gPlayContext.remainTime = gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength();

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

        if (keys == 10 || keys == 14)
        {
            gPlayContext.ruleset[PLAYER_SLOT_1P] = std::make_shared<RulesetBMS>(
                gChartContext.chartObj, gPlayContext.chartObj[PLAYER_SLOT_1P],
                gPlayContext.mods[PLAYER_SLOT_1P].gauge, keys, judgeDiff, 
                gPlayContext.initialHealth[PLAYER_SLOT_1P], RulesetBMS::PlaySide::DP);
        }
        else if (isPlaymodeBattle())
        {
            gPlayContext.ruleset[PLAYER_SLOT_1P] = std::make_shared<RulesetBMS>(
                gChartContext.chartObj,  gPlayContext.chartObj[PLAYER_SLOT_1P],
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
                gPlayContext.initialHealth[PLAYER_SLOT_1P], RulesetBMS::PlaySide::SP);
        }

        if (isPlaymodeAuto())
        {
            gPlayContext.ruleset[PLAYER_SLOT_1P]->setAutoplay(true);
            if (isPlaymodeBattle())
                gPlayContext.ruleset[PLAYER_SLOT_2P]->setAutoplay(true);

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
            SetThreadName("Chart sound sample loading thread");
            SoundMgr::freeKeySamples();

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
				Path pWav(wav);
				if (pWav.is_absolute())
					SoundMgr::loadKeySample(pWav, i);
				else
					SoundMgr::loadKeySample((chartDir / wav), i);
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
            SetThreadName("Chart BGA loading thread");
            gPlayContext.bgaTexture = std::make_shared<TextureBmsBga>();

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

            for (size_t i = 0; i < _pChart->bgaFiles.size(); ++i)
            {
				if (sceneEnding) return;
                const auto& bmp = _pChart->bgaFiles[i];
                if (bmp.empty()) continue;


				Path pBmp(bmp);
				if (pBmp.is_absolute())
					gPlayContext.bgaTexture->addBmp(i, pBmp);
				else
					gPlayContext.bgaTexture->addBmp(i, chartDir / pBmp);

				/*
				if (fs::exists(bmp) && fs::is_regular_file(bmp) && pBmp.has_extension())
				{
					if (video_file_extensions.find(toLower(pBmp.extension().string())) != video_file_extensions.end())
					{
						if (int vi; (vi = getVideoSlot()) >= 0)
						{
							_video[vi].setVideo(pBmp);
							auto pv = std::make_shared<sVideo>(_video[vi]);
							_bgaIdxBuf[i] = pv;
							for (auto& bv : _bgaVideoSprites[i])
								bv->bindVideo(pv);
						}
					}
					else
					{
						_bgaIdxBuf[i] = std::make_shared<Texture>(Image(bmp.c_str()));
					}
				}
				*/

                ++_bmpLoaded;
            }
			gPlayContext.bgaTexture->setSlotFromBMS(*std::reinterpret_pointer_cast<chartBMS>(gPlayContext.chartObj[PLAYER_SLOT_1P]));
            gChartContext.isBgaLoaded = true;
        });
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (isPlaymodeBattle())
    {
        if (gPlayContext.ruleset[PLAYER_SLOT_1P] != nullptr)
        {
            auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_p("JUDGE_PRESS", fp);
            auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_h("JUDGE_HOLD", fh);
            auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_r("JUDGE_RELEASE", fr);
            auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_a("JUDGE_AXIS", fa);
        }
        else
            LOG_ERROR << "[Play] Ruleset of 1P not initialized!";

        if (gPlayContext.ruleset[PLAYER_SLOT_2P] != nullptr)
        {
            auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_p("JUDGE_PRESS", fp);
            auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_h("JUDGE_HOLD", fh);
            auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_r("JUDGE_RELEASE", fr);
            auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_a("JUDGE_AXIS", fa);
        }
        else
            LOG_ERROR << "[Play] Ruleset of 2P not initialized!";
    }
    else // SINGLE or MULTI
    {
        if (gPlayContext.ruleset[PLAYER_SLOT_1P] != nullptr)
        {
            auto fp = std::bind(&vRuleset::updatePress, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_p("JUDGE_PRESS", fp);
            auto fh = std::bind(&vRuleset::updateHold, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_h("JUDGE_HOLD", fh);
            auto fr = std::bind(&vRuleset::updateRelease, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_r("JUDGE_RELEASE", fr);
            auto fa = std::bind(&vRuleset::updateAxis, gPlayContext.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_a("JUDGE_AXIS", fa);
        }
        else
            LOG_ERROR << "[Play] Ruleset not initialized!";
    }
}

void ScenePlay::removeInputJudgeCallback()
{
    for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
    {
        _input.unregister_p("JUDGE_PRESS");
        _input.unregister_h("JUDGE_HOLD");
        _input.unregister_r("JUDGE_RELEASE");
        _input.unregister_a("JUDGE_AXIS");
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::_updateAsync()
{
	gNumbers.set(eNumber::SCENE_UPDATE_FPS, getRate());
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

void pushGraphPoints()
{
    gPlayContext.graphGauge[PLAYER_SLOT_1P].push_back(gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().health * 100);

    gPlayContext.graphScore[PLAYER_SLOT_1P].push_back(gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().score2);

    if (gPlayContext.ruleset[PLAYER_SLOT_2P])
        gPlayContext.graphScore[PLAYER_SLOT_2P].push_back(gPlayContext.ruleset[PLAYER_SLOT_2P]->getData().score2);

    gPlayContext.graphScoreTarget.push_back(static_cast<int>(std::floor(
        gPlayContext.ruleset[PLAYER_SLOT_1P]->getCurrentMaxScore() * (0.01 * gNumbers.get(eNumber::DEFAULT_TARGET_RATE)))));
}

void ScenePlay::updatePlaying()
{
	auto t = Time();
	auto rt = t - gTimers.get(eTimer::PLAY_START);
    gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBeat() * 4.0)) % 1000);
    gPlayContext.bgaTexture->update(rt, false);

    if (isPlaymodeBattle())
    {
        gPlayContext.chartObj[PLAYER_SLOT_1P]->update(rt);
        gPlayContext.chartObj[PLAYER_SLOT_2P]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_1P]->update(t);
        gPlayContext.ruleset[PLAYER_SLOT_2P]->update(t);
    }
    else
    {
        gPlayContext.chartObj[PLAYER_SLOT_1P]->update(rt);
        gPlayContext.ruleset[PLAYER_SLOT_1P]->update(t);
    }

    gNumbers.set(eNumber::PLAY_BPM, int(std::round(gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBPM())));
    // play time / remain time
    {
        auto startTime = rt - gTimers.get(eTimer::PLAY_START);
        auto playtime = rt.norm() / 1000;
        auto remaintime = gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() - playtime;
        gNumbers.set(eNumber::PLAY_MIN, int(playtime / 60));
        gNumbers.set(eNumber::PLAY_SEC, int(playtime % 60));
        gNumbers.set(eNumber::PLAY_REMAIN_MIN, int(remaintime / 60));
        gNumbers.set(eNumber::PLAY_REMAIN_SEC, int(remaintime % 60));
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
        if (isPlaymodeSingle())
        {
            //if (context_play.health[context_play.playerSlot] <= 0)
            if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isFailed())
            {
                pushGraphPoints();
                gTimers.set(eTimer::FAIL_BEGIN, t.norm());
                gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FAILED);
                _isExitingFromPlay = true;
                _state = ePlayState::FAILED;
                SoundMgr::stopSamples();
                SoundMgr::stopKeySamples();
                SoundMgr::playSample(eSoundSample::SOUND_PLAYSTOP);
                for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
                {
                    _input.unregister_p("SCENE_PRESS");
                }
                LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
            }

            if (_isPlayerFinished[PLAYER_SLOT_1P] ^ gPlayContext.ruleset[PLAYER_SLOT_1P]->isFinished())
            {
                _isPlayerFinished[PLAYER_SLOT_1P] = true;
                gTimers.set(eTimer::PLAY_P1_FINISHED, t.norm());
                if (gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().combo == gPlayContext.chartObj[PLAYER_SLOT_1P]->getNoteCount())
                    gTimers.set(eTimer::PLAY_FULLCOMBO_1P, t.norm());
            }
        }

        if (isPlaymodeBattle())
        {
            if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isFailed() &&
                gPlayContext.ruleset[PLAYER_SLOT_2P]->isFailed())
            {
                pushGraphPoints();
                gTimers.set(eTimer::FAIL_BEGIN, t.norm());
                gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FAILED);
                _isExitingFromPlay = true;
                _state = ePlayState::FAILED;
                SoundMgr::stopSamples();
                SoundMgr::stopKeySamples();
                SoundMgr::playSample(eSoundSample::SOUND_PLAYSTOP);
                for (size_t i = 0; i < gPlayContext.ruleset.size(); ++i)
                {
                    _input.unregister_p("SCENE_PRESS");
                }
                LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
            }

            if (_isPlayerFinished[PLAYER_SLOT_1P] ^ gPlayContext.ruleset[PLAYER_SLOT_1P]->isFinished())
            {
                _isPlayerFinished[PLAYER_SLOT_1P] = true;
                gTimers.set(eTimer::PLAY_P1_FINISHED, t.norm());
                if (gPlayContext.ruleset[PLAYER_SLOT_1P]->getData().combo == gPlayContext.chartObj[PLAYER_SLOT_1P]->getNoteCount())
                    gTimers.set(eTimer::PLAY_FULLCOMBO_1P, t.norm());
            }
            if (_isPlayerFinished[PLAYER_SLOT_2P] ^ gPlayContext.ruleset[PLAYER_SLOT_2P]->isFinished())
            {
                _isPlayerFinished[PLAYER_SLOT_2P] = true;
                gTimers.set(eTimer::PLAY_P2_FINISHED, t.norm());
                if (gPlayContext.ruleset[PLAYER_SLOT_2P]->getData().combo == gPlayContext.chartObj[PLAYER_SLOT_2P]->getNoteCount())
                    gTimers.set(eTimer::PLAY_FULLCOMBO_2P, t.norm());
            }
        }
    }

    spinTurntable(true);

    //last note check
    if (rt.norm() - gPlayContext.chartObj[PLAYER_SLOT_1P]->getTotalLength().norm() >= 0)
    {
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        _isExitingFromPlay = true;
        _state = ePlayState::FADEOUT;
    }
     
}

void ScenePlay::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);
    if (gChartContext.started)
        gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBeat() * 4.0)) % 1000);
    spinTurntable(gChartContext.started);
	gPlayContext.bgaTexture->update(rt, false);

    if (_isExitingFromPlay)
    {
        removeInputJudgeCallback();

        bool cleared = false;
        if (isPlaymodeSingle())
            if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared())
                cleared = true;

        if (isPlaymodeBattle())
            if (gPlayContext.ruleset[PLAYER_SLOT_1P]->isCleared() &&
                gPlayContext.ruleset[PLAYER_SLOT_2P]->isCleared())
                cleared = true;

        gSwitches.set(eSwitch::RESULT_CLEAR, cleared);
    }

    if (ft >= _skin->info.timeOutro)
    {
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

        loopEnd();
        _input.loopEnd();
        SoundMgr::stopKeySamples();

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
        gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (gPlayContext.chartObj[PLAYER_SLOT_1P]->getCurrentBeat() * 4.0)) % 1000);
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
            _bgmSampleIdxBuf[i] = (unsigned)std::get<long long>(it->value);
        }
    }

    // also play keysound in auto
    if (gPlayContext.isAuto)
    {
        auto it = gPlayContext.chartObj[PLAYER_SLOT_1P]->noteExpired.begin();
        size_t max2 = std::min(_bgmSampleIdxBuf.size(), max + gPlayContext.chartObj[PLAYER_SLOT_1P]->noteExpired.size());
        while (i < max && it != gPlayContext.chartObj[PLAYER_SLOT_1P]->noteExpired.end())
        {
            if (it->flags & Note::LN_TAIL == 0)
            {
                _bgmSampleIdxBuf[i] = (unsigned)std::get<long long>(it->value);
                ++i;
            }
            ++it;
        }
    }

    SoundMgr::playKeySample(i, (size_t*)_bgmSampleIdxBuf.data());
}

void ScenePlay::changeKeySampleMapping(const Time& t)
{
    static const Time MIN_REMAP_INTERVAL{ 1000 };
    if (isPlaymodeBattle())
    {
        for (size_t i = 0; i < Input::S2L; ++i)
            if (_inputAvailable[i])
            {
                assert(gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr);
                auto chan = gPlayContext.chartObj[PLAYER_SLOT_1P]->getLaneFromKey((Input::Pad)i);
                if (chan.first == chart::NoteLaneCategory::_) continue;
                auto note = gPlayContext.chartObj[PLAYER_SLOT_1P]->incomingNote(chan.first, chan.second);
                if (note->time - t > MIN_REMAP_INTERVAL) continue;
                _currentKeySample[i] = (size_t)std::get<long long>(note->value);
                if (i == Input::S1L)
                    _currentKeySample[Input::S1R] = (size_t)std::get<long long>(note->value);
            }
        for (size_t i = Input::S2L; i < Input::ESC; ++i)
            if (_inputAvailable[i])
            {
                assert(gPlayContext.chartObj[PLAYER_SLOT_2P] != nullptr);
                auto chan = gPlayContext.chartObj[PLAYER_SLOT_2P]->getLaneFromKey((Input::Pad)i);
                if (chan.first == chart::NoteLaneCategory::_) continue;
                auto note = gPlayContext.chartObj[PLAYER_SLOT_2P]->incomingNote(chan.first, chan.second);
                if (note->time - t > MIN_REMAP_INTERVAL) continue;
                _currentKeySample[i] = (size_t)std::get<long long>(note->value);
                if (i == Input::S2L)
                    _currentKeySample[Input::S2R] = (size_t)std::get<long long>(note->value);
            }
    }
    else
    {
        for (auto i = 0; i < Input::ESC; ++i)
            if (_inputAvailable[i])
            {
                assert(gPlayContext.chartObj[PLAYER_SLOT_1P] != nullptr);
                auto chan = gPlayContext.chartObj[PLAYER_SLOT_1P]->getLaneFromKey((Input::Pad)i);
                if (chan.first == chart::NoteLaneCategory::_) continue;
                auto note = gPlayContext.chartObj[PLAYER_SLOT_1P]->incomingNote(chan.first, chan.second);
                if (note->time - t > MIN_REMAP_INTERVAL) continue;
                _currentKeySample[i] = (size_t)std::get<long long>(note->value);
                if (i == Input::S1L)
                    _currentKeySample[Input::S1R] = (size_t)std::get<long long>(note->value);
            }
    }
}

/*
void ScenePlay::updateBga()
{
	// base
	if (std::holds_alternative<pVideo>(_bgaIdxBuf[bgaBaseIdx]))
	{
		auto pv = std::get<pVideo>(_bgaIdxBuf[bgaBaseIdx]);
		bgaBaseTexture = nullptr;
		pv->startPlaying();
	}
	else if (std::holds_alternative<pTexture>(_bgaIdxBuf[bgaBaseIdx]))
	{
		bgaBaseTexture = std::get<pTexture>(_bgaIdxBuf[bgaBaseIdx]);
	}
	else
	{
		bgaBaseTexture = nullptr;
	}

	// layer
	if (std::holds_alternative<pVideo>(_bgaIdxBuf[bgaLayerIdx]))
	{
		auto pv = std::get<pVideo>(_bgaIdxBuf[bgaLayerIdx]);
		bgaLayerTexture = nullptr;
		pv->startPlaying();
	}
	else if (std::holds_alternative<pTexture>(_bgaIdxBuf[bgaLayerIdx]))
	{
		bgaLayerTexture = std::get<pTexture>(_bgaIdxBuf[bgaLayerIdx]);
	}
	else
	{
		bgaLayerTexture = nullptr;
	}

	// poor
	if (std::holds_alternative<pVideo>(_bgaIdxBuf[bgaPoorIdx]))
	{
		auto pv = std::get<pVideo>(_bgaIdxBuf[bgaPoorIdx]);
		bgaPoorTexture = nullptr;
		pv->startPlaying();
	}
	else if (std::holds_alternative<pTexture>(_bgaIdxBuf[bgaPoorIdx]))
	{
		bgaPoorTexture = std::get<pTexture>(_bgaIdxBuf[bgaPoorIdx]);
	}
	else
	{
		bgaPoorTexture = nullptr;
	}
}
*/

void ScenePlay::spinTurntable(bool startedPlaying)
{
    if (startedPlaying)
    {
        auto t = Time();
        auto rt = t - gTimers.get(eTimer::PLAY_START);
        for (auto& aa : _ttAngleDiff)
            aa += int(rt.norm() * 180 / 1000);
    }
    for (auto& aa : _ttAngleDiff)
        aa %= 360;
    gNumbers.set(eNumber::_ANGLE_TT_1P, _ttAngleDiff[0]);
    gNumbers.set(eNumber::_ANGLE_TT_2P, _ttAngleDiff[1]);
}

////////////////////////////////////////////////////////////////////////////////
// CALLBACK
void ScenePlay::inputGamePress(InputMask& m, const Time& t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // individual keys
    size_t sampleCount = 0;
    for (size_t i = 0; i < ESC; ++i)
    {
        if (input[i])
        {
            if (_currentKeySample[i])
                _keySampleIdxBuf[sampleCount++] = _currentKeySample[i];
            gTimers.set(InputGamePressMapSingle[i].tm, t.norm());
            gTimers.set(InputGameReleaseMapSingle[i].tm, TIMER_NEVER);
            gSwitches.set(InputGamePressMapSingle[i].sw, true);
        }
    }
    SoundMgr::playKeySample(sampleCount, (size_t*)&_keySampleIdxBuf[0]);

    if (true)
    {
        if (input[S1L] || input[S1R] || isPlaymodeSingle() && (input[S2L] || input[S2R]))
        {
            gTimers.set(eTimer::S1_DOWN, t.norm());
            gTimers.set(eTimer::S1_UP, TIMER_NEVER);
            gSwitches.set(eSwitch::S1_DOWN, true);
        }

        if (input[K1START] || isPlaymodeSingle() && input[K2START]) _isHoldingStart[PLAYER_SLOT_1P] = true;
        if (input[K1SELECT] || isPlaymodeSingle() && input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_1P] = true;

        bool white = (input[K11] || input[K13] || input[K15] || input[K17] || input[K19]) ||
            isPlaymodeSingle() && (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]);
        bool black = (input[K12] || input[K14] || input[K16] || input[K18]) ||
            isPlaymodeSingle() && (input[K22] || input[K24] || input[K26] || input[K28]);

        if (input[K1SPDUP] || isPlaymodeSingle() && input[K2SPDUP] || _isHoldingStart[PLAYER_SLOT_1P] && black)
        {
            int hs = gNumbers.get(eNumber::HS_1P);
            if (hs < 900)
                gNumbers.set(eNumber::HS_1P, hs + 25);
        }

        if (input[K1SPDDN] || isPlaymodeSingle() && input[K2SPDDN] || _isHoldingStart[PLAYER_SLOT_1P] && white)
        {
            int hs = gNumbers.get(eNumber::HS_1P);
            if (hs > 25)
                gNumbers.set(eNumber::HS_1P, hs - 25);
        }

    }
    if (!isPlaymodeSingle())
    {
        if (input[S2L] || input[S2R])
        {
            gTimers.set(eTimer::S2_DOWN, t.norm());
            gTimers.set(eTimer::S2_UP, TIMER_NEVER);
            gSwitches.set(eSwitch::S2_DOWN, true);
        }

        if (input[K2START]) _isHoldingStart[PLAYER_SLOT_2P] = true;
        if (input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_2P] = true;

        bool white = (input[K21] || input[K23] || input[K25] || input[K27] || input[K29]);
        bool black = (input[K22] || input[K24] || input[K26] || input[K28]);

        if (input[K2SPDUP] || black)
        {
            int hs = gNumbers.get(eNumber::HS_2P);
            if (hs < 900)
                gNumbers.set(eNumber::HS_2P, hs + 25);
        }
        if (input[K2SPDDN] || white)
        {
            int hs = gNumbers.get(eNumber::HS_2P);
            if (hs > 25)
                gNumbers.set(eNumber::HS_2P, hs - 25);
        }

    }

    if (input[Input::ESC] || (input[Input::K1START] && input[Input::K1SELECT]) || (input[Input::K2START] && input[Input::K2SELECT]))
    {
        if (gChartContext.started)
        {
            _isExitingFromPlay = true;

            if (isPlaymodeSingle())
            {
                if (!_isPlayerFinished[PLAYER_SLOT_1P])
                {
                    gPlayContext.ruleset[PLAYER_SLOT_1P]->fail();
                }
            }
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
	for (size_t i = 0; i < ESC; ++i)
	{
		// TODO analog spin speed
		switch (i)
		{
		case S1L: _ttAngleDiff[PLAYER_SLOT_1P] -= 5; break;
		case S1R: _ttAngleDiff[PLAYER_SLOT_1P] += 5; break;
		case S2L: _ttAngleDiff[PLAYER_SLOT_2P] -= 5; break;
		case S2R: _ttAngleDiff[PLAYER_SLOT_2P] += 5; break;
		default: break;
		}
	}
}

// CALLBACK
void ScenePlay::inputGameRelease(InputMask& m, const Time& t)
{
    using namespace Input;
    auto input = _inputAvailable & m;

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
        if (input[S1L] || input[S1R] || isPlaymodeSingle() && (input[S2L] || input[S2R]))
        {
            gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
            gTimers.set(eTimer::S1_UP, t.norm());
            gSwitches.set(eSwitch::S1_DOWN, false);
        }
        if (input[K1START] || isPlaymodeSingle() && input[K2START]) _isHoldingStart[PLAYER_SLOT_1P] = false;
        if (input[K1SELECT] || isPlaymodeSingle() && input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_1P] = false;
    }
    if (!isPlaymodeSingle())
    {
        if (input[S2L] || input[S2R])
        {
            gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
            gTimers.set(eTimer::S2_UP, t.norm());
            gSwitches.set(eSwitch::S2_DOWN, false);
        }
        if (input[K2START]) _isHoldingStart[PLAYER_SLOT_2P] = false;
        if (input[K2SELECT]) _isHoldingSelect[PLAYER_SLOT_2P] = false;
    }
}

// CALLBACK
void ScenePlay::inputGameAxis(InputAxisPlus& m, const Time& t)
{
    using namespace Input;
    std::array<size_t, 4> keySampleIdxBufScratch;
    size_t sampleCount = 0;

    double S1 = -m[S1L].first + m[S1R].first;
    if (true)
    {
        _ttAngleDiff[PLAYER_SLOT_1P] += S1 * 360;
        if (_isHoldingStart[PLAYER_SLOT_1P] && !_isHoldingSelect[PLAYER_SLOT_1P] ||
            isPlaymodeSingle() && _isHoldingStart[PLAYER_SLOT_2P] && !_isHoldingSelect[PLAYER_SLOT_2P])
        {
            // lanecover 1P
            int lanecoverPrev = gNumbers.get(eNumber::LANECOVER_1P);
            gNumbers.set(eNumber::LANECOVER_1P, lanecoverPrev + S1);

            // ars 1P
        }
    }

    double S2 = -m[S2L].first + m[S2R].first;
    if (!isPlaymodeSingle())
    {
        _ttAngleDiff[PLAYER_SLOT_2P] += S2 * 360;
        if (_isHoldingStart[PLAYER_SLOT_2P] && !_isHoldingSelect[PLAYER_SLOT_2P])
        {
            // lanecover 2P
            int lanecoverPrev = gNumbers.get(eNumber::LANECOVER_2P);
            gNumbers.set(eNumber::LANECOVER_2P, lanecoverPrev + S2);

            // ars 2P
        }
    }

    double minSpeed = InputMgr::getAxisMinSpeed();

    AxisDir dir(S1, minSpeed);
    if (dir != AxisDir::AXIS_NONE)
    {
        _ttAxisLastUpdate[PLAYER_SLOT_1P] = t;
    }
    if (dir != AxisDir::AXIS_NONE && dir != _ttAxisDir[PLAYER_SLOT_1P])
    {
        gTimers.set(eTimer::S1_DOWN, t.norm());
        gTimers.set(eTimer::S1_UP, TIMER_NEVER);
        gSwitches.set(eSwitch::S1_DOWN, true);
        _ttAxisDir[PLAYER_SLOT_1P] = dir;

        if (dir == AxisDir::AXIS_UP && _currentKeySample[S1L])
            keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S1L];
        if (dir == AxisDir::AXIS_DOWN && _currentKeySample[S1R])
            keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S1R];
    }
    else if ((t - _ttAxisLastUpdate[PLAYER_SLOT_1P]).norm() > 133)
    {
        gTimers.set(eTimer::S1_DOWN, TIMER_NEVER);
        gTimers.set(eTimer::S1_UP, t.norm());
        gSwitches.set(eSwitch::S1_DOWN, false);
        _ttAxisDir[PLAYER_SLOT_1P] = 0;
        _ttAxisLastUpdate[PLAYER_SLOT_1P] = TIMER_NEVER;
    }

    dir = AxisDir(S2, minSpeed);
    if (dir != AxisDir::AXIS_NONE)
    {
        _ttAxisLastUpdate[PLAYER_SLOT_2P] = t;
    }
    if (dir != AxisDir::AXIS_NONE && dir != _ttAxisDir[PLAYER_SLOT_2P])
    {
        gTimers.set(eTimer::S2_DOWN, t.norm());
        gTimers.set(eTimer::S2_UP, TIMER_NEVER);
        gSwitches.set(eSwitch::S2_DOWN, true);
        _ttAxisDir[PLAYER_SLOT_2P] = dir;

        if (dir == AxisDir::AXIS_UP && _currentKeySample[S2L])
            keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S2L];
        if (dir == AxisDir::AXIS_DOWN && _currentKeySample[S2R])
            keySampleIdxBufScratch[sampleCount++] = _currentKeySample[S2R];
    }
    else if ((t - _ttAxisLastUpdate[PLAYER_SLOT_2P]).norm() > 133)
    {
        gTimers.set(eTimer::S2_DOWN, TIMER_NEVER);
        gTimers.set(eTimer::S2_UP, t.norm());
        gSwitches.set(eSwitch::S2_DOWN, false);
        _ttAxisDir[PLAYER_SLOT_2P] = 0;
        _ttAxisLastUpdate[PLAYER_SLOT_2P] = TIMER_NEVER;
    }

    SoundMgr::playKeySample(sampleCount, keySampleIdxBufScratch.data());
}