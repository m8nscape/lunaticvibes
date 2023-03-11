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
    WAIT_ARENA,
};

class ScenePlay : public SceneBase
{
private:
	std::future<void> _loadChartFuture;

private:
    ePlayState state;
    InputMask _inputAvailable;
    std::vector<size_t> keySampleIndex;

protected:
    bool isPlaymodeDP() const;

private:
    bool playInterrupted = false;
    bool playFinished = false;
    std::array<bool, 2>     playerFinished{ false, false };

    std::array<bool, 2>     playerHoldingStart = { false, false };
    std::array<bool, 2>     playerHoldingSelect = { false, false };
    std::array<Time, 2>     playerStartPressedTime = { TIMER_NEVER, TIMER_NEVER };
    std::array<Time, 2>     playerSelectPressedTime = { TIMER_NEVER, TIMER_NEVER };

    std::array<double, 2>   playerTurntableAngleAdd{ 0 };

    std::array<AxisDir, 2>  playerScratchDirection{ 0 };
    std::array<Time, 2>     playerScratchLastUpdate{ TIMER_NEVER, TIMER_NEVER };
    std::array<double, 2>   playerScratchAccumulator = { 0, 0 };

    std::array<double, 2>   playerLockspeedValueInternal{ 0 };  // internal use only, for precise calculation
    std::array<double, 2>   playerLockspeedHispeedBuffered{ 0 };
    std::array<int, 2>      playerLockspeedGreenNumber{ 0 };    // green number integer

    std::array<int, 2>      playerHispeedAddPending{ 0 };
    std::array<int, 2>      playerLanecoverAddPending{ 0 };

    std::array<double, 2>   playerSavedHispeed{ 1.0, 1.0 };

    std::array<Option::e_lane_effect_type, 2>     playerOrigLanecoverType;

    std::array<int, 2>      playerHealthLastTick{ 0 };

    std::array<bool, 2>     playerHispeedHasChanged{ false, false };
    std::array<bool, 2>     playerLanecoverTopHasChanged{ false, false };
    std::array<bool, 2>     playerLanecoverBottomHasChanged{ false, false };
    std::array<bool, 2>     playerLanecoverStateHasChanged{ false, false };
    std::array<bool, 2>     playerLockSpeedResetPending{ false, false };

    Time delayedReadyTime = 0;
    int retryRequestTick = 0;

    std::vector<ReplayChart::Commands>::iterator itReplayCommand;
    InputMask replayKeyPressing;
    unsigned replayCmdMapIndex = 0;

    bool isManuallyRequestedExit = false;
    bool isReplayRequestedExit = false;

    std::array<int, 2>      playerJudgeBP = { 0 };  // used for displaying poor bga
    Time poorBgaStartTime;
    int poorBgaDuration;

    double hiSpeedMinSoft = 0.25;
    double hiSpeedMinHard = 0.01;
    double hiSpeedMax = 10.0;
    double hiSpeedMargin = 0.25;
    int lanecoverMargin = 100;
    bool adjustHispeedWithUpDown = false;
    bool adjustHispeedWithSelect = false;
    bool adjustLanecoverWithStart67 = false;
    bool adjustLanecoverWithMousewheel = false;
    bool adjustLanecoverWithLeftRight = false;

public:
    ScenePlay();
    virtual ~ScenePlay();
    void clearGlobalDatas();
    bool createChartObj();
    bool createRuleset();

protected:
    void setInitialHealthBMS();

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
	constexpr double getWavLoadProgress() { return (wavTotal == 0) ? (gChartContext.isSampleLoaded ? 1.0 : 0.0) : (double)wavLoaded / wavTotal; }
	constexpr double getBgaLoadProgress() { return (bmpTotal == 0) ? (gChartContext.isBgaLoaded ? 1.0 : 0.0) : (double)bmpLoaded / bmpTotal; }

    void setInputJudgeCallback();
    void removeInputJudgeCallback();


protected:
    // loading indicators
    bool chartObjLoaded = false;
    bool rulesetLoaded = false;
    //bool _sampleLoaded = false;
    //bool _bgaLoaded = false;
    unsigned wavLoaded = 0;
    unsigned wavTotal = 0;
    unsigned bmpLoaded = 0;
    unsigned bmpTotal = 0;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    void updateAsyncLanecover(const Time& t);
    void updateAsyncHSGradient(const Time& t);
    void updateAsyncGreenNumber(const Time& t);
    void updateAsyncGaugeUpTimer(const Time& t);
    void updateAsyncLanecoverDisplay(const Time& t);
    void updateAsyncAbsoluteAxis(const Time& t);
    void updatePrepare();
    void updateLoading();
    void updateLoadEnd();
    void updatePlaying();
    void updateFadeout();
    void updateFailed();
    void updateWaitArena();

protected:
    // Inner-state updates
    void procCommonNotes();
    void changeKeySampleMapping(const Time& t);
    void spinTurntable(bool startedPlaying);
    void requestExit();
    void toggleLanecover(int slot, bool state);

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameHold(InputMask&, const Time&);
    void inputGameRelease(InputMask&, const Time&);
    void inputGamePressTimer(InputMask&, const Time&);
    void inputGameReleaseTimer(InputMask&, const Time&);
    void inputGameAxis(double s1, double s2, const Time&);

protected:
    bool imguiShowAdjustMenu = false;
    int imguiAdjustBorderX = 640;
    int imguiAdjustBorderY = 480;
    int imguiAdjustBorderSize = 50;
    bool imguiAdjustIsDP = false;
    bool imguiAdjustHas2P = false;

    virtual void updateImgui() override;
    void imguiInit();
    void imguiAdjustMenu();
};