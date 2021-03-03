#include <cassert>
#include <future>
#include <set>
#include "scene_play.h"
#include "scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_classic.h"
#include "chartformat/format_bms.h"
#include "game/chart/chart_bms.h"
#include "game/graphics/sprite_video.h"
#include "config/config_mgr.h"
#include <plog/Log.h>

struct InputDataMap{
    eTimer tm;
    eSwitch sw;
};

static const InputDataMap InputGamePressMap[] =
{
    { eTimer::S1L_DOWN, eSwitch::S1L_DOWN },
    { eTimer::S1R_DOWN, eSwitch::S1R_DOWN },
    { eTimer::K11_DOWN, eSwitch::K11_DOWN },
    { eTimer::K12_DOWN, eSwitch::K12_DOWN },
    { eTimer::K13_DOWN, eSwitch::K13_DOWN },
    { eTimer::K14_DOWN, eSwitch::K14_DOWN },
    { eTimer::K15_DOWN, eSwitch::K15_DOWN },
    { eTimer::K16_DOWN, eSwitch::K16_DOWN },
    { eTimer::K17_DOWN, eSwitch::K17_DOWN },
    { eTimer::K18_DOWN, eSwitch::K18_DOWN },
    { eTimer::K19_DOWN, eSwitch::K19_DOWN },
    { eTimer::K1START_DOWN, eSwitch::K1START_DOWN },
    { eTimer::K1SELECT_DOWN, eSwitch::K1SELECT_DOWN },
    { eTimer::K1SPDUP_DOWN, eSwitch::K1SPDUP_DOWN },
    { eTimer::K1SPDDN_DOWN, eSwitch::K1SPDDN_DOWN },
    { eTimer::S2L_DOWN, eSwitch::S2L_DOWN },
    { eTimer::S2R_DOWN, eSwitch::S2R_DOWN },
    { eTimer::K21_DOWN, eSwitch::K21_DOWN },
    { eTimer::K22_DOWN, eSwitch::K22_DOWN },
    { eTimer::K23_DOWN, eSwitch::K23_DOWN },
    { eTimer::K24_DOWN, eSwitch::K24_DOWN },
    { eTimer::K25_DOWN, eSwitch::K25_DOWN },
    { eTimer::K26_DOWN, eSwitch::K26_DOWN },
    { eTimer::K27_DOWN, eSwitch::K27_DOWN },
    { eTimer::K28_DOWN, eSwitch::K28_DOWN },
    { eTimer::K29_DOWN, eSwitch::K29_DOWN },
    { eTimer::K2START_DOWN, eSwitch::K2START_DOWN },
    { eTimer::K2SELECT_DOWN, eSwitch::K2SELECT_DOWN },
    { eTimer::K2SPDUP_DOWN, eSwitch::K2SPDUP_DOWN },
    { eTimer::K2SPDDN_DOWN, eSwitch::K2SPDDN_DOWN },
};

static const InputDataMap InputGameReleaseMap[] =
{
    { eTimer::S1L_UP, eSwitch::S1L_DOWN },
    { eTimer::S1R_UP, eSwitch::S1R_DOWN },
    { eTimer::K11_UP, eSwitch::K11_DOWN },
    { eTimer::K12_UP, eSwitch::K12_DOWN },
    { eTimer::K13_UP, eSwitch::K13_DOWN },
    { eTimer::K14_UP, eSwitch::K14_DOWN },
    { eTimer::K15_UP, eSwitch::K15_DOWN },
    { eTimer::K16_UP, eSwitch::K16_DOWN },
    { eTimer::K17_UP, eSwitch::K17_DOWN },
    { eTimer::K18_UP, eSwitch::K18_DOWN },
    { eTimer::K19_UP, eSwitch::K19_DOWN },
    { eTimer::K1START_UP, eSwitch::K1START_DOWN },
    { eTimer::K1SELECT_UP, eSwitch::K1SELECT_DOWN },
    { eTimer::K1SPDUP_UP, eSwitch::K1SPDUP_DOWN },
    { eTimer::K1SPDDN_UP, eSwitch::K1SPDDN_DOWN },
    { eTimer::S2L_UP, eSwitch::S2L_DOWN },
    { eTimer::S2R_UP, eSwitch::S2R_DOWN },
    { eTimer::K21_UP, eSwitch::K21_DOWN },
    { eTimer::K22_UP, eSwitch::K22_DOWN },
    { eTimer::K23_UP, eSwitch::K23_DOWN },
    { eTimer::K24_UP, eSwitch::K24_DOWN },
    { eTimer::K25_UP, eSwitch::K25_DOWN },
    { eTimer::K26_UP, eSwitch::K26_DOWN },
    { eTimer::K27_UP, eSwitch::K27_DOWN },
    { eTimer::K28_UP, eSwitch::K28_DOWN },
    { eTimer::K29_UP, eSwitch::K29_DOWN },
    { eTimer::K2START_UP, eSwitch::K2START_DOWN },
    { eTimer::K2SELECT_UP, eSwitch::K2SELECT_DOWN },
    { eTimer::K2SPDUP_UP, eSwitch::K2SPDUP_DOWN },
    { eTimer::K2SPDDN_UP, eSwitch::K2SPDDN_DOWN },
};

////////////////////////////////////////////////////////////////////////////////

ScenePlay::ScenePlay(ePlayMode gamemode): vScene(context_play.mode, 1000), _mode(gamemode)
{
    _currentKeySample.assign(Input::ESC, 0);

    _inputAvailable = INPUT_MASK_FUNC;

    {
        using namespace std::string_literals;
        uint8_t slotmask = 0;

        if (context_play.mode == eMode::PLAY14 || _mode == ePlayMode::LOCAL_BATTLE) slotmask = 1 | 2;
        if (context_play.playerSlot == PLAYER_SLOT_1P) slotmask |= 1;
        if (context_play.playerSlot == PLAYER_SLOT_2P) slotmask |= 2;

        if (slotmask & 1)
        {
            _skin->setExtendedProperty("GAUGETYPE_1P"s, (void*)&context_play.gaugeType[PLAYER_SLOT_1P]);
            _inputAvailable |= INPUT_MASK_1P;
        }

        if (slotmask & 2)
        {
            _skin->setExtendedProperty("GAUGETYPE_2P"s, (void*)&context_play.gaugeType[PLAYER_SLOT_2P]);
            _inputAvailable |= INPUT_MASK_2P;
        }

        if (context_play.mode == eMode::PLAY14)
        {
            if (context_play.playerSlot == PLAYER_SLOT_1P)
                _skin->setExtendedProperty("GAUGETYPE_1P"s, (void*)&context_play.gaugeType[PLAYER_SLOT_1P]);
            if (context_play.playerSlot == PLAYER_SLOT_2P)
                _skin->setExtendedProperty("GAUGETYPE_2P"s, (void*)&context_play.gaugeType[PLAYER_SLOT_2P]);
        }
    }

    // file loading may delayed

    _state = ePlayState::PREPARE;

	if (context_chart.chartObj == nullptr)
	{

	}

	// global info
	if (ConfigMgr::get("P", cfg::P_LOAD_BGA, cfg::ON) == cfg::ON)
	{
		// TODO bga type
		gOptions.set(eOption::PLAY_BGA_TYPE, Option::BGA_NORMAL);
	}
	else
	{
		gOptions.set(eOption::PLAY_BGA_TYPE, Option::BGA_OFF);
	}

    // basic info
    gTexts.set(eText::PLAY_TITLE, context_chart.title);
    gTexts.set(eText::PLAY_SUBTITLE, context_chart.title2);
    gTexts.set(eText::PLAY_ARTIST, context_chart.artist);
    gTexts.set(eText::PLAY_SUBARTIST, context_chart.artist2);
    gTexts.set(eText::PLAY_GENRE, context_chart.genre);
    gNumbers.set(eNumber::PLAY_BPM, int(std::round(context_chart.itlBPM)));
    gNumbers.set(eNumber::BPM_MIN, int(std::round(context_chart.minBPM)));
    gNumbers.set(eNumber::BPM_MAX, int(std::round(context_chart.maxBPM)));

    // player datas
    gNumbers.set(eNumber::HS_1P, (int)ConfigMgr::get("P", cfg::P_HISPEED, 1.0) * 100);
    gNumbers.set(eNumber::HS_2P, 100);

    // set gauge type
    if (context_chart.chartObj)
        switch (context_chart.chartObj->type())
        {
        case eChartFormat::BMS:
        case eChartFormat::BMSON:
            for (size_t i = 0; i < MAX_PLAYERS; ++i)
            {
                switch (context_play.mods[i].gauge)
                {
                case eModGauge::NORMAL:     _gaugetype[i] = rc::gauge_ty::GROOVE; break;
                case eModGauge::HARD:       _gaugetype[i] = rc::gauge_ty::HARD; break;
                case eModGauge::EASY:       _gaugetype[i] = rc::gauge_ty::EASY; break;
                case eModGauge::DEATH:      _gaugetype[i] = rc::gauge_ty::DEATH; break;
                case eModGauge::PATTACK:    _gaugetype[i] = rc::gauge_ty::P_ATK; break;
                case eModGauge::GATTACK:    _gaugetype[i] = rc::gauge_ty::G_ATK; break;
                case eModGauge::ASSISTEASY: _gaugetype[i] = rc::gauge_ty::ASSIST; break;
                case eModGauge::EXHARD:     _gaugetype[i] = rc::gauge_ty::EXHARD; break;
                default: break;
                }
            }
            break;
        default:
            break;
        }

    if (!context_play.isCourse || context_play.isCourseFirstStage)
    {
        if (_mode == ePlayMode::LOCAL_BATTLE)
        {
            for (size_t i = 0; i < 2; ++i)
                switch (_gaugetype[i])
                {
                case rc::gauge_ty::GROOVE: 
                case rc::gauge_ty::EASY:
                case rc::gauge_ty::ASSIST:
                    context_play.initialHealth[i] = 0.2;
                    gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 20);
                    gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 20);
                    break;

                case rc::gauge_ty::HARD: 
                case rc::gauge_ty::DEATH:
                case rc::gauge_ty::P_ATK:
                case rc::gauge_ty::G_ATK:
                case rc::gauge_ty::EXHARD:
                    context_play.initialHealth[i] = 1.0;
                    gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 100);
                    gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 100);
                    break;

                default: break;
                }
        }
        else
        {
            switch (_gaugetype[context_play.playerSlot])
            {
            case rc::gauge_ty::GROOVE:
            case rc::gauge_ty::EASY:
            case rc::gauge_ty::ASSIST:
                context_play.initialHealth[context_play.playerSlot] = 0.2; 
                if (context_play.playerSlot == 0) gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 20);
                if (context_play.playerSlot == 1) gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 20);
                break;

            case rc::gauge_ty::HARD:
            case rc::gauge_ty::DEATH:
            case rc::gauge_ty::P_ATK:
            case rc::gauge_ty::G_ATK:
            case rc::gauge_ty::EXHARD:
                context_play.initialHealth[context_play.playerSlot] = 1.0; break;
                if (context_play.playerSlot == 0) gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, 100);
                if (context_play.playerSlot == 1) gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, 100);

            default: break;
            }
        }
    }
    else
    {
        if (_mode == ePlayMode::LOCAL_BATTLE)
        {
            gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, (int)context_play.initialHealth[0]);
            gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, (int)context_play.initialHealth[1]);
        }
        else
        {
            if (context_play.playerSlot == 0) gNumbers.set(eNumber::PLAY_1P_GROOVEGAUGE, (int)context_play.initialHealth[0]);
            if (context_play.playerSlot == 1) gNumbers.set(eNumber::PLAY_2P_GROOVEGAUGE, (int)context_play.initialHealth[1]);
        }
    }

	// load failed sound
	// TODO find failed sound path
	SoundMgr::loadSample("failed.wav", SOUND_FAILED_IDX);

    using namespace std::placeholders;
    _input.register_p("SCENE_PRESS", std::bind(&ScenePlay::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD", std::bind(&ScenePlay::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&ScenePlay::inputGameRelease, this, _1, _2));

    loopStart();
    _input.loopStart();
}
////////////////////////////////////////////////////////////////////////////////

void ScenePlay::loadChart()
{
    if (context_chart.chartObj == nullptr || !context_chart.chartObj->isLoaded())
    {
        if (context_chart.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            return;
        }

        context_chart.chartObj = vChartFormat::getFromFile(context_chart.path);
    }

    if (context_chart.chartObj == nullptr || !context_chart.chartObj->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart: " << context_chart.path.string();
        return;
    }

    switch (context_chart.chartObj->type())
    {
    case eChartFormat::BMS:
    {
        auto bms = std::reinterpret_pointer_cast<BMS>(context_chart.chartObj);
        // TODO mods
        break;
    }

    default:
        break;
    }

    context_chart.title = context_chart.chartObj->_title;
    context_chart.title2 = context_chart.chartObj->_title2;
    context_chart.artist = context_chart.chartObj->_artist;
    context_chart.artist2 = context_chart.chartObj->_artist2;
    context_chart.genre = context_chart.chartObj->_genre;
    context_chart.minBPM = context_chart.chartObj->_minBPM;
    context_chart.itlBPM = context_chart.chartObj->_itlBPM;
    context_chart.maxBPM = context_chart.chartObj->_maxBPM;

    //load chart object from Chart object
    switch (context_chart.chartObj->type())
    {
    case eChartFormat::BMS:
        if (_mode == ePlayMode::LOCAL_BATTLE)
        {
            context_play.chartObj[0] = std::make_shared<chartBMS>(std::reinterpret_pointer_cast<BMS>(context_chart.chartObj));
            context_play.chartObj[1] = std::make_shared<chartBMS>(std::reinterpret_pointer_cast<BMS>(context_chart.chartObj));
        }
        else
        {
            context_play.chartObj[context_play.playerSlot] = std::make_shared<chartBMS>(std::reinterpret_pointer_cast<BMS>(context_chart.chartObj));
        }
        _chartLoaded = true;
        gNumbers.set(eNumber::PLAY_REMAIN_MIN, int(context_play.chartObj[context_play.playerSlot]->getTotalLength().norm() / 1000 / 60));
        gNumbers.set(eNumber::PLAY_REMAIN_SEC, int(context_play.chartObj[context_play.playerSlot]->getTotalLength().norm() / 1000 % 60));
        break;

    case eChartFormat::BMSON:
    default:
		LOG_WARNING << "[Play] chart format not supported.";
        return;
    }

    context_play.remainTime = context_play.chartObj[context_play.playerSlot]->getTotalLength();

    // build Ruleset object
    switch (context_play.rulesetType)
    {
    case eRuleset::CLASSIC:
    {
        // set judge diff
        rc::judgeDiff judgediff = rc::judgeDiff::NORMAL;
        switch (context_chart.chartObj->type())
        {
        case eChartFormat::BMS:
            switch (std::reinterpret_pointer_cast<BMS>(context_chart.chartObj)->rank)
            {
            case 0: judgediff = rc::judgeDiff::VERYHARD; break;
            case 1: judgediff = rc::judgeDiff::HARD; break;
            case 2: judgediff = rc::judgeDiff::NORMAL; break;
            case 3: judgediff = rc::judgeDiff::EASY; break;
            case 4: judgediff = rc::judgeDiff::VERYEASY; break;
            case 5: break;
            case 6: judgediff = rc::judgeDiff::WHAT; break;
            default: break;
            }
        case eChartFormat::BMSON:
        default: 
			LOG_WARNING << "[Play] chart format not supported.";
			break;
        }

        if (context_play.mode == eMode::PLAY14)
        {
            context_play.ruleset[context_play.playerSlot] = std::make_shared<RulesetClassic>(
                context_chart.chartObj,
                context_play.chartObj[context_play.playerSlot], judgediff, 
                _gaugetype[context_play.playerSlot], context_play.initialHealth[context_play.playerSlot], rc::player::DP);
        }
        else if (_mode == ePlayMode::LOCAL_BATTLE)
        {
            context_play.ruleset[PLAYER_SLOT_1P] = std::make_shared<RulesetClassic>(
                context_chart.chartObj,
                context_play.chartObj[PLAYER_SLOT_1P], judgediff,
                _gaugetype[PLAYER_SLOT_1P], context_play.initialHealth[PLAYER_SLOT_1P], rc::player::BATTLE_1P);
            context_play.ruleset[PLAYER_SLOT_2P] = std::make_shared<RulesetClassic>(
                context_chart.chartObj,
                context_play.chartObj[PLAYER_SLOT_2P], judgediff,
                _gaugetype[PLAYER_SLOT_2P], context_play.initialHealth[PLAYER_SLOT_2P], rc::player::BATTLE_2P);
        }
        else
        {
            context_play.ruleset[context_play.playerSlot] = std::make_shared<RulesetClassic>(
                context_chart.chartObj,
                context_play.chartObj[context_play.playerSlot], judgediff, 
                _gaugetype[context_play.playerSlot], context_play.initialHealth[context_play.playerSlot],
                context_play.playerSlot == PLAYER_SLOT_1P ? rc::player::SP_1P : rc::player::SP_2P);
        }
        _rulesetLoaded = true;
    }
    break;

    default:
        break;
    }

    // load samples
    if (!context_chart.isSampleLoaded && !sceneEnding)
    {
        auto dtor = std::async(std::launch::async, [&]() {
            auto _pChart = context_chart.chartObj;
            auto chartDir = context_chart.chartObj->getDirectory();
            LOG_DEBUG << "[Play] Load files from " << chartDir.c_str();
            for (const auto& it : _pChart->_wavFiles)
            {
				if (sceneEnding) break;
                if (it.empty()) continue;
                ++wavToLoad;
            }
            if (wavToLoad == 0)
            {
                wavLoaded = 1;
                context_chart.isSampleLoaded = true;
                return;
            }
            for (size_t i = 0; i < _pChart->_wavFiles.size(); ++i)
            {
				if (sceneEnding) break;
                const auto& wav = _pChart->_wavFiles[i];
                if (wav.empty()) continue;
				Path pWav(wav);
				if (pWav.is_absolute())
					SoundMgr::loadKeySample(wav, i);
				else
					SoundMgr::loadKeySample((chartDir / wav).string(), i);
                ++wavLoaded;
            }
            context_chart.isSampleLoaded = true;
        });
    }

    // load bga
    if (ConfigMgr::get("P", cfg::P_LOAD_BGA, cfg::ON) == cfg::ON && !context_chart.isBgaLoaded && !sceneEnding)
    {
        auto dtor = std::async(std::launch::async, [&]() {
            auto _pChart = context_chart.chartObj;
			auto chartDir = context_chart.chartObj->getDirectory();
            for (const auto& it : _pChart->_bgaFiles)
            {
				if (sceneEnding) return;
                if (it.empty()) continue;
                ++bmpToLoad;
            }
            if (bmpToLoad == 0)
            {
                bmpLoaded = 1;
                context_chart.isBgaLoaded = true;
                return;
            }
            for (size_t i = 0; i < _pChart->_bgaFiles.size(); ++i)
            {
				if (sceneEnding) return;
                const auto& bmp = _pChart->_bgaFiles[i];
                if (bmp.empty()) continue;


				Path pBmp(bmp);
				if (pBmp.is_absolute())
					context_play.bgaTexture->addBmp(i, pBmp);
				else
					context_play.bgaTexture->addBmp(i, chartDir / pBmp);

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

                ++bmpLoaded;
            }
			context_play.bgaTexture->setSlotFromBMS(*std::reinterpret_pointer_cast<chartBMS>(context_play.chartObj[context_play.playerSlot]));
            context_chart.isBgaLoaded = true;
        });
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (_mode == ePlayMode::LOCAL_BATTLE)
    {
        if (context_play.ruleset[PLAYER_SLOT_1P] != nullptr)
        {
            auto fp = std::bind(&vRuleset::updatePress, context_play.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_p("JUDGE_PRESS", fp);
            auto fh = std::bind(&vRuleset::updateHold, context_play.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_h("JUDGE_HOLD", fh);
            auto fr = std::bind(&vRuleset::updateRelease, context_play.ruleset[PLAYER_SLOT_1P], _1, _2);
            _input.register_r("JUDGE_RELEASE", fr);
        }
        else
            LOG_ERROR << "[Play] Ruleset of 1P not initialized!";

        if (context_play.ruleset[PLAYER_SLOT_2P] != nullptr)
        {
            auto fp = std::bind(&vRuleset::updatePress, context_play.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_p("JUDGE_PRESS", fp);
            auto fh = std::bind(&vRuleset::updateHold, context_play.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_h("JUDGE_HOLD", fh);
            auto fr = std::bind(&vRuleset::updateRelease, context_play.ruleset[PLAYER_SLOT_2P], _1, _2);
            _input.register_r("JUDGE_RELEASE", fr);
        }
        else
            LOG_ERROR << "[Play] Ruleset of 2P not initialized!";
    }
    else // SINGLE or MULTI
    {
        if (context_play.ruleset[context_play.playerSlot] != nullptr)
        {
            auto fp = std::bind(&vRuleset::updatePress, context_play.ruleset[context_play.playerSlot], _1, _2);
            _input.register_p("JUDGE_PRESS", fp);
            auto fh = std::bind(&vRuleset::updateHold, context_play.ruleset[context_play.playerSlot], _1, _2);
            _input.register_h("JUDGE_HOLD", fh);
            auto fr = std::bind(&vRuleset::updateRelease, context_play.ruleset[context_play.playerSlot], _1, _2);
            _input.register_r("JUDGE_RELEASE", fr);
        }
        else
            LOG_ERROR << "[Play] Ruleset not initialized!";
    }
}

void ScenePlay::removeInputJudgeCallback()
{
    for (size_t i = 0; i < context_play.ruleset.size(); ++i)
    {
        _input.unregister_p("JUDGE_PRESS");
        _input.unregister_h("JUDGE_HOLD");
        _input.unregister_r("JUDGE_RELEASE");
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
        _state = ePlayState::LOADING;
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_LOADING);

		loadChartRet = std::async(std::launch::async, std::bind(&ScenePlay::loadChart, this));
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

    if (_chartLoaded && _rulesetLoaded &&
        context_chart.isSampleLoaded && context_chart.isBgaLoaded && 
		rt > _skin->info.timeMinimumLoad)
    {
        _state = ePlayState::LOAD_END;
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_READY);
		if (context_play.bgaTexture) context_play.bgaTexture->reset();
        gTimers.set(eTimer::PLAY_READY, t.norm());
        LOG_DEBUG << "[Play] State changed to READY";
    }
}

void ScenePlay::updateLoadEnd()
{
	auto t = Time();
    auto rt = t - gTimers.get(eTimer::PLAY_READY);
    updateTTrotation(false);
    if (rt > _skin->info.timeGetReady)
    {
        _state = ePlayState::PLAYING;
		gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_PLAYING);
        gTimers.set(eTimer::PLAY_START, t.norm());
        setInputJudgeCallback();
		context_chart.started = true;
        LOG_DEBUG << "[Play] State changed to PLAY_START";
    }
}

void ScenePlay::updatePlaying()
{
	auto t = Time();
	auto rt = t - gTimers.get(eTimer::PLAY_START);
    gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (context_play.chartObj[context_play.playerSlot]->getCurrentBeat() * 4.0)) % 1000);
    context_play.bgaTexture->update(rt, false);

    if (_mode == ePlayMode::LOCAL_BATTLE)
    {
        context_play.chartObj[PLAYER_SLOT_1P]->update(rt);
        context_play.chartObj[PLAYER_SLOT_2P]->update(rt);
        context_play.ruleset[PLAYER_SLOT_1P]->update(t);
        context_play.ruleset[PLAYER_SLOT_2P]->update(t);
    }
    else
    {
        context_play.chartObj[context_play.playerSlot]->update(rt);
        context_play.ruleset[context_play.playerSlot]->update(t);
    }

    gNumbers.set(eNumber::PLAY_BPM, int(std::round(context_play.chartObj[context_play.playerSlot]->getCurrentBPM())));
    // play time / remain time
    {
        auto startTime = rt - gTimers.get(eTimer::PLAY_START);
        auto playtime = rt.norm() / 1000;
        auto remaintime = context_play.chartObj[context_play.playerSlot]->getTotalLength().norm() - playtime;
        gNumbers.set(eNumber::PLAY_MIN, int(playtime / 60));
        gNumbers.set(eNumber::PLAY_SEC, int(playtime % 60));
        gNumbers.set(eNumber::PLAY_REMAIN_MIN, int(remaintime / 60));
        gNumbers.set(eNumber::PLAY_REMAIN_SEC, int(remaintime % 60));
    }

    procCommonNotes();
    changeKeySampleMapping(rt);
	//updateBga();

    // health check (-> to failed)
    // TODO also play failed sound
    switch (_mode)
    {
    case ePlayMode::SINGLE:
    {
		//if (context_play.health[context_play.playerSlot] <= 0)
        if (context_play.ruleset[context_play.playerSlot]->getData().health <= 0)
        {
            _state = ePlayState::FAILED;
            gTimers.set(eTimer::FAIL_BEGIN, t.norm());
            gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FAILED);
			SoundMgr::playSample(SOUND_FAILED_IDX);
            removeInputJudgeCallback();
            LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
        }

        if (_isPlayerFinished[context_play.playerSlot] ^ context_play.ruleset[context_play.playerSlot]->isFinished())
        {
            _isPlayerFinished[context_play.playerSlot] = true;
            gTimers.set(context_play.playerSlot == 0 ? eTimer::PLAY_P1_FINISHED : eTimer::PLAY_P2_FINISHED, t.norm());
            if (context_play.ruleset[context_play.playerSlot]->getData().combo == context_play.chartObj[context_play.playerSlot]->getNoteCount())
                gTimers.set(context_play.playerSlot == 0 ? eTimer::PLAY_FULLCOMBO_1P : eTimer::PLAY_FULLCOMBO_2P, t.norm());
        }
    }
    break;

    case ePlayMode::LOCAL_BATTLE:
    {
        if (context_play.ruleset[PLAYER_SLOT_1P]->getData().health <= 0 && 
            context_play.ruleset[PLAYER_SLOT_2P]->getData().health <= 0)
        {
            _state = ePlayState::FAILED;
            gTimers.set(eTimer::FAIL_BEGIN, t.norm());
            gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FAILED);
			SoundMgr::playSample(SOUND_FAILED_IDX);
            removeInputJudgeCallback();
            LOG_DEBUG << "[Play] State changed to PLAY_FAILED";
        }

        if (_isPlayerFinished[PLAYER_SLOT_1P] ^ context_play.ruleset[PLAYER_SLOT_1P]->isFinished())
        {
            _isPlayerFinished[PLAYER_SLOT_1P] = true;
            gTimers.set(eTimer::PLAY_P1_FINISHED, t.norm());
            if (context_play.ruleset[PLAYER_SLOT_1P]->getData().combo == context_play.chartObj[PLAYER_SLOT_1P]->getNoteCount())
                gTimers.set(eTimer::PLAY_FULLCOMBO_1P, t.norm());
        }
        if (_isPlayerFinished[PLAYER_SLOT_2P] ^ context_play.ruleset[PLAYER_SLOT_2P]->isFinished())
        {
            _isPlayerFinished[PLAYER_SLOT_2P] = true;
            gTimers.set(eTimer::PLAY_P2_FINISHED, t.norm());
            if (context_play.ruleset[PLAYER_SLOT_2P]->getData().combo == context_play.chartObj[PLAYER_SLOT_2P]->getNoteCount())
                gTimers.set(eTimer::PLAY_FULLCOMBO_2P, t.norm());
        }
    }
    break;

    default:
        break;
    }

    updateTTrotation(true);

    //last note check
    if (rt.norm() - context_play.chartObj[context_play.playerSlot]->getTotalLength().norm() >= 0)
    {
        _state = ePlayState::FADEOUT;
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);
        removeInputJudgeCallback();
    }
     
}

void ScenePlay::updateFadeout()
{
    auto t = Time();
    auto rt = t - gTimers.get(eTimer::SCENE_START);
    auto ft = t - gTimers.get(eTimer::FADEOUT_BEGIN);
    gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (context_play.chartObj[context_play.playerSlot]->getCurrentBeat() * 4.0)) % 1000);
    updateTTrotation(context_chart.started);
	context_play.bgaTexture->update(rt, false);

    if (ft >= _skin->info.timeOutro)
    {
        removeInputJudgeCallback();
        loopEnd();
        _input.loopEnd();
        __next_scene = eScene::RESULT;
    }

}

void ScenePlay::updateFailed()
{
    auto t = Time();
    auto ft = t - gTimers.get(eTimer::FAIL_BEGIN);
    gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (context_play.chartObj[context_play.playerSlot]->getCurrentBeat() * 4.0)) % 1000);
    updateTTrotation(context_chart.started);

    //failed play finished, move to next scene. No fadeout
    if (ft.norm() >= _skin->info.timeFailed)
    {
        removeInputJudgeCallback();
        loopEnd();
        _input.loopEnd();
        __next_scene = eScene::RESULT;
    }
}


void ScenePlay::procCommonNotes()
{
    assert(context_play.chartObj[context_play.playerSlot] != nullptr);
    auto it = context_play.chartObj[context_play.playerSlot]->notePlainExpired.begin();
    size_t i;
    size_t max = std::min(_bgmSampleIdxBuf.size(), context_play.chartObj[context_play.playerSlot]->notePlainExpired.size());
    for (i = 0; i < max && it != context_play.chartObj[context_play.playerSlot]->notePlainExpired.end(); ++i, ++it)
    {
        if ((it->index & 0xF0) == 0xE0)
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
        else
        {
            // BGM
            _bgmSampleIdxBuf[i] = (unsigned)std::get<long long>(it->value);
        }
    }
    // TODO also play keysound in auto

    SoundMgr::playKeySample(i, (size_t*)_bgmSampleIdxBuf.data());
}

void ScenePlay::changeKeySampleMapping(Time t)
{
    if (_mode == ePlayMode::LOCAL_BATTLE)
    {
        for (auto i = 0; i < Input::S2L; ++i)
            if (_inputAvailable[i])
            {
                assert(context_play.chartObj[PLAYER_SLOT_1P] != nullptr);
                auto chan = context_play.chartObj[PLAYER_SLOT_1P]->getLaneFromKey((Input::Ingame)i);
                if (chan.first == NoteLaneCategory::_) continue;
                auto note = context_play.chartObj[PLAYER_SLOT_1P]->incomingNoteOfLane(chan.first, chan.second);
                _currentKeySample[i] = (size_t)std::get<long long>(note->value);
            }
        for (auto i = 0; i < Input::ESC; ++i)
            if (_inputAvailable[i])
            {
                assert(context_play.chartObj[PLAYER_SLOT_2P] != nullptr);
                auto chan = context_play.chartObj[PLAYER_SLOT_2P]->getLaneFromKey((Input::Ingame)i);
                if (chan.first == NoteLaneCategory::_) continue;
                auto note = context_play.chartObj[PLAYER_SLOT_2P]->incomingNoteOfLane(chan.first, chan.second);
                _currentKeySample[i] = (size_t)std::get<long long>(note->value);
            }
    }
    else
    {
        for (auto i = 0; i < Input::ESC; ++i)
            if (_inputAvailable[i])
            {
                assert(context_play.chartObj[context_play.playerSlot] != nullptr);
                auto chan = context_play.chartObj[context_play.playerSlot]->getLaneFromKey((Input::Ingame)i);
                if (chan.first == NoteLaneCategory::_) continue;
                auto note = context_play.chartObj[context_play.playerSlot]->incomingNoteOfLane(chan.first, chan.second);
                _currentKeySample[i] = (size_t)std::get<long long>(note->value);
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

void ScenePlay::updateTTrotation(bool startedPlaying)
{
    auto a = _ttAngleDelta;
    if (startedPlaying)
    {
        auto t = Time();
        auto rt = t - gTimers.get(eTimer::PLAY_START);
        for (auto& aa : a)
            aa += int(rt.norm() * 180 / 1000);
    }
    for (auto& aa : a)
        aa %= 360;
    gNumbers.set(eNumber::_ANGLE_TT_1P, a[0]);
    gNumbers.set(eNumber::_ANGLE_TT_2P, a[1]);
}

////////////////////////////////////////////////////////////////////////////////
// CALLBACK
void ScenePlay::inputGamePress(InputMask& m, Time t)
{
    using namespace Input;

    auto input = _inputAvailable & m;

    // individual keys
    size_t sampleCount = 0;
    for (size_t i = 0; i < ESC; ++i)
        if (input[i])
        {
            if (_currentKeySample[i])
                _keySampleIdxBuf[sampleCount++] = _currentKeySample[i];
            gTimers.set(InputGamePressMap[i].tm, t.norm());
            gTimers.set(InputGameReleaseMap[i].tm, LLONG_MAX);
            gSwitches.set(InputGamePressMap[i].sw, true);
        }

    if (input[S1L] || input[S1R])
    {
        gTimers.set(eTimer::S1_DOWN, t.norm());
        gTimers.set(eTimer::S1_UP, LLONG_MAX);
        gSwitches.set(eSwitch::S1_DOWN, true);
    }

    if (input[S2L] || input[S2R])
    {
        gTimers.set(eTimer::S2_DOWN, t.norm());
        gTimers.set(eTimer::S2_UP, LLONG_MAX);
        gSwitches.set(eSwitch::S2_DOWN, true);
    }

    SoundMgr::playKeySample(sampleCount, (size_t*)&_keySampleIdxBuf[0]);

    if (input[K1SPDUP])
    {
        int hs = gNumbers.get(eNumber::HS_1P);
        if (hs < 900)
            gNumbers.set(eNumber::HS_1P, hs + 25);
    }
    if (input[K1SPDDN])
    {
        int hs = gNumbers.get(eNumber::HS_1P);
        if (hs > 25)
            gNumbers.set(eNumber::HS_1P, hs - 25);
    }

    if (input[K2SPDUP])
    {
        int hs = gNumbers.get(eNumber::HS_2P);
        if (hs < 900)
            gNumbers.set(eNumber::HS_2P, hs + 25);
    }
    if (input[K2SPDDN])
    {
        int hs = gNumbers.get(eNumber::HS_2P);
        if (hs > 25)
            gNumbers.set(eNumber::HS_2P, hs - 25);
    }

    if (input[Input::ESC] || (input[Input::K1START] && input[Input::K1SELECT]) || (input[Input::K2START] && input[Input::K2SELECT]))
    {
        _state = ePlayState::FADEOUT;
        gTimers.set(eTimer::FADEOUT_BEGIN, t.norm());
        gOptions.set(eOption::PLAY_SCENE_STAT, Option::SPLAY_FADEOUT);

        // TODO set health to 0
        switch (_mode)
        {
        case ePlayMode::SINGLE:
        {
            // 
        }
        break;

        case ePlayMode::LOCAL_BATTLE:
        {
            // 
        }
        break;

        default:
            break;
        }

    }
}

// CALLBACK
void ScenePlay::inputGameHold(InputMask& m, Time t)
{
	using namespace Input;
	for (size_t i = 0; i < ESC; ++i)
	{
		// TODO analog spin speed
		switch (i)
		{
		case S1L: _ttAngleDelta[PLAYER_SLOT_1P] -= 5; break;
		case S1R: _ttAngleDelta[PLAYER_SLOT_1P] += 5; break;
		case S2L: _ttAngleDelta[PLAYER_SLOT_2P] -= 5; break;
		case S2R: _ttAngleDelta[PLAYER_SLOT_2P] += 5; break;
		default: break;
		}
	}
}

// CALLBACK
void ScenePlay::inputGameRelease(InputMask& m, Time t)
{
    using namespace Input;
    auto input = _inputAvailable & m;

    size_t count = 0;
    for (size_t i = 0; i < Input::ESC; ++i)
        if (input[i])
        {
            gTimers.set(InputGamePressMap[i].tm, LLONG_MAX);
            gTimers.set(InputGameReleaseMap[i].tm, t.norm());
            gSwitches.set(InputGameReleaseMap[i].sw, false);

            // TODO stop sample playing while release in LN notes
        }


    if (input[S1L] || input[S1R])
    {
        gTimers.set(eTimer::S1_DOWN, LLONG_MAX);
        gTimers.set(eTimer::S1_UP, t.norm());
        gSwitches.set(eSwitch::S1_DOWN, false);
    }

    if (input[S2L] || input[S2R])
    {
        gTimers.set(eTimer::S2_DOWN, LLONG_MAX);
        gTimers.set(eTimer::S2_UP, t.norm());
        gSwitches.set(eSwitch::S2_DOWN, false);
    }

}
