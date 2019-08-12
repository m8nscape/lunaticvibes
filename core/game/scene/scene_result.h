#pragma once
#include <mutex>
#include "scene.h"

enum class eResultState
{
    DRAW,
    STOP,
    RECORD,
    FADEOUT,
};

class SceneResult : public vScene
{
private:
    std::mutex _mutex;
    eResultState _state;
    InputMask _inputAvailable;

protected:
    bool _scoreSyncFinished = false;
    bool _retryRequested = false;

public:
    SceneResult();
    virtual ~SceneResult() = default;

protected:
    // Looper callbacks
    virtual void _updateAsync() override;
    void updateDraw();
    void updateStop();
    void updateRecord();
    void updateFadeout();

protected:
    // Register to InputWrapper: judge / keysound
    void inputGamePress(InputMask&, timestamp);
    void inputGameHold(InputMask&, timestamp);
    void inputGameRelease(InputMask&, timestamp);
};