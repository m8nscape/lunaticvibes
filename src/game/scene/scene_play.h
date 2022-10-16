#pragma once
#include <variant>
#include <mutex>
#include <future>
#include "scene.h"
#include "scene_context.h"
#include "common/chartformat/chartformat.h"
#include "game/chart/chart.h"

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
	std::future<void> _loadChartFuture;

private:
    ePlayState _state;
    InputMask _inputAvailable;
    std::vector<size_t> _currentKeySample;

protected:
    bool isPlaymodeDP() const;

private:
    bool _isExitingFromPlay = false;
    std::array<bool, 2>     _isPlayerFinished{ false, false };

    std::array<bool, 2>     _isHoldingStart = { false, false };
    std::array<bool, 2>     _isHoldingSelect = { false, false };
    std::array<Time, 2>     _startPressedTime = { TIMER_NEVER, TIMER_NEVER };
    std::array<Time, 2>     _selectPressedTime = { TIMER_NEVER, TIMER_NEVER };

    std::array<int, 2>      _ttAngleTime{ 0 };
    std::array<double, 2>   _ttAngleDiff{ 0 };

    std::array<AxisDir, 2>  _scratchDir{ 0 };
    std::array<Time, 2>     _scratchLastUpdate{ TIMER_NEVER, TIMER_NEVER };
    std::array<double, 2>   _scratchAccumulator = { 0, 0 };

    std::array<double, 2>   _lockspeedValue{ 0 };           // internal use only, for precise calculation
    std::array<int, 2>      _lockspeedGreenNumber{ 0 };     // green number integer

    std::array<int, 2>      _hispeedAdd{ 0 };
    std::array<int, 2>      _lanecoverAdd{ 0 };

    std::array<double, 2>   _hispeedOld{ 1.0, 1.0 };
    std::array<bool, 2>     _laneEffectHIDDEN { false, false };
    std::array<bool, 2>     _laneEffectSUDHID { false, false };

    Time _readyTime = 0;
    int retryRequestTick = 0;

    std::vector<ReplayChart::Commands>::iterator itReplayCommand;
    InputMask replayKeyPressing;

    std::array<int, 2>      _missPlayer = { 0 };
    Time _missLastTime;
    int _missBgaLength;

public:
    ScenePlay();
    virtual ~ScenePlay();
    void clearGlobalDatas();
    bool createChartObj();
    bool createRuleset();

protected:
    void setTempInitialHealthBMS();

private:
    std::array<size_t, 128> _bgmSampleIdxBuf{};
    std::array<size_t, 128> _keySampleIdxBuf{};

private:
	//std::map<size_t, std::variant<std::monostate, pVideo, pTexture>> _bgaIdxBuf{};
	//std::map<size_t, std::list<std::shared_ptr<SpriteVideo>>> _bgaVideoSprites{};	// set when loading skins, to bind videos while loading chart
	//size_t bgaBaseIdx = -1u;
	//size_t bgaLayerIdx = -1u;
	//size_t bgaPoorIdx = -1u;
	//pTexture bgaBaseTexture;
	//pTexture bgaLayerTexture;
	//pTexture bgaPoorTexture;
public:
	//void bindBgaVideoSprite(size_t idx, std::shared_ptr<SpriteVideo> pv) { _bgaVideoSprites[idx].push_back(pv); }

protected:
    // common
    void loadChart();
	constexpr double getWavLoadProgress() { return (_wavToLoad == 0) ? (_wavLoaded ? 1.0 : 0.0) : (double)_wavLoaded / _wavToLoad; }
	constexpr double getBgaLoadProgress() { return (_bmpToLoad == 0) ? (_wavLoaded ? 1.0 : 0.0) : (double)_bmpLoaded / _bmpToLoad; }

    void setInputJudgeCallback();
    void removeInputJudgeCallback();


protected:
    // loading indicators
    bool _chartLoaded = false;
    bool _rulesetLoaded = false;
    //bool _sampleLoaded = false;
    //bool _bgaLoaded = false;
    unsigned _wavLoaded = 0;
    unsigned _wavToLoad = 0;
    unsigned _bmpLoaded = 0;
    unsigned _bmpToLoad = 0;

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
    void procCommonNotes();
    void changeKeySampleMapping(const Time& t);
    void spinTurntable(bool startedPlaying);
    void requestExit();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameHold(InputMask&, const Time&);
    void inputGameRelease(InputMask&, const Time&);
    void inputGamePressTimer(InputMask&, const Time&);
    void inputGameReleaseTimer(InputMask&, const Time&);
    void inputGameAxis(double s1, double s2, const Time&);
};