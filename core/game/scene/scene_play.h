#pragma once
#include <mutex>
#include "scene.h"
#include "asynclooper.h"
#include "chart/chart.h"
#include "game/scroll/scroll.h"
#include "game/ruleset/ruleset.h"

enum class ePlayMode
{
    SINGLE,
    LOCAL_BATTLE,
};

enum class ePlayState
{
    PREPARE,
    LOADING,
    LOAD_END,
    PLAYING,
    FAILED,
    FADEOUT,
};

class ScenePlay : public vScene
{
private:
    ePlayState _state;
    ePlayMode _mode;
    InputMask _inputAvailable;
    StringPath _chartPath;
    std::vector<size_t> _currentKeySample;
    std::mutex _mutex;

private:
    bool _isHoldingStart = false;
    bool _isHoldingSelect = false;
    std::array<bool, 2> _isFinished{ false };

public:
    ScenePlay(ePlayMode);
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
    void removeInputJudgeCallback();


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