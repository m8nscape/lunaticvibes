#pragma once
#include "scene.h"
#include "asynclooper.h"
#include "chart/chart.h"
#include "game/scroll/scroll.h"
#include "game/ruleset/ruleset.h"

enum class ePlayMode
{
    SINGLE,
    PLAY_BATTLE,
    MULTIPLAYER
};

enum class ePlayState
{
    PREPARE,
    LOADING,
    LOAD_END,
    PLAYING,
    LAST_NOTE_END,
    FAILED,
    FADEOUT,
};

class ScenePlay : public vScene
{
private:
	bool _switchingState = false;
    ePlayState _state;
    ePlayMode _mode;
    eRuleset  _rule;
    unsigned  _keys;
    InputMask _inputAvailable;
    StringPath _chartPath;
    std::vector<size_t> _currentKeySample;
    std::shared_ptr<vRuleset> _pRuleset;

private:
    bool _isHoldingStart = false;
    bool _isHoldingSelect = false;

public:
    ScenePlay(ePlayMode mode, unsigned keys, eRuleset ruleset);
    virtual ~ScenePlay() = default;

private:
    std::array<unsigned, 128> _bgmSampleIdxBuf{};
    std::array<unsigned, 128> _keySampleIdxBuf{};

protected:
    // common
    void loadChart();
	constexpr double getWavLoadProgress() { return (wavToLoad == 0) ? (wavLoaded ? 1.0 : 0.0) : (double)wavLoaded / wavToLoad; }
	constexpr double getBgaLoadProgress() { return (bmpToLoad == 0) ? (wavLoaded ? 1.0 : 0.0) : (double)bmpLoaded / bmpToLoad; }

    void setInputJudgeCallback();
    void removeInputJudgeCallback(bool shutter = false);

protected:
    // loading indicators
    bool _scrollLoaded = false;
    bool _rulesetLoaded = false;
    //bool _sampleLoaded = false;
    //bool _bgaLoaded = false;
    unsigned wavLoaded = 0;
    unsigned wavToLoad = 0;
    unsigned bmpLoaded = 0;
    unsigned bmpToLoad = 0;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    void updatePrepare();
    void updateLoading();
    void updateLoadEnd();
    void updatePlaying();
    void updateSongOutro();
    void updateFadeout();
    void updateFailed();

protected:
    // Inner-state updates
    void playBGMSamples();
    void changeKeySampleMapping(timestamp t);

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, timestamp);
    void inputGameHold(InputMask&, timestamp);
    void inputGameRelease(InputMask&, timestamp);
};