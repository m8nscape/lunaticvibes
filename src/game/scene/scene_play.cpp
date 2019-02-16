#include <cassert>
#include <future>
#include "scene_play.h"
#include "scene_context.h"
#include "game/sound/sound_mgr.h"
#include "game/ruleset/ruleset_classic.h"
#include "chart/bms.h"
#include "game/scroll/scroll_bms.h"
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
    {},{},
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
    {},{}
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
    {},{},
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
    {},{}
};

////////////////////////////////////////////////////////////////////////////////

ScenePlay::ScenePlay(ePlayMode mode, unsigned keys, eRuleset ruleset):
    vScene(eMode::PLAY7, 1000), _mode(mode), _rule(ruleset), _keys(keys)
{
    _currentKeySample.assign(Input::ESC, 0);


    // file loading may delayed

    _state = ePlayState::PREPARE;

    _input.loopStart();
    loopStart();
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::loadChart()
{
    if (context_chart.chartObj == nullptr)
    {
        if (context_chart.path.empty())
        {
            LOG_ERROR << "[Play] Chart not specified!";
            return;
        }

        // TODO dispatch Chart object upon filename extension
        // currently hard coded for BMS
        context_chart.chartObj = std::make_shared<BMS>(std::filesystem::absolute(context_chart.path).string());
    }

    if (!context_chart.chartObj->isLoaded())
    {
        LOG_ERROR << "[Play] Invalid chart!";
        return;
    }

    context_chart.title = context_chart.chartObj->_title;
    context_chart.title2 = context_chart.chartObj->_title2;
    context_chart.artist = context_chart.chartObj->_artist;
    context_chart.artist2 = context_chart.chartObj->_artist2;
    context_chart.genre = context_chart.chartObj->_genre;
    context_chart.minBPM = context_chart.chartObj->_minBPM;
    context_chart.itlBPM = context_chart.chartObj->getInitialBPM();
    context_chart.maxBPM = context_chart.chartObj->_maxBPM;

    // TODO load Scroll object from Chart object
    // currently hard coded for BMS
    context_chart.scrollObj = std::make_shared<ScrollBMS>((const BMS&)*context_chart.chartObj);
    _scrollLoaded = true;

    // build Ruleset object
    switch (_rule)
    {
    case eRuleset::CLASSIC:
        _pRuleset = std::make_shared<RulesetClassic>(&*context_chart.scrollObj);
        _rulesetLoaded = true;
        break;
    default:
        _pRuleset = nullptr;
        break;
    }

    // load samples
    if (!context_chart.isSampleLoaded)
    {
        std::async(std::launch::async, [&]() {
            auto _pChart = context_chart.chartObj;
            for (const auto& it : _pChart->_wavFiles)
            {
                if (it.empty()) continue;
                ++wavToLoad;
            }
            if (wavToLoad == 0)
            {
                wavLoaded = 1;
                return;
            }
            for (size_t i = 0; i < _pChart->_wavFiles.size(); ++i)
            {
                const auto& wav = _pChart->_wavFiles[i];
                if (wav.empty()) continue;
                SoundMgr::loadKeySample(wav, i);
                ++wavLoaded;
            }
        }).wait();
        context_chart.isSampleLoaded = true;
    }

    // load bga
    if (!context_chart.isBgaLoaded)
    {
        std::async(std::launch::async, [&]() {
            auto _pChart = context_chart.chartObj;
            for (const auto& it : _pChart->_bgaFiles)
            {
                if (it.empty()) continue;
                ++bmpToLoad;
            }
            if (bmpToLoad == 0)
            {
                bmpLoaded = 1;
                return;
            }
            for (size_t i = 0; i < _pChart->_bgaFiles.size(); ++i)
            {
                const auto& bmp = _pChart->_bgaFiles[i];
                if (bmp.empty()) continue;
                // TODO load bga textures
                ++bmpLoaded;
            }
        }).wait();
        context_chart.isBgaLoaded = true;
    }
}

void ScenePlay::setInputJudgeCallback()
{
    using namespace std::placeholders;
    if (_pRuleset != nullptr)
    {
        auto fp = std::bind(&vRuleset::updatePress, _pRuleset, _1, _2);
        LOG_DEBUG << "[Play] Bind fpress: " << &fp;
        _input.register_p("JUDGE_PRESS", fp);

        auto fh = std::bind(&vRuleset::updateHold, _pRuleset, _1, _2);
        LOG_DEBUG << "[Play] Bind fhold: " << &fh;
        _input.register_h("JUDGE_HOLD", fh);

        auto fr = std::bind(&vRuleset::updateRelease, _pRuleset, _1, _2);
        LOG_DEBUG << "[Play] Bind frelease: " << &fr;
        _input.register_r("JUDGE_RELEASE", fr);
    }
    _input.register_p("SCENE_PRESS",   std::bind(&ScenePlay::inputGamePress, this, _1, _2));
    _input.register_h("SCENE_HOLD",    std::bind(&ScenePlay::inputGameHold, this, _1, _2));
    _input.register_r("SCENE_RELEASE", std::bind(&ScenePlay::inputGameRelease, this, _1, _2));
}

void ScenePlay::removeInputJudgeCallback(bool shutter)
{
    _input.unregister_p("JUDGE_PRESS");
    _input.unregister_h("JUDGE_HOLD");
    _input.unregister_r("JUDGE_RELEASE");
    _input.unregister_p("SCENE_PRESS");
    _input.unregister_h("SCENE_HOLD");
    _input.unregister_r("SCENE_RELEASE");
}

////////////////////////////////////////////////////////////////////////////////

void ScenePlay::_updateAsync()
{
    switch (_state)
    {
    case ePlayState::PREPARE:
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
    case ePlayState::LAST_NOTE_END:
        updateSongOutro();
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
    auto rt = getTimePoint() - gTimers.get(eTimer::SCENE_START);
    if (rt > _skin->info.timeIntro)
    {
        _state = ePlayState::LOADING;
        std::thread(&ScenePlay::loadChart, this).detach();
        LOG_DEBUG << "[Play] State changed to LOADING";
    }
}

void ScenePlay::updateLoading()
{
    // TODO display progress
    //  set global bargraph values

    if (_scrollLoaded && _rulesetLoaded &&
        context_chart.isSampleLoaded && context_chart.isBgaLoaded)
    {
        _state = ePlayState::LOAD_END;
        gTimers.set(eTimer::PLAY_READY, getTimePoint());
        LOG_DEBUG << "[Play] State changed to READY";
    }
}

void ScenePlay::updateLoadEnd()
{
    auto rt = getTimePoint() - gTimers.get(eTimer::SCENE_START);
    if (rt > _skin->info.timeGetReady)
    {
        _state = ePlayState::PLAYING;
        gTimers.set(eTimer::PLAY_START, getTimePoint());
        setInputJudgeCallback();
        LOG_DEBUG << "[Play] State changed to PLAY_START";
    }
}

void ScenePlay::updatePlaying()
{
    //gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (context_chart.scrollObj->getCurrentBeat() / 4.0)) % 1000);
    auto ht = getHighresTimePoint() - r2h(gTimers.get(eTimer::PLAY_START));
    auto rt = h2r(ht);

    _pRuleset->updateAsync(rt);
    context_chart.scrollObj->update(ht);
    playBGMSamples();
    changeKeySampleMapping(rt);


    // TODO health check (-> to failed)
    // also play failed sound
     //if (*failed*)
     //{
     //    _state = ePlayState::FAILED;
     //    gTimers.set(eTimer::FAIL_BEGIN, rt);
     //    removeInputJudgeCallback(true);
     //}

    // TODO last note check
    //if (*last note end*)
    //{
    //    _state = ePlayState::LAST_NOTE_END;
    //    gTimers.set(eTimer::PLAY_LAST_NOTE_JUDGE, rt);
    //}
     
}

void ScenePlay::updateSongOutro()
{
    gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (context_chart.scrollObj->getCurrentBeat() / 4.0)) % 1000);
    auto rt = getTimePoint();

    // TODO chart play finished
    //if (*asdasfsa*)
    //{
    //    _state = ePlayState::FADEOUT;
    //    gTimers.set(eTimer::FADEOUT_BEGIN, rt);
    //    removeInputJudgeCallback(false);
    //}
}

void ScenePlay::updateFadeout()
{
    gTimers.set(eTimer::MUSIC_BEAT, int(1000 * (context_chart.scrollObj->getCurrentBeat() / 4.0)) % 1000);

    // TODO fadeout finished

}

void ScenePlay::updateFailed()
{
    // TODO failed play finished
    //if (*asdasfsa*)
    //{
    //    _state = ePlayState::FADEOUT;
    //    gTimers.set(eTimer::FADEOUT_BEGIN, rt);
    //    removeInputJudgeCallback(false);
    //}
}


void ScenePlay::playBGMSamples()
{
    assert(context_chart.scrollObj != nullptr);
    size_t i = 0;
    auto it = context_chart.scrollObj->notePlainExpired.begin();
    size_t max = _bgmSampleIdxBuf.size() < context_chart.scrollObj->notePlainExpired.size() ?
        _bgmSampleIdxBuf.size() : context_chart.scrollObj->notePlainExpired.size();
    for (; i < max; ++i)
        _bgmSampleIdxBuf[i] = (unsigned)std::get<long long>(it->value);

    // TODO also play keysound in auto

    SoundMgr::playKeySample(i, &_bgmSampleIdxBuf[0]);
}

void ScenePlay::changeKeySampleMapping(rTime t)
{
    for (auto i = 0; i < Input::ESC; ++i)
        if (_inputAvailable[i])
        {
            assert(context_chart.scrollObj != nullptr);
            auto chan = context_chart.scrollObj->getChannelFromKey((Input::Ingame)i);
            auto note = context_chart.scrollObj->lastNoteOfChannel(chan.first, chan.second);
            _currentKeySample[i] = (size_t)std::get<long long>(note->value);
        }
}



////////////////////////////////////////////////////////////////////////////////

void ScenePlay::inputGamePress(InputMask& m, rTime t)
{
    // individual keys
    using namespace Input;
    size_t sampleCount = 0;
    for (size_t i = 0; i < ESC; ++i)
        if (_inputAvailable[i] && m[i])
        {
            _keySampleIdxBuf[sampleCount++] = _currentKeySample[i];
            gTimers.set(InputGamePressMap[i].tm, t);
            gTimers.set(InputGameReleaseMap[i].tm, -1);
            gSwitches.set(InputGamePressMap[i].sw, true);
        }

    SoundMgr::playKeySample(sampleCount, &_keySampleIdxBuf[0]);
}


void ScenePlay::inputGameHold(InputMask& m, rTime t)
{

}

void ScenePlay::inputGameRelease(InputMask& m, rTime t)
{
    size_t count = 0;
    for (size_t i = 0; i < Input::ESC; ++i)
        if (_inputAvailable[i] && m[i])
        {
            gTimers.set(InputGamePressMap[i].tm, -1);
            gTimers.set(InputGameReleaseMap[i].tm, t);
            gSwitches.set(InputGameReleaseMap[i].sw, false);

            // TODO stop sample playing while release in LN notes
        }
}
