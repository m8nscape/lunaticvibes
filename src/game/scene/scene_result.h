#pragma once
#include "scene.h"

namespace lunaticvibes
{

class ScoreBase;
enum class eResultState
{
    DRAW,
    STOP,
    RECORD,
    FADEOUT,
    WAIT_ARENA,
};

class SceneResult : public SceneBase
{
private:
    eResultState state;
    InputMask _inputAvailable;

protected:
    bool _scoreSyncFinished = false;
    bool _retryRequested = false;
    std::shared_ptr<ScoreBase> _pScoreOld;

    bool saveScore = false;
    LampType saveLampMax;

public:
    SceneResult();
    virtual ~SceneResult();

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    void updateDraw();
    void updateStop();
    void updateRecord();
    void updateFadeout();
    void updateWaitArena();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, const Time&);
    void inputGameHold(InputMask&, const Time&);
    void inputGameRelease(InputMask&, const Time&);
};

}
